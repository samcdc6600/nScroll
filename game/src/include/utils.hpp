#ifndef COMMON_H_
#define COMMON_H_


#include <string>
#include <vector>
#include <sstream>


struct yx
{
  yx() {}  
  yx(const int y, const int x)
  {
    this->y = y;
    this->x = x;
  }

  bool operator==(const yx & rhs)
  {
    if(y != rhs.y || x != rhs.x)
      {
	return false;
      }
    return true;
  }

  bool operator!=(const yx & rhs)
  {
    return !operator==(rhs);
  }

  int y;
  int x;
};


// The window must be these dimensions.
constexpr int yHeight {48}, xWidth {170};
constexpr int ASCII_CH_MAX {127};
constexpr int MONO_CH_MAX {158};
constexpr int COLOR_CH_MAX {63};
constexpr int ASCII_NUMBER_OFFSET {48};
constexpr char ESC_CHAR {27};
//constexpr int BACKGROUND_HEIGHT {33};


enum errorsCodes {                     /* Error codes. */
                   ERROR_WIN_PARAM,    // Window not initialised - there was a
                                       // problem with the window parameters.
                   ERROR_CURSOR_PARAM, // Cursor parameters out of range.
                   ERROR_SPRITE_POS_RANGE,  // Sprite position out of range.
                   ERROR_POS_CH_RANGE,      // Character position out of range.
                   ERROR_INVALID_DIRECTION, // Direction character found not to
                                            // be valid.
                   ERROR_OPENING_FILE,      // Error opening file.
                   ERROR_CHARACTER_RANGE,   // Character out of range.
                   ERROR_COLOR_CODE_RANGE,  // Color code out of range.
                   ERROR_GENERIC_RANGE_ERROR,
                   ERROR_BACKGROUND,          // Malformed BG file.
                   ERROR_RULES_LEV_HEADER,    // Header of .level.rules file is
                                              // malformed.
                   ERROR_RULES_STRING_FIELDS, // There was an error in a field
                                              // containing string's.
                   ERROR_MALFORMED_STRING, // We encountered a malformed string.
                   ERROR_MALFORMED_COORDINATE, // We have encountered a
                                               // malformed coordinate.
                   ERROR_DUPLICATE_COORDINATE, // We have encountered a
                                               // duplicate coordinate.
                   ERROR_MALFORMED_COORDINATE_CHAR_FIELD, // We have encountered
                                                          // a coordinate
                                                          // character field
                                                          // that is malformed.
                   ERROR_BAD_LOGIC // There was an error in the code.
};


namespace boarderRuleChars
{
  /* NOTE: when adding boarder rule characters you must add then to utils.cpp as
     well as adding the corresponding extern declarations here. */
  extern const char BOARDER_CHAR;
  extern const char PLATFORM_CHAR;
  extern const std::vector<char> CHARS;
};


void sleep(const unsigned long long t);
bool isNum(const char c);
/* Returns false if a is not range [min, max). */
bool checkRange(const int a, const int min, const int max);
/* Return's true if a - offset is within the range [SINGLE_DIGIT_MIN, SINGLE_DIGIT_MAX].
   Return's false otherwise. */
bool inSingleDigitRange(const int a, const int offset);
// Returns false if unable to open file at path name.
bool loadFileIntoString(const char name[], std::string &buff);
/* First checks if buffPos is in range. Returns false if it is not. Otherwise
   attempts to read the coordinates into chunkCoord. If this succeeds returns
   true (with chunkCoord being set to the coordinates read.) If there is a
   failure in reading the coordinates then the program will be aborted with eMsg
   being displayed. */
bool getChunkCoordinate
(const std::string & data, std::string::const_iterator & buffPos,
 const std::string & eMsg, yx & chunkCoord);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers);
void getChunk
(const std::string & data, std::string::const_iterator & buffPos,
 const std::string & eMsg, std::string & chunk, const yx expectedChunkSize);
/* Advances buffPos (past white space) until it reads one past a sequence of
   characters that matches a string in targets, where buffPos points to
   somewhere in buff and the strings in targets will be checked in order of the
   longest string to the shortest and where any two strings are the same length
   it will check the one that comes first alphabetically. Returns the string
   that was matched. Returns an empty string if no strings in targets were
   matched. BuffPos may be advanced up to buff.end() (inclusive). Spaces aren't
   skipped if skipSpace is set to true, however the rest of the behaviour
   remains the same. */
std::string skipSpaceUpTo(const std::string & buff,
                          std::string::const_iterator & buffPos,
                          std::vector<std::string> & targets,
                          const bool skipSpace = true);
/* Advances buffPos up to and past the next '\n' character. If the end of the
   buffer or any non white space characters are encountered before '\n' prints
   eMsg and terminates program. */
void skipSpaceUpToNextLine(const std::string & buff,
			   std::string::const_iterator & buffPos,
			   const std::string & eMsg);
/* Finds a target from targets in the buffer pointed to by
   outerPeekPos. Returns the target found. If no target was found returns "". */
static std::string
findTargetInBuff(const std::string::const_iterator & outerPeekPos,
		 const std::vector<std::string> & targets);
/* Calls endwin() then print's e to std::cerr and finally call's exit() with
   status */
void exit(const std::string & e, const int status);


template <typename T>
std::string concat(T newStr)
{
  return newStr;
}

/* This template function in combination with the one above is designed to
   concatenate it's arguments into a single string in the order in which they
   are given. The first argument must be an std::string, the rest of the
   arguments can be anything that std::stringstream can handle. The template
   function will return the concatenated string. This these template functions
   probably aren't very efficient since they create a new stringstream for
   every argument after the first, however it is only really designed for
   building error messages and so shouldn't be on the critical path. */
template <typename T, typename... Args>
std::string concat(std::string newStr,
		      const T & newStrCompRaw, const Args &... args)
{
  std::stringstream newStrCompsS {};
  newStrCompsS<<newStrCompRaw;
  newStr += newStrCompsS.str();
  return concat(newStr, args...);
}



/* Increment's i by n, if i equals iEnd before being incremented n times we call
   exit(str, eNum). */
template <typename T_A, typename T_B> auto
getAdvancedIter(T_A i, const T_B iEnd, const size_t n, const std::string & eMsg)
  -> T_A
{
  for(size_t iter {}; iter < n -1; ++iter)
    {
      i++;
      if(*i == *iEnd)
	{
	  exit(eMsg, ERROR_GENERIC_RANGE_ERROR);
	}
    }
  return i;
}


template<typename T>
bool getCoordRule(const int y, const int x, const std::vector<T> & rules,
		  int bgHeight, T & coordRulesRet)
{
  bool ret {false};
  int linearAddress = {y * ((int)rules.size() / bgHeight) + x};
  if(size_t(linearAddress) < rules.size())
    {
      coordRulesRet = rules[linearAddress];
      for(auto rule: boarderRuleChars::CHARS)
	{
	  if(coordRulesRet == rule)
	    {
	      ret = true;
	      break;
	    }
	}
    }
  return ret;
}


template<typename T>
bool getCoordRule(const yx & pos, const std::vector<T> & rules, int bgHeight,
		  T & coordRulesRet)
{
  return getCoordRule(pos.y, pos.x, rules, bgHeight, coordRulesRet);
}


#endif
