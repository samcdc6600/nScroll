#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/utils.hpp"


yx abs(const yx a)
{
  return yx{abs(a.y), abs(a.x)};
}


std::ostream & operator<<(std::ostream &lhs, const yx rhs)
{
  return lhs<<"("<<rhs.y<<", "<<rhs.x<<")";
}


/* NOTE THAT WE DECLARE THIS HEADING HERE BECAUSE A FUNCTION WITH THE SAME
   SIGNATURE (OR AT LEAST THIS WOULD BE THE CASE, BUT APPARENTLY THE LINKER
   DOESN'T LIKE IT WHEN TWO FUNCTIONS HAVE THE SAME SIGNATURE EVEN IF THEIR
   SCOPE DOESN'T OVERLAP.) IS DECLARED IN levelRules.hpp AND WE ONLY NEED THIS
   FUNCTION IN THIS FILE. SkipSpace suppresses the skipping of spaces before the
   opening
   terminal "(". */
void readSingleCoordSectionUtils
(const std::string & buff,
 std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers,
 void *coord, const std::string typeOfNumber,
 const bool skipSpace = true);
/* NOTE AGAIN THAT AS WITH THE ABOVE A FUNCTION WITH THE SAME SIGNATURE IS
   DEFINED ELSE WHERE. Attempts to read the bracket at the start of a section. Calls exit
   with eMsg and section if there is an error. */
void readSectionOpeningBracketUtils
(const std::string &buff, std::string::const_iterator &buffPos,
 const std::string &eMsg, const std::string &section, const bool skipSpace);
/* NOTE AGAIN THAT AS WITH THE ABOVE A FUNCTION WITH THE SAME SIGNATURE IS
   DEFINED ELSE WHERE. Attempts to read the bracket at the end of a
   section. Calls exit with eMsg and section if there is an error. */
void readSectionEndingBracketUtils
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section);


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
/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places than this. */
constexpr int MAX_COORD_LEN{10};  


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


void loadFileIntoString(const char name [], std::string & buff,
			const std::string & eMsg)
{
  std::ifstream file {};
  file.open(name);
  if(!file.is_open())
    {				// Couldn't open file.
      file.close();
      exit(concat("Error: opening file \"", name, "\" when ", eMsg, "."),
	   ERROR_OPENING_FILE);
    }
  char c;
  while(file.get(c))		// Copy file content's to buff.
    {
      buff.push_back(c);
    }
  file.close();

  if(buff.size() == 0)
    {
      exit(concat("Error: found file \"", name, "\" to be empty when ", eMsg,
		  "."), ERROR_OPENING_FILE);
    }
}


bool getChunkCoordinate
  (const std::string & data, std::string::const_iterator & buffPos,
   const std::string & eMsg, yx & chunkCoord)
{
  if(buffPos != std::end(data))
    {
      readSingleCoordSectionUtils
	(data, buffPos, eMsg, true, & chunkCoord, "integers (without "
	 "skipping space up until the coordinate)", false);
      return true;
    }
  else
    {
      return false;
    }
}


// SkipSpace has a default value.
void readSingleCoordSectionUtils
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers,
 void * coord, const std::string typeOfNumber, const bool skipSpace)
{
  constexpr char COORD_SEPARATION {','};
  std::vector<std::string> targets {};
  std::string targetFound {};
  readSectionOpeningBracketUtils
    (buff, buffPos, eMsg,
     concat("single coordinate section (with " , typeOfNumber, ")"),
     skipSpace);
  
  ((yx*)coord)->y = readSingleNum(buff, buffPos, eMsg, useIntegers);
      
  targets = {std::string {COORD_SEPARATION}};
  targetFound = skipSpaceUpTo
    (buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<COORD_SEPARATION<<"\" before second coordinate "
	"component in single coordinate section (with "<<typeOfNumber
       <<") when "<<eMsg<<". Encountered "<<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  ((yx*)coord)->x = readSingleNum(buff, buffPos, eMsg, useIntegers);

  readSectionEndingBracketUtils
    (buff, buffPos, eMsg,
     concat("single coordinate section (with ", typeOfNumber, ")"));
}


// SkipSpace has a default value.
void readSectionOpeningBracketUtils
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section, const bool skipSpace)
{
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  std::vector<std::string> targets {};
  std::string targetFound {};
      
  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets, skipSpace);
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<"\" to "
	"denote the start of "<<section<<" when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\"\n";
      
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


void readSectionEndingBracketUtils
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg,  const std::string & section)
{
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets = {std::string {RULES_HEADER_SECTION_END_DENOTATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<"\" to "
	"denote the end of "<<section<<" when "<<eMsg<<". Encountered "<<"\""
       <<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


bool readInChunkCoordFromFile
(yx & retVal, const std::string & fileName, std::fstream & file,
 const bool exitOnError)
{
  auto intIn = [& fileName, & file, exitOnError] (int & coord)
  {
    char inputCoord [sizeof(int)];

    if(!file.read(inputCoord, sizeof(int)))
      {
	if(exitOnError)
	  {
	    exit(concat("Error: trying to read int from \"", fileName, "\". "
			"The file size may be of note."),
		 ERROR_MALFORMED_FILE);
	  }
	else
	  {
	    return false;
	  }
      }

    coord = 0;

    for(int iter {}; (size_t)iter < sizeof(int); iter++)
      {
	coord |= (static_cast<int>(inputCoord[iter]) & 0xFF) << (iter * 8);
      }

    return true;
  };

  bool ret;

  if((ret = intIn(retVal.y)))
    {
      ret = intIn(retVal.x);
    }
  
  return ret;
}


int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers)
{
  std::vector<std::string> targets
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  if(useIntegers)
    {
      targets.push_back("-");
    }
  std::string targetFound {};
  std::stringstream number {};

  // Skip space up until the start of the number.
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      exit(concat("Error: expected ", useIntegers ? "integer": "natural",
		  " number when ", eMsg, ". Encountered \"", *buffPos, "\".\n"),
	   ERROR_RULES_LEV_HEADER);
    }

  int decPlacesIter {};
  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
      // There must be at least 1 digit after a negative sign.
      if(isNum(*buffPos))
	{
	  number<<*buffPos;
	  decPlacesIter++;
	  buffPos++;
	}
      else
	{
	  exit(concat("Error: expected integer number when ", eMsg,
		      ". Encountered \"", *buffPos, "\".\n"),
	       ERROR_RULES_LEV_HEADER);
	}
    }
  
  // Read in number.
  while(isNum(*buffPos))
    {
      number<<*buffPos;
      decPlacesIter++;
      buffPos++;
    }
  
  // Check if number was too long.
  if(decPlacesIter > MAX_COORD_LEN)
    {
      exit(concat
	   ("Error: number \"", number.str(), "\" too long (longer than \"",
	    MAX_COORD_LEN, "\") when ", eMsg,
	    ". Encountered \"", *buffPos, "\".\n").c_str(),
	   ERROR_RULES_LEV_HEADER);
    }

  int ret;
  number>>ret;
  return ret;
}


double readSingleRNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg)
{
  std::vector<std::string> targets
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", "."};
  std::string targetFound {};
  std::stringstream number {};

  // Skip space up until the start of the number.
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      exit(concat("Error: expected real number when ", eMsg, ". Encountered "
		  "\"", *buffPos, "\".\n"), ERROR_RULES_LEV_HEADER);
    }

  int decPlacesIter {};
  bool foundDigit {false};
  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
      // Can't have a negative sign after a negative sign.
      targets.erase(std::remove(targets.begin(), targets.end(), "-"),
		    targets.end());
    }
  else if(targetFound == ".")
    {
      number<<*buffPos;
      ++buffPos;
      // Can't have a negative sign after the decimal or another decimal.
      targets.erase(std::remove(targets.begin(), targets.end(), "."),
		    targets.end());
      targets.erase(std::remove(targets.begin(), targets.end(), "-"),
		    targets.end());
    }

  // Read in number.
  while(isNum(*buffPos) || *buffPos == '.')
    {
      if(*buffPos == '.')
	{
	  if(std::find(targets.begin(), targets.end(), ".") == targets.end())
	    {
	      // We've encountered a second '.'!
	      break;
	    }
	  targets.erase(std::remove(targets.begin(), targets.end(), "."),
			targets.end());
	}
      else
	{
	  foundDigit = true;
	  decPlacesIter++;
	}
      number<<*buffPos;
      buffPos++;
    }

  if(!foundDigit)
    {
      exit(concat("Error: expected real number when ", eMsg, ". Encountered \"",
		  *buffPos, "\".\n"),
	   ERROR_RULES_LEV_HEADER);
    }
  // Check if number was too long.
  if(decPlacesIter > MAX_COORD_LEN)
    {
      exit(concat
	   ("Error: number \"", number.str(), "\" too long (longer than \"",
	    MAX_COORD_LEN, "\") when ", eMsg,
	    ". Encountered \"", *buffPos, "\".\n").c_str(),
	   ERROR_RULES_LEV_HEADER);
    }

  double ret;
  number>>ret;
  return ret;
}


void getChunk(const std::string & data,
	      std::string::const_iterator & buffPos, const std::string & eMsg,
	      std::string & chunk, const yx expectedChunkSize)
{
  chunk.clear();

  int lnCount {};
  if(buffPos != std::end(data))
    {
      while(true)
	{
	  char newCh {};
	  newCh = *buffPos++;
	  lnCount += (newCh == '\n' ? 1: 0);
	  
	  if(lnCount >= expectedChunkSize.y)
	    {
	      break;
	    }
	  else 
	    {
	      chunk += newCh;
	      if(buffPos == std::end(data))
		{
		  break;
		}
	    }
      
	}
    }

  if(lnCount != expectedChunkSize.y)
    {
      exit(concat
	   ("Error: Wrong number of lines. Expected ", expectedChunkSize.y,
	    ", but found ", lnCount, " when ", eMsg), ERROR_BACKGROUND);
    }
}


std::string createChunkCoordKey(const yx coord)
{
    /* ',' must be included to delineate between the y and x
     coordinates. */
    return concat("", coord.y, ",", coord.x);
}


std::string createChunkCoordKeyFromCharCoord(const yx chunkCoord)
{
  /* If y or x is less than 0 we must sub one of the view port dimension (minus
     1) sizes from the value before devision otherwise the result will be
     wrong. E.g. -50 / 170 = 0 and what we need is -1. This is because 50 / 170
     is also 0, but these two coordinates aren't in the same chunks. Minus 1
     because the negative chunks don't have a coordinate zero and as such
     -yHeight or -xHeight both have coordinate -1 where as yHeight and
     xHeight both have coordinate 1 but (yHeight -1) and (xHeight -1) both have
     coordinate 0. */
  yx chunkCoordKey
    {(chunkCoord.y < 0 ? (chunkCoord.y - (yHeight -1)) / yHeight :
      chunkCoord.y / yHeight),
     (chunkCoord.x < 0 ? (chunkCoord.x - (xWidth -1)) / xWidth :
      chunkCoord.x / xWidth)};
  return createChunkCoordKey(chunkCoordKey);
}


bool checkForStringInBufferAtPos(const std::string & buff, int buffPos,
				 const std::string str)
{
  bool found {false};

  if((unsigned long)buffPos < buff.size() &&
     buff.size() - buffPos >= str.size())
    {
      int strIter {};
      for( ; buffPos < buff.size(); ++buffPos, ++strIter)
	{
	  if(buff[buffPos] != str[strIter])
	    {
	      break;
	    }	  
	}
      if((unsigned long)strIter == str.size())
	{

	  found = true;
	}
    }

  return found;
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


bool checkForPostfix(const char str [], const char postfix [])
{
  const char * postfixPos {strstr(str, postfix)};

  if(postfixPos == nullptr ||
     (size_t)((postfixPos + strlen(postfix)) - str) != strlen(str))
    {
      return false;
    }

  return true;
}


void updateVirtualNestedChunkYXLoop
    (int & xIter, int & yIter, const yx chunkSize)
{
  xIter++;
  if(xIter > chunkSize.x -1)
    {
      xIter = 0;
      yIter++;
    }
}


bool checkYOfVirtualNestedChunkLoop
(int & xIter, int & yIter, const yx chunkSize,
 const bool multiChunkFile, const std::string & eMsgStart)
{
  bool ret {true};
  if(yIter > chunkSize.y - 1)
    {
      if(!multiChunkFile)
	{
	  exit(concat(eMsgStart, "Chunk is too large, equal to or over ",
		      yIter * chunkSize.x + xIter +1, " It should be ",
		      chunkSize.y * chunkSize.x, "."),
	       ERROR_MALFORMED_FILE);
	}
      else
	{
	  ret = false;
	}
    }

  return ret;
}


/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status)
{
  endwin();
  std::cerr<<e<<'\n';
  exit(status);
}

