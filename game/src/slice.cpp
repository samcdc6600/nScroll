#include <stdexcept>
#include <ncurses.h>
#include "include/slice.hpp"
#include "include/common.hpp"


std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x)
{
  if((offset + x) > (buff.size() / y))
    { // Check for error condition (can't scroll past end of background file).
      // exit("error in (slice.cpp)->(std::vector<int> getSlice(std::vector<int> "
      // 	   "buff, unsigned long offset, int y,int x)): (offset + x) > (array / "
      // 	   "y)!", ERROR_GENERIC_RANGE_ERROR);
    }  
  std::vector<int> slice {};  
  for(int iterY {}; iterY < y; ++iterY)	// Column.
    {
      for(int iterX {}; iterX < x; ++iterX) // Row.
	{ // (offset + screen length) + (column number * level length)	  
	  slice.push_back(buff[(offset + iterX) + (iterY * (buff.size() / y))]);
	}
    }
  return slice;
}
