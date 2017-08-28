#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <assert.h>
#include <iterator>

using namespace std;


void helper() {
  cout<<"Usage : program ... "<<endl;
}

class Thread;
void doSomething(Thread* t);



////////////////////////////////////////////////////////////////////////////////////////
class acout: public std::ostringstream
{
public:
    acout() = default;

    ~acout()
    {
        std::lock_guard<std::mutex> guard(_mutexCout);
        std::cout << this->str();
    }

private:
    static std::mutex _mutexCout;
};
std::mutex acout::_mutexCout{};
////////////////////////////////////////////////////////////////////////////////////////


class Thread {

public:
  typedef void (*threadFunc)(Thread*);
  enum threadStatus{ready, running, paused, stopped, finished};
  
  Thread(threadFunc func) {
    m_func = func;
    m_id = 0;
    m_status = threadStatus::ready;
  }

  ~Thread() {
    m_thread.join();
  }

  void start(int id) {
    unique_lock<mutex> lock(m_mutex);
    assert(m_status==threadStatus::ready);
    m_thread = thread(m_func, this);
    m_status = threadStatus::running;	
    m_id = id;
  }

  void wait() {
    unique_lock<mutex> lock(m_mutex);
    while(m_status==threadStatus::paused) {
      m_condition.wait(lock);
    }
  }

  bool forceStop() {
    unique_lock<mutex> lock(m_mutex);
    return (m_status==threadStatus::stopped);
  }
  
  void stop() {
    unique_lock<mutex> lock(m_mutex);
    m_status=threadStatus::stopped;    
    //m_thread.join();
    acout{} << "Thread " << m_id << " stopped" << endl;
  }
  
  void pause() {
    assert(m_status==threadStatus::running);
    acout{} << "Thread " << m_id << " paused" << endl;
    m_status=threadStatus::paused;
  }

  void restart() {
    {
      unique_lock<mutex> lock(m_mutex);
      assert(m_status==threadStatus::paused);
      m_status = threadStatus::running;
    }
    m_condition.notify_all();
    acout{} << "Thread " << m_id << " re-started" << endl;
  }
  
  void join() {
    m_thread.join();
  }

  bool isRunning() {
      unique_lock<mutex> lock(m_mutex);
      return (m_status == threadStatus::running);
  }
  
  threadStatus status() {
    return m_status;
  }

  string printStatus() {
    switch (m_status) {
    case threadStatus::ready :
      return "ready";
    case threadStatus::running :
      return "running";
    case threadStatus::paused :
      return "paused";
    case threadStatus::finished :
      return "finished";
    case threadStatus::stopped :
      return "stopped";
    default :
      return "underfined";
    }
  }

  int id() {
    return m_id;
  }
  
  
private:

  int m_id;
  threadStatus m_status;
  threadFunc  m_func;
  thread m_thread;
  mutex m_mutex;
  condition_variable m_condition;
};


int main(int argc, char* argv[]) {

  if( argc==1 || ( argc==2 && !strcmp(argv[1],"--help") ) ) {
    helper();
    return 0;
  }

  int nb_threads_to_run=0;
  
  if(!strcmp(argv[1],"--threads"))
    nb_threads_to_run = atoi(argv[2]);
  else
    helper();

 
  vector<Thread*> workers;

  for(unsigned int i=0; i<nb_threads_to_run; ++i) {
    workers.push_back ( new Thread(doSomething) );
  }
  
  for(unsigned int i=0; i<nb_threads_to_run; ++i) {
    workers[i]->start(i+1);
  }

  while( [&] () -> bool {bool run=false;
      for(auto wk : workers) {
	run = run | wk->isRunning();}
      return run;} () ) {

    string command="";
    getline(cin, command);

    if(command == "status") {
      for(auto wk : workers)
 	cout<<"Thread " << wk->id() << " status " << wk->printStatus() << endl;      
    }

    else if(command == "exit") {
      break;
    }

    else{
      istringstream iss(command);
      vector<string> tokens{istream_iterator<string>{iss},istream_iterator<string>{}};

      if (tokens.size() != 2)
	helper();

      else if( !(tokens[1].find_first_not_of( "0123456789" ) == string::npos))
	helper();
      
      else{
	int wId = atoi(tokens[1].c_str());
	if( wId > nb_threads_to_run)
	  helper();
	else if(tokens[0] == "pause") {
	  workers[wId-1]->pause();
	}
	else if(tokens[0] == "restart") {
	  workers[wId-1]->restart();
	}
	else if(tokens[0] == "stop") {
	  workers[wId-1]->stop();
	}
      }
    }
    
    // if(s=="pause")
    // else if(s=="restart")
    // else if(s=="stop")
    // else if(s=="status")
  }


  
  for(auto wk : workers)
    wk->join();


  // delete workers...

  return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
void doSomething(Thread* t) {
  int tot=0;
  for(int i=0; i<10; ++i) { 
    for(int j=0; j<10; ++j) {
      
      t->wait();

      if(t->forceStop())
	 return;
      
      for(int k=0; k<10; ++k) {		
	tot+=i;
	tot+=j;
	tot+=k;
      }
    }
  }
}
