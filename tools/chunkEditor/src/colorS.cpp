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
