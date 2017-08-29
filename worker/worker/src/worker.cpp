#include <worker/worker.h>
#include <worker/exceptions.h>

std::mutex acout::_mutexCout{};

worker::worker(threadFunc func) {
  m_func = func;
  m_id = 0;
  m_status = worker::threadStatus::ready;
}

worker::~worker() {
  if(m_status != worker::threadStatus::joined) {
    try{
      m_thread.join();
    }
    catch(const std::system_error& e) {
      acout{} << "Caught system_error : " << e.what() << std::endl;
    }
  }
}

void worker::start(int id) {
  if(m_status!=worker::threadStatus::ready)
    throw WorkerStateException{};
  std::unique_lock<std::mutex> lock(m_mutex);
  m_thread = std::thread(m_func, this);
  m_status = worker::threadStatus::running;
  m_id = id;
}

void worker::wait() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while(m_status==worker::threadStatus::paused) {
    m_condition.wait(lock);
  }
}

bool worker::forceStop() {
  std::unique_lock<std::mutex> lock(m_mutex);
  return (m_status==worker::threadStatus::stopped);
}

void worker::stop() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_status=worker::threadStatus::stopped;
  m_condition.notify_all();
  acout{} << "worker " << m_id << " stopped" << std::endl;
}

void worker::pause() {
  if(m_status!=worker::threadStatus::running)
    throw WorkerStateException{};
  std::unique_lock<std::mutex> lock(m_mutex);
  acout{} << "worker " << m_id << " paused" << std::endl;
  m_status=worker::threadStatus::paused;
}

void worker::restart() {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_status!=worker::threadStatus::paused)
      throw WorkerStateException{};
    m_status = worker::threadStatus::running;
  }
  m_condition.notify_all();
  acout{} << "worker " << m_id << " re-started" << std::endl;
}

void worker::join() {
  if(m_status != worker::threadStatus::joined) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_status = worker::threadStatus::joined;
    m_thread.join();
  }
}

bool worker::isRunning() {
  std::unique_lock<std::mutex> lock(m_mutex);
  return (m_status == worker::threadStatus::running);
}

void worker::finalize() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_status = worker::threadStatus::finished;
}

worker::threadStatus worker::status() {
  return m_status;
}

std::string worker::printStatus() {
  switch (m_status) {
  case worker::threadStatus::ready :
    return "ready";
  case worker::threadStatus::running :
    return "running";
  case worker::threadStatus::paused :
    return "paused";
  case worker::threadStatus::finished :
    return "finished";
  case worker::threadStatus::stopped :
    return "stopped";
  case worker::threadStatus::joined :
    return "joined";
  default :
    return "underfined";
  }
}

int worker::id() {
  return m_id;
}
