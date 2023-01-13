#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/utils.hpp"


namespace boarderRuleChars
{
  /* NOTE: when adding boarder rule characters you must also add them to the
     CHARS array and add the extern declarations to utils.hpp. */
  // The player cannot move through boarder chars.
  constexpr char BOARDER_CHAR = 'b';
  /* Same as boarder chars (except the player can move through them if
     moving up.) */
  constexpr char PLATFORM_CHAR = 'p';
  const std::vector<char> CHARS {BOARDER_CHAR, PLATFORM_CHAR};
};


void sleep(const unsigned long long t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


bool isNum(const char c)
{
  return (c >= '0' && c <= '9') ? true: false;
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



// SkipSpace has a default value.
std::string skipSpaceUpTo(const std::string & buff,
			  std::string::const_iterator & buffPos,
			  std::vector<std::string> & targets,
			  const bool skipSpace)
{
  std::string targetFound {};
  std::string::const_iterator outerPeekPos {buffPos};
  
  if(outerPeekPos != buff.end())
    {
      sort(targets.begin(), targets.end()); // Sort alphabetically.
      stable_sort(targets.begin(), targets.end(),
		  [](const std::string & first, const std::string & second)
		  {
		    return first.size() > second.size();
		  });			// Stable sort by length in desc order.

      // If the first char isn't white space.
      if(*outerPeekPos != ' ' &&
	 *outerPeekPos != '\n' &&
	 *outerPeekPos != '\r' &&
	 *outerPeekPos != '\t')
	{
	  // Check for targets at buffPos.
	  targetFound = findTargetInBuff(outerPeekPos, targets);
	  for(int iter {}; size_t(iter) < targetFound.size(); ++iter)
	    {
	      outerPeekPos++;
	    }
	}
      else
	{
	  // skip spaces.
	  while(outerPeekPos != buff.end() && skipSpace &&
		(*outerPeekPos == ' ' ||
		 *outerPeekPos == '\n' ||
		 *outerPeekPos == '\r' ||
		 *outerPeekPos == '\t'))
	    {
	      outerPeekPos++;
	    }

	  // Check for targets again.
	  if(outerPeekPos != buff.end())
	    {
	      targetFound = findTargetInBuff(outerPeekPos, targets);
	      for(int iter {}; size_t(iter) < targetFound.size(); ++iter)
		{
		  outerPeekPos++;
		}
	    }
	}
    }

  buffPos = outerPeekPos;
  return targetFound;
}


void skipSpaceUpToNextLine(const std::string & buff,
			   std::string::const_iterator & buffPos,
			   const std::string & eMsg)
{
  while(buffPos != std::end(buff))
    {
      if(*buffPos == '\n')
	{
	  break;
	}
      else if(*buffPos == ' ' || *buffPos == '\r' || *buffPos == '\t')
	{
	  buffPos++;
	}
      else
	{
	  goto ERROR_EXIT;
	}
    }

  if(buffPos == std::end(buff))
    {
    ERROR_EXIT:
      exit(eMsg.c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  // Can be equal to std::end(buff) after incrementing.
  buffPos++;
}


static std::string findTargetInBuff
(const std::string::const_iterator & outerPeekPos,
 const std::vector<std::string> & targets)
{
  std::string targetFound {};
  std::string::const_iterator peekPos {};
  // Iterate over targets.
  for(int targetsIter {}; size_t(targetsIter) < targets.size();
      ++targetsIter)
    {
      peekPos = outerPeekPos;
      bool found {true};
      
      // Iterate over characters in target.
      for(int targetIter {}; size_t(targetIter) < targets[targetsIter].size();
	  ++targetIter)
	{
	  if(*peekPos != targets[targetsIter][targetIter])
	    {
	      found = false;
	      break;
	    }
	  peekPos++;
	}
      if(found)
	{
	  targetFound = targets[targetsIter];
	  goto RETURN;
	}
    }

 RETURN:
  return targetFound;
}


/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status)
{
  endwin();
  std::cerr<<e<<'\n';
  exit(status);
}
