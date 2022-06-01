#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/utils.hpp"


void sleep(const unsigned long long t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


bool checkRange(const int a, const int min, const int max)
{ 
  return (a >= min && a < max) ? true : false;
}


bool inSingleDigitRange(const int a, const int offset)
{
  constexpr int SINGLE_DIGIT_MIN {};
  constexpr int SINGLE_DIGIT_MAX {9};
  if(((a - offset) < SINGLE_DIGIT_MIN) || ((a - offset) > SINGLE_DIGIT_MAX))
    return false;
  return true;
}


bool loadFileIntoString(const char name [], std::string & buff)
{
  std::ifstream file {};
  file.open(name);
  if(!file.is_open())
    {				// Couldn't open file.
      file.close();
      return false;
    }
  char c;
  while(file.get(c))		// Copy file content's to buff.
    buff.push_back(c);
  file.close();
  
  return true;
}


std::string skipSpaceUpTo(const std::string & buff,
			  std::string::const_iterator & buffPos,
			  std::vector<std::string> & targets)
{
  sort(targets.begin(), targets.end()); // Sort alphabetically.
  stable_sort(targets.begin(), targets.end(),
	      [](const std::string & first, const std::string & second)
	      {
		return first.size() > second.size();
	      });			// Stable sort by length in desc order.
  
  std::string::const_iterator outerPeekPos {buffPos};
  std::string retVal {};

  // Iterate over characters of buff and check for targets.
  while(outerPeekPos != buff.end())
    {
      // Iterate over targets.
      for(int targetsIter {}; targetsIter < targets.size();
	  ++targetsIter)
	{
	  std::string::const_iterator peekPos {};
	  peekPos = outerPeekPos;

	  std::cout<<"Checking target \""<<targets[targetsIter]<<"\"\n";
	  
	  bool found {true};
	  // Iterate over characters in target.
	  for(int targetIter {}; targetIter < targets[targetsIter].size();
	      ++targetIter)
	    {
	      std::cout<<"\t*peekPos = "<<*peekPos
		       <<", targets[targetsIter][targetIter] = "
		       <<targets[targetsIter][targetIter]<<", targets[targetIter].size() = "<<targets[targetsIter].size()<<"\n";
	      if(*peekPos != targets[targetsIter][targetIter])
		{
		  found = false;
		  break;
		}
	      peekPos++;
	    }
	  if(found)
	    {
	      retVal = targets[targetsIter];
	      goto RETURN;
	    }
	}
      outerPeekPos++;
    }

 RETURN:
  buffPos = ++outerPeekPos;
  return retVal;
}


/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status)
{
  endwin();
  std::cerr<<e<<'\n';
  exit(status);
}
