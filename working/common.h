#ifndef COMMON_H_
#define COMMON_H_
#define MONO_CH_MAX 158
#define COLOR_CH_MAX 63

/* Error codes */
/* There was a problem with the window parameters and the window could not be initialised */
#define ERR_WIN_PARAM -1
#define ERR_CURSOR_PARAM -2	/* Cursor parameters out of range */
#define ERR_POS_CH_RANGE -3	/* Character position out of range */


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

#endif
