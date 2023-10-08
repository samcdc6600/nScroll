#ifndef UTILS_HPP_
#define UTILS_HPP_


#include <string>
#include <vector>
#include <sstream>
#ifdef DEBUG
/* This file should be included directly or indirectly be almost every other
   file. We put includes here because if building in debug mode we often need
   them but don't want to accidentally leave includes in a file. */
#include <iostream>
#include <curses.h>
#include <random>
#include <fstream>
#endif


struct yx
{
  int y {};
  int x {};

  
  yx() {}

  yx(const int a)
  {
    x = y = a;
  }
  
  yx(const yx & yxIn)
  {
    this->y = yxIn.y;
    this->x = yxIn.x;
  }
  
  yx(const int y, const int x)
  {
    this->y = y;
    this->x = x;
  }

  yx operator=(const yx & rhs)
  {
    y = rhs.y;
    x = rhs.x;
    return * this;
  }

  bool operator==(const yx & rhs) const
  {
    if(y != rhs.y || x != rhs.x)
      {
	return false;
      }
    return true;
  }

  bool operator!=(const yx & rhs) const
  {
    return !operator==(rhs);
  }

  yx operator+=(const yx & rhs)
  {
    y += rhs.y;
    x += rhs.x;
    return * this;
  }

  yx operator+(const yx & rhs) const
  {
    return yx{y + rhs.y, x + rhs.x};
  }

  yx operator-(const yx & rhs) const
  {
    return yx{y - rhs.y, x - rhs.x};
  }

  yx operator%(const yx & rhs) const
  {
    return yx{y % rhs.y, x % rhs.x};
  }
};


yx abs(const yx a);


std::ostream & operator<<(std::ostream &lhs, const yx rhs);


typedef int bgSecondStageDrawBufferType;
// The window must be these dimensions.
constexpr int yHeight {48}, xWidth {170};
constexpr int ASCII_CH_MAX {127};
constexpr int MONO_CH_MAX {158};
constexpr int ASCII_NUMBER_OFFSET {48};
constexpr char ESC_CHAR {27};
// constexpr int BACKGROUND_HEIGHT {33};

constexpr char BACKGROUND_FILE_EXTENSION [] {".levbg"};
constexpr char COORD_RULES_FILE_EXTENSION [] {".levcr"};
constexpr char RULES_CONFIG_FILE_EXTENSION [] {".rules.lev"};
constexpr char SPRITE_FILE_EXTENSION [] {".sprite"};

enum errorsCodes {                     /* Error codes. */
                   ERROR_WIN_PARAM,    // Window not initialised - there was a
                                       // problem with the window parameters.
                   ERROR_CURSOR_PARAM, // Cursor parameters out of range.
		   /* Relative view player pos or padding out of range. */
                   ERROR_PLAYER_PADDING_RANGE,
                   ERROR_POS_CH_RANGE,      // Character position out of range.
                   ERROR_INVALID_DIRECTION, // Direction character found not to
                                            // be valid.
                   ERROR_OPENING_FILE,      // Error opening file.
                   ERROR_CHARACTER_RANGE,   // Character out of range.
                   ERROR_COLOR_CODE_RANGE,  // Color code out of range.
                   ERROR_GENERIC_RANGE_ERROR,
		   /* Some file (of unknown type) is malformed. */
		   ERROR_MALFORMED_FILE,
                   ERROR_BACKGROUND,          // Malformed BG file.
		   ERROR_RULES_CHAR_FILE,
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
                   ERROR_BAD_LOGIC, // There was an error in the code.
		   ERROR_BAD_COMMENT,	// C style comment not closed.
		   ERROR_BAD_INTERNAL_VALUE,	// Used when a bad hard coded
						// value is encountered.
		   ERROR_INVALID_MOVEMENT_LIMIT	// A user supplied player
						// movement limit is out of
						// range. 
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
void loadFileIntoString(const char name[], std::string & buff,
			const std::string & eMsg);
/* First checks if buffPos is in range. Returns false if it is not. Otherwise
   attempts to read the coordinates into chunkCoord. If this succeeds returns
   true (with chunkCoord being set to the coordinates read.) If there is a
   failure in reading the coordinates then the program will be aborted with eMsg
   being displayed. */
bool getChunkCoordinate
(const std::string & data, std::string::const_iterator & buffPos,
 const std::string & eMsg, yx & chunkCoord);
bool readInChunkCoordFromFile
(yx & retVal, const std::string & fileName, std::fstream & file,
 const bool exitOnError);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers);
/* Similar to readSingleNum except can read real numbers and can always read
   negative numbers. */
double readSingleRNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg);
void getChunk
(const std::string & data, std::string::const_iterator & buffPos,
 const std::string & eMsg, std::string & chunk, const yx expectedChunkSize);


inline yx convertCharCoordToChunkCoord(const yx chunkSize, yx charCoord)
{
  /* We need to add an offset here if charCoord is less than 0 because otherwise
     we would have two zero chunks (for each axis). */
  return yx
    {(charCoord.y - (charCoord.y < 0 ? chunkSize.y -1: 0)) / chunkSize.y,
     (charCoord.x - (charCoord.x < 0 ? chunkSize.x -1: 0)) / chunkSize.x};
}


/* Creates a chunk coord key from a chunk coordinate. I.e. the input must
   already be a chunk coordinate and not a character coordinate. */
std::string createChunkCoordKey(const yx coord);
std::string createChunkCoordKeyFromCharCoord(const yx charCoord);
/* Returns true if str is in buff at buffPos. */
bool checkForStringInBufferAtPos(const std::string & buff, int buffPos,
				 const std::string str);
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
/* Returns true if postfix is a strict postfix of str. */
bool checkForPostfix(const char str [], const char postfix []);
/* Used to update yIter and xIter for loop in get chunk functions. */
void updateVirtualNestedChunkYXLoop
(int & xIter, int & yIter, const yx chunkSize);
/* Used to detect if the chunk is too big in get chunk functions. If
   multiChunkFile is true and yIter is too large the function will return
   false. Other wise it will return true. If multiChunkfile is false and yIter
   is too large the function will exit with an error message. */
bool checkYOfVirtualNestedChunkLoop
(int & xIter, int & yIter, const yx chunkSize, const bool multiChunkFile,
 const std::string & eMsgStart);
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


template<typename T1, typename T2>
void insertChunk
(const yx coord, const T1 & chunk, const ssize_t chunksReadIn,
 const char fileName [], T2 & chunkMap)
{
  /* Store chunk in chunkMap with a key that should be calculated according to
     the following:
     Concatenate (y / chunkSize.y) and (x / chunkSize.y) and use as index into
     map. Then  (y % (chunkSize.y * 3)) * chunkSize.x + (x % (chunkSize.x * 3))
     can be used to index into the object returned. The stage 1 draw buffer will
     be 3 by 3 chunks. */
  const std::string key
    {createChunkCoordKey(coord)};
  // Note that insert should copy it's argument.
  if(chunkMap.insert
     (std::pair<std::string, T1>
      (key, chunk)).second == false)
    {
      exit(concat
	   ("Error: duplicate chunk coordinate (",
	    key, ") found when loading chunk no. ",
	    chunksReadIn, " from file \"", fileName, "\"."),
	   ERROR_DUPLICATE_COORDINATE);
    }
}


#endif
