#ifndef COMMON_H_
#define COMMON_H_


constexpr int MONO_CH_MAX {158};
constexpr int COLOR_CH_MAX {63};
constexpr int ASCII_NUMBER_OFFSET {48};


enum errorsCodes
  {/* Error codes */
    /* There was a problem with the window parameters and the window could not be initialised */
    ERROR_WIN_PARAM, // Window not initialised - there was a problem with the window parameters.
    ERROR_CURSOR_PARAM,	// Cursor parameters out of range.
    ERROR_POS_CH_RANGE,	// Character position out of range.
    ERROR_OPENING_FILE,	// Error opening file.
    ERROR_CHARACTER_RANGE,	// Character out of range.
    ERROR_COLOR_CODE_RANGE,	// Color code out of range.
    ERROR_GENERIC_RANGE_ERROR,
    ERROR_RULES_LEV_HEADER,	// Header of .level.rules file is malformed.
    ERROR_RULES_STRING_FIELDS,	// There was an error in a field containing string's.
    ERROR_MALFORMED_STRING,	// We encountered a malformed string.
    ERROR_MALFORMED_COORDINATE	// We have encountered a malformed coordinate.
    //ERROR_BAD_LOGIC		// There was an error in the code.
  };


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


void sleep(const unsigned long long t);
/* Returns false if a is not range [0, coord). */
bool checkRange(const int a, const int coord);
/* return's true if a - offset is within the range [SINGLE_DIGIT_MIN, SINGLE_DIGIT_MAX].
   return's false otherwise. */
bool inSingleDigitRange(const int a, const int offset);
/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status);

#endif
