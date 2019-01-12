#include <stdexcept>
#include "slice.h"
#include <ncurses.h>

std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x)
{/*
  printw("offset + x = %d, buff.size() / y = %d ", offset+x, buff.size() / y);
  getch();
  printw("y = %d, x = %d, buff.size() = %d ", y, x, buff.size());
  getch();*/
  if((offset + x) > (buff.size() / y))
    {//check for error condition (can't scroll past end of background file).
      throw std::logic_error("error in (slice.cpp)->(std::vector<int> getSlice(std::vector<int> buff, unsigned long offset, int y, int x)): (offset + x) > (array / y)!");
    }  
  std::vector<int> slice {};  
  for(int iterY {}; iterY < y; ++iterY)//column
    {
      for(int iterX {}; iterX < x; ++iterX)//row
	{//(offset + screen length) + (column number * level length)	  
	  slice.push_back(buff[(offset + iterX) + (iterY * (buff.size() / y))]);
	}
    }
  return slice;
}
