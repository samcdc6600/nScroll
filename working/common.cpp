#include <chrono>
#include <thread>
#include "common.h"


void sleep(const unsigned long long t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


bool checkRange(const int a, const int coord)
{
  bool ret = (a >= 0 && a < coord) ? true : false;
  return ret;
}
