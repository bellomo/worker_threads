#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>

class acout : public std::ostringstream {
public:

  acout() = default;

  ~acout() {
    std::lock_guard<std::mutex> guard(_mutexCout);
    std::cout << this->str();
  }

private:
  static std::mutex _mutexCout;
};
