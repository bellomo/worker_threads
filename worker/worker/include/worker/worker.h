#pragma once

#include <thread>
#include <string>
#include <worker/acout.h>

class worker {

public:
  typedef void (*threadFunc)(worker*);
  enum threadStatus{ready, running, paused, stopped, finished, joined};

  worker(threadFunc func);
  ~worker();
  void start(int id);
  void wait();
  bool forceStop();
  void stop();
  void pause();
  void restart();
  void join();
  bool state(worker::threadStatus s);
  void finalize();
  threadStatus status();
  std::string printStatus();
  int id();

private:
  int m_id;
  threadStatus m_status;
  threadFunc  m_func;
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_condition;
};
