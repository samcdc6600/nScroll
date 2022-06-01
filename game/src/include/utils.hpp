#ifndef COMMON_H_
#define COMMON_H_


#include <string>
#include <vector>


struct yx
{
  yx() {}  
  yx(const int y, const int x)
  {
    this->y = y;
    this->x = x;
  }
  int y;
  int x;
};


constexpr int yHeight {33}, xWidth{125}; // The window must be these dimensions.
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


/* NOTE: THE CODE IN LOADASSETS.CPP THAT CHECKS FOR INCORRECT BOARDER RULE
   CHARACTERS SHOULD BE UPDATED AFTER AFTER ADDING A NEW BOARDER RULE
   CHARACTER */
namespace boarderRuleChars
{
  // The player cannot move through boarder chars.
  constexpr char boarderChar = 'b';
  /* Same as boarder chars (except the player can move through them if
     moving up.) */
  constexpr char platformChar = 'p';
}; // namespace boarderRuleChars


void sleep(const unsigned long long t);
/* Returns false if a is not range [min, max). */
bool checkRange(const int a, const int min, const int max);
/* Return's true if a - offset is within the range [SINGLE_DIGIT_MIN, SINGLE_DIGIT_MAX].
   Return's false otherwise. */
bool inSingleDigitRange(const int a, const int offset);
// Returns false if unable to open file at path name.
bool loadFileIntoString(const char name[], std::string &buff);
/* Advances buffPos until it reads one past a sequence of characters that
   matches a string in targets, where buffPos points to somewhere in buff and
   the strings in targets will be checked in order of the longest string to the
   shortest and where any two strings are the same length it will check the one
   that comes first alphabetically. Returns the string that was matched. Returns
   an empty string if no strings in targets were matched. BuffPos may be
   advanced up to buff.end() (inclusive). */
std::string skipSpaceUpTo(const std::string &buff,
                          std::string::const_iterator &buffPos,
                          std::vector<std::string> &targets);
/* Finds a target from targets in the buffer pointed to by
   outerPeekPos. Returns the target found. If no target was found returns "". */
static std::string findTargetInBuff
(const std::string::const_iterator & outerPeekPos,
 const std::vector<std::string> & targets);
/* Calls endwin() then print's e to std::cerr and finally call's exit() with
   status */
void exit(const std::string &e, const int status);


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


#endif
