#include <ncurses.h>
#include "colorS.h"

colourMap::colourMap()
{
  const int offset{159};
  int colourVal {offset};//range of characters
  for(std::string colourIter: colorPairs)
    {
      map[colourIter] = colourVal;//associate color pair with range (offset)
      colourVal += offset;//set to next range
    }
}

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
  if(color < 0 || color > colorMax)
    return false;//color is out of range
  return true;//color is in range
}

setColorMode::setColorMode(const int color): defaultColor(color)//set the default color and the color state
{
  if(!inRange(color))
    {
      std::string eMsg {"in colorS.cpp->setColorMode(const int color), color = "};
      eMsg += color;
      eMsg += "\n";
      throw std::logic_error(eMsg);
    }
  setState(defaultColor);//set default color
}

void setColorMode::setColor(const int color)
{
  if(!inRange(color))//is the color variable out of range?
    {
      std::string eMsg {"in colorS.cpp->setColor(const int color), color = "};
      eMsg += color;
      eMsg += "\n";
      throw std::logic_error(eMsg);
    }
  setState(color);//turn on color color
}

void setColorMode::clearColor()
{
  setState(defaultColor);//turn on defaultColor color
}
