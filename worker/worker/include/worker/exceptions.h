// using standard exceptions
#include <iostream>
#include <exception>
using namespace std;

class WorkerStateException: public exception {
public:
  virtual const char* what() const throw() {
    return "WorkerStateException : Change of worker state request failed, verify worker state (status)";
  }
};

class WorkerCommandException: public exception {
  public:
    virtual const char* what() const throw() {
      return "WorkerCommandException : Unexpected interactive command";
    }
};
