#ifndef COMMON_H_
#define COMMON_H_
#define MONO_CH_MAX 158
#define COLOR_CH_MAX 63


enum errorsCodes
  {/* Error codes */
    /* There was a problem with the window parameters and the window could not be initialised */
    ERROR_WIN_PARAM, // Window not initialised - there was a problem with the window parameters.
    ERROR_CURSOR_PARAM,	// Cursor parameters out of range.
    ERROR_POS_CH_RANGE,	// Character position out of range.
    ERROR_OPENING_FILE,	// Error opening file.
    ERROR_CHARACTER_RANGE,	// Character out of range.
    ERROR_COLOR_CODE_RANGE,	// Color code out of range.
    ERROR_GENERIC_RANGE_ERROR
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
/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status);

#endif
