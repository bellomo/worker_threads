#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <worker/worker.h>
#include <worker/exceptions.h>
using namespace std;

void doWriteFile(worker* w) {
  stringstream workerIdStringStream;
  workerIdStringStream << w->id();
  ofstream logfile (("worker_id"+workerIdStringStream.str()+"_heartbeat.txt").c_str());
  if (logfile.is_open()) {
    while (true) {
      auto time_point = chrono::system_clock::now();
      time_t now_c = chrono::system_clock::to_time_t(time_point);
      logfile << "Worker Id " << w->id() <<" : "<< ctime(&now_c) << endl;
      w->wait();
      if(w->forceStop()) {
        logfile.close();
        w->finalize();
        return;
      }
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
  }
}

void doLongLoop(worker* w) {
  int tot=0;
  int ncycles=10000;
  for(int i=0; i<ncycles; ++i) {
    for(int j=0; j<ncycles; ++j) {

      w->wait();

      if(w->forceStop())
	     return;

      for(int k=0; k<ncycles; ++k) {
	       tot+=i;
	       tot+=j;
	       tot+=k;
      }
    }
  }

  w->finalize();
}

void helper() {
  cout<<"Usage : program --help         # print this helper"<<endl;
  cout<<"                --threads N    # start N worker threads"<<endl<<endl;
  cout<<"                Interacting commands: "<<endl;
  cout<<"                status       # print worker threads status"<<endl;
  cout<<"                pause i      # pause ith worker thread (i goes from 1 to N)"<<endl;
  cout<<"                restart i    # restart ith worker thread (i goes from 1 to N)"<<endl;
  cout<<"                stop i       # stop ith worker thread (i goes from 1 to N)"<<endl;
  cout<<"                exit         # stop all worker threads and terminate program"<<endl;
}

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

  vector<worker*> workers;

  for(int i=0; i<nb_threads_to_run; ++i) {
    //workers.push_back ( new worker(doLongLoop) );
    workers.push_back ( new worker(doWriteFile) );
  }

  try{
    for(int i=0; i<nb_threads_to_run; ++i) {
      workers[i]->start(i+1);
    }
  }
  catch(WorkerStateException& e) {
    cout << e.what() << endl;
  }

  auto isWorkerRunning = [&] () -> bool {
    bool run=false;
    for(auto wk : workers) {
      run = run | wk->isRunning();
    }
    return run;
  };

  while( isWorkerRunning() ) {

    try{
      string command="";
      getline(cin, command);
      istringstream iss(command);
      vector<string> tokens{istream_iterator<string>{iss},istream_iterator<string>{}};

      if(tokens.empty()) {
        throw WorkerCommandException{};
      }

      if(tokens[0] == "status") {
        for(auto wk : workers)
          cout<<"worker " << wk->id() << " status " << wk->printStatus() << endl;
      }

      else if(tokens[0] == "exit") {
        for(auto wk : workers)
          wk->stop();
      }

      else if(tokens.size() == 2) {

        if( !(tokens[1].find_first_not_of( "0123456789" ) == string::npos)) {
          throw WorkerCommandException{};
        }

        try{
          int wId = atoi(tokens[1].c_str());
          if( wId > nb_threads_to_run)
            throw WorkerCommandException{};

          if(tokens[0] == "pause") {
            workers[wId-1]->pause();
          }
          else if(tokens[0] == "restart") {
            workers[wId-1]->restart();
          }
          else if(tokens[0] == "stop") {
            workers[wId-1]->stop();
          }
          else
            throw WorkerCommandException{};
          }
          catch(WorkerStateException& e){
            cout << e.what() << endl;
          }
        }
        else
          throw WorkerCommandException{};
      }
      catch(WorkerCommandException& e) {
        cout<<e.what()<<endl;
        cout<<"Available worker ids are from 1 to "<<nb_threads_to_run<<endl;
        helper();
      }
  }

  if(isWorkerRunning()) {
    for(auto wk : workers)
      wk->join();
  }

  for(auto wk : workers)
      delete wk;

  return 0;
}



/* Prossible extension wit a workerTask class derived from worker
   with a member function to be executed in the thread defined as
   below where runTask1, runTask2, ecc are steps of the thread tasks
   where pauses or stops are allowed.

   runTask {

     runTask1();

     wait();
     if(forceStop()) return;

     runTask2();

     wait();
     if(forceStop()) return;

      ...

     finalize();
  }
*/
