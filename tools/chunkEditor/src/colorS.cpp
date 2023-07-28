#include <ncurses.h>
#include "include/colorS.hpp"
#include "include/utils.hpp"


void setColorMode::setState(const int state)
{
  if(!inRange(state))
    {//state is out of range
      std::string eMsg {"in colorS.cpp->setState(const int state), state = "};
      eMsg += state;
      eMsg += "\n";
      throw std::logic_error(eMsg);      
    }
  attron(COLOR_PAIR(state));
}


bool setColorMode::inRange(const int color)
{
  if(color < 0 || color > colorParams::gameColorPairsNo)
    return false;//color is out of range
  return true;//color is in range
}

void setColorMode::setColor(const int color)
{
  if(!inRange(color))//is the color variable out of range?
    {
      exit(concat("Error: in setColor, colour (", color, ") is out of range\n"),
	   ERROR_COLOR_CODE_RANGE);
    }
  setState(color);//turn on color color
}
