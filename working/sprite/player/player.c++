//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "player.h++"
#include "../../draw/colorS.h++"
#include "../../draw/draw.h++"

extern setColorMode colorMode;//must be included for the draw function

/*Direction's that ch can take on
  Q---W---E
  |...^...|
  A<--|-->D
  |...v...|                                            
  z---S---X                                                     
*/
void player::updatePosRel(sprite::directions dir)
{//update's position of sprite in a relative fashion with reference to the sprite and update's direction.
  yx p {getNewPos(dir)};
  if(inScreenBounds(p.y, p.x))
    {				// We have a valid position so update.
      position = p;
    }
  if(direction != dir)
    {				// Change direction.
      resetCurrentSliceNum();
      direction = dir;
    }  
}

void player::draw(bool updateSlice)
{
  switch(direction)
    {
    case LEFT:
    case LEFT_UPPER:
      sprite::draw(DIR_LEFT, updateSlice);
      break;
    case RIGHT:
    case RIGHT_UPPER:
      sprite::draw(DIR_RIGHT, updateSlice);
      break;
    case UP:
    case UP_UPPER:
      sprite::draw(DIR_UP, updateSlice);
      break;
    case DOWN:
    case DOWN_UPPER:
      sprite::draw(DIR_DOWN, updateSlice);
      break;
    default:
      //add exception here!------------------------------------------~~~~~~~~~~~~~~*******************(O)
      break;
    }    
}
