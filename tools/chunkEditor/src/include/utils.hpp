#ifndef UTILS_HPP_
#define UTILS_HPP_


#include <vector>
#include <sstream>
#ifdef DEBUG
/* This file should be included directly or indirectly be almost every other
   file. We put includes here because if building in debug mode we often need
   them but don't want to accidentally leave includes in a file. */
#include <iostream>
#include <curses.h>
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


// The window must be these dimensions.
constexpr int yHeight {48}, xWidth {170};


/* TODO: find and update any code still using int directory in situations
   where this type should be used. */
typedef int backgroundChunkCharType;


struct backgroundChunkCharInfo
{
  backgroundChunkCharType ch;
  // Tells if ch has been set yet.
  bool set;
};


typedef backgroundChunkCharType bgChunk[yHeight][xWidth];
typedef char 			cRChunk[yHeight][xWidth];


struct bgChunkStrt
{
  backgroundChunkCharType chunk[yHeight][xWidth];

  bgChunkStrt(const bgChunk chunk)
  {
    for(int yIter {}; yIter < yHeight; ++yIter)
      {
	for(int xIter {}; xIter < xWidth; ++xIter)
	  {
	    this->chunk[yIter][xIter] = chunk[yIter][xIter];
	  }
      }
  }
};


struct cRChunkStrt
{
  char chunk[yHeight][xWidth];

  cRChunkStrt(const cRChunk chunk)
  {
    for(int yIter {}; yIter < yHeight; ++yIter)
      {
	for(int xIter {}; xIter < xWidth; ++xIter)
	  {
	    this->chunk[yIter][xIter] = chunk[yIter][xIter];
	  }
      }
  }
};


/* Character printed in bg chunk mode when a character is not yet in the
   position in question (when not in bgShowUnsetChars mode.) Also used for
   the same purpose for cR chunk mode. */
constexpr int emptyCharChar {' '};
/* These constants are special values that signify the start of run length
   sequences. */
constexpr backgroundChunkCharType bgRunLengthSequenceSignifier
  {std::numeric_limits<int>::max()};
constexpr char cRRunLengthSequenceSignifier {'R'};
/* The point after which we will gain an advantage from our run length
   compression scheme compression.
   RunLengthSequenceSignifier + length + character = 3. */
constexpr int bgCompressionAdvantagePoint{3};
/* We need to be able to store lengths of at least 8160 (our chunk size 170x48)
   For this we will need two chars. Thus a run length sequence will take up
   RunLengthSequenceSignifier + lengthComp1 + lengthComp2 + character
   characters. */
constexpr int cRCompressionAdvantagePoint {4};


constexpr int ASCII_CH_MAX {127};
/* Numerical value of highest character using color pair 1.  */
constexpr int maxCharNum {158};
constexpr int COLOR_CH_MAX {63};
constexpr int subMenuSleepTimeMs {160};
constexpr int helpColorPair	{1};
constexpr int warningColor	{86};
constexpr int ASCII_NUMBER_OFFSET {48};
constexpr char ESC_CHAR{27};
constexpr char BACKGROUND_FILE_EXTENSION [] {".levbg"};
constexpr char COORD_RULES_FILE_EXTENSION [] {".levcr"};
constexpr char BACKGROUND_CHUNK_FILE_EXTENSION [] {".bgchunk"};
constexpr char COORD_RULES_CHUNK_FILE_EXTENSION[] {".crchunk"};

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
		   ERROR_WRITING_TO_FILE,
		   ERROR_MALFORMED_FILE,
                   ERROR_CHARACTER_RANGE,   // Character out of range.
                   ERROR_COLOR_CODE_RANGE,  // Color code out of range.
                   ERROR_GENERIC_RANGE_ERROR,
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
		   ERROR_INVALID_MOVEMENT_LIMIT,	// A user supplied player
						// movement limit is out of
						// range.
		   ERROR_INVALID_CMD_ARGS
};


namespace boarderRuleChars
{
  /* NOTE: when adding boarder rule characters you must add then to utils.cpp as
     well as adding the corresponding extern declarations here. */
  extern const char BOARDER_CHAR;
  extern const char PLATFORM_CHAR;
  extern const std::vector<char> CHARS;
};


constexpr int printCharSpeed		{1};
constexpr int afterPrintSleep 		{580};
constexpr int afterPrintSleepFast 	{10};
    

void disableBlockingUserInput();
void enableBlockingUserInput();
void move(const yx pos);
void mvprintw(const yx pos, const std::string &str);
/* Calls progressivePrintMessage() (see header below.) with clearScreen and
   printProgressively  */
void progressivePrintMessage
(const std::string & msg, const yx viewPortSize, const int interCharacterSleep,
 const int afterMsgSleep);
/* Print a message to the centre of the view port when in curses mode.
   If printProgressively is set to true then refresh() will be called and each
   character is printed at which point the function will sleep for
   interCharacterSleep before the next character is printed. If clearScreen is
   true then the screen will be cleared before printing the message. */
void progressivePrintMessage
(const std::string & msg, const yx viewPortSize, const int interCharacterSleep,
 const int afterMsgSleep, const bool clearScreen,
 const bool printProgressively);
/* Print a message when not in curses mode. */
void printMessageNoWin
(const std::string & msg, const int interCharacterSleep,
 const int afterMsgSleep);
/* Should only be used for fully populated bgChunks, hence the use of
   backgroundChunkCharType and not backgroundChunkCharInfo. Use of not fully
   populated bgChunks will lead to undefined behaviour. */
void printBgChunk
(const backgroundChunkCharType bgChunk[][xWidth], const yx viewPortSize);
void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize);
bool getConfirmation
(const yx viewPortSize, const int msgColorPair, int & userInput,
 const int editSubMenuSleepTimeMs, const std::string & question);
/* If input == quitChar then asks the user if they really want to quit. Returns
   true if they do, false otherwise. */
bool confirmQuit(const yx viewPortSize, int & input, const int quitChar);
void safeScreenExit(int & userInput, const int editSubMenuSleepTimeMs);
void setColorFromChar(const int ch);
int getColor(const int ch);
void setRandomColor();
/* Removes color info from ch. If ch is an aCS character sets aCS to true,
   otherwise sets aCS to false. Prints an error and exits if ch is out of
   range. */
int getChar(const int ch, bool & aCS);
std::chrono::steady_clock::time_point setCursorVisibility
(bool & cursorOn, const std::chrono::steady_clock::time_point tLast);
void printACS(const yx position, const int aCSChar);
void printACS(const int aCSChar);
void sleep(const unsigned long long t);
bool isNum(const char c);
void loadFileIntoString(const char name[], std::string & buff,
			const std::string & eMsg);
void createFileIfNoneExists
(const std::string & fileName, const std::string & eMsg);
bool fileExists(const std::string & fileName);
void createFile(const std::string & fileName, const std::string & eMsg);
int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers);
void readInBgChunkFile
(const std::string fileName, backgroundChunkCharType chunk[yHeight][xWidth],
 const yx chunkSize, yx & chunkCoord, bool & foundCoord);
void readInCRChunkFile
(const std::string fileName, char chunk[][xWidth], const yx chunkSize,
 yx & chunkCoord, bool & foundCoord);
/* Reads in and decompress a chunk from a file. The file should already be
   open. If it is a multi chunk file, multiChunkFile should be set to true. In
   this case if there is an error the function will return false. However if
   multiChunkFile is false an error message will be printed and the program
   exited upon error. File should be an already opened chunk file, chunk is
   where the chunk read in will be stored and chunkCoord is the chunk
   coordinates read from the file.
   NOTE THAT IF A THE LAST CHUNK IN THE FILE IS READ FILE.FAIL() WILL BE TRUE
   AND WILL NEED TO BE RESET IF MORE OPERATIONS (AT LEAST FOR SOME POSSIBLE
   OPERATIONS) NEED TO BE DONE ON FSTREAM. */
bool getBgChunk
(std::fstream & file, backgroundChunkCharType chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile = false);
bool readInChunkCoordFromFile
(yx & retVal, const std::string & fileName, std::fstream & file,
 const bool exitOnError);
/* NOTE THAT IF A THE LAST CHUNK IN THE FILE IS READ FILE.FAIL() WILL BE TRUE
   AND WILL NEED TO BE RESET IF MORE OPERATIONS (AT LEAST FOR SOME POSSIBLE
   OPERATIONS) NEED TO BE DONE ON FSTREAM. */
bool getCRChunk
(std::fstream & file, char chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile = false);
// /* Attempts to read in the next background (starting at file.tellg()). */
// bool getNextBgChunk
// (std::fstream & file, backgroundChunkCharType chunk[][xWidth],
//  const yx chunkSize, yx & chunkCoordconst std::string & fileName);
void compressAndWriteOutBgChunk
(const std::string & fileName, std::ofstream & file, const yx chunkCoord,
 const backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize);
void compressAndWriteOutCRChunk
(const std::string & fileName, std::ofstream & file, const yx chunkCoord,
 const char cRChunk[][xWidth], const yx chunkSize);
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
/* Finds a target from targets in the buffer pointed to by
   outerPeekPos. Returns the target found. If no target was found returns "". */
static std::string
findTargetInBuff(const std::string::const_iterator & outerPeekPos,
		 const std::vector<std::string> & targets);
/* Returns true if postfix is a strict postfix of str. */
bool checkForPostfix(const char str [], const char postfix []);
/* Finds the minimum y value in coords as well as the maximum y value. Stores
   the minimum and maximum values for y in minMaxY.y and minMax.x
   respectively. Does the same for x, only of course storing the values in
   minMaxX. This function assumes that coords is non empty. */
void getMinMaxYAndX(const std::vector<yx> & coords, yx & minMaxY, yx & minMaxX);
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


#endif
