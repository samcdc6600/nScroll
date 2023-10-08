#include <ncurses.h>
#include "include/colorS.hpp"
#include "include/utils.hpp"


void setColorMode::setState(const int state)
{
  if(!inRange(state))
    {
      exit(concat("Error: in setState. Color (", state, ") out of range."),
	   ERROR_COLOR_CODE_RANGE);
    }
  else
    {
      if(state > colorParams::gameColorPairsNo)
	{
	  attron(A_REVERSE);
	  attron(COLOR_PAIR(state - colorParams::gameColorPairsNo));
	}
      else
	{
	  attroff(A_REVERSE);
	  attron(COLOR_PAIR(state));
	}
    }
}


bool setColorMode::inRange(const int color)
{
  if(color < 1 || color > colorParams::effectiveGameColorPairsNo)
    {
      return false; 	// Color is out of range.
    }
  return true;
}


void setColorMode::setColor(const int color)
{
  if(!inRange(color))//is the color variable out of range?
    {
      exit(concat("Error: in setColor(), colour (", color, ") is out of "
		  "range\n"), ERROR_COLOR_CODE_RANGE);
    }
  setState(color);//turn on color color
}


int setColorMode::getColor(const int ch)
{
  /* A character is encoded as it's basic value + it's color value -1 times
     maxCharNum. Since integer division rounds down (ch -1) / maxCharNum should
     give the color code. less 1. */
  int color {((ch -1) / MONO_CH_MAX) +1};
  if(color > colorParams::effectiveGameColorPairsNo)
    {
      exit(concat("Error (in getColor()): encountered colour (", color, ") "
		  "code that is out of range.\n"),
	   ERROR_COLOR_CODE_RANGE);
    }

  return color;
}


void setColorMode::setColorFromChar(const int ch)
{
  setColorMode colorMode {};
  int colorCode = getColor(ch);
  colorMode.setColor(colorCode);
}


void setColorMode::setRandomColor()
{
  setColorMode colorMode {};
  colorMode.setColor
    (abs(rand() % colorParams::gameColorPairsNo) +1);
  if(rand() % 2)
    {
      attron(A_REVERSE);
    }
  else
    {
      attroff(A_REVERSE);
    }
}
