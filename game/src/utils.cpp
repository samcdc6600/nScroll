#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/utils.hpp"


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
	(data, buffPos, eMsg, false, & chunkCoord, "natural numbers (without "
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


int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers)
{
  // using namespace levelFileKeywords;

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
      std::stringstream e {};
      e<<"Error: expected non-negative number when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
    }

  int decPlacesIter {};
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
      std::stringstream e {};
      e<<"Error: number \""<<number.str()<<"\" too long (longer than \""
       <<MAX_COORD_LEN<<"\") when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  int numberRet;
  number>>numberRet;
  return numberRet;
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


std::string createChunkCoordKeyFromCharCoord(const yx charCoord)
{
  return createChunkCoordKey(yx(charCoord.y / yHeight, charCoord.x / xWidth));
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

