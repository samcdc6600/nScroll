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
    position = p;		// The position is mint :).
  else
    {			// Oh no :'(.
      switch(dir)
	{
	case LEFT_UP:
	case LEFT_UP_UPPER://left up
	  if(direction != DIR_LEFT)//if we are changind the direction
	    {
	      resetCurrentSliceNum(DIR_RIGHT);//we don't wan't to cause a seg fault
	      direction = DIR_LEFT;//change sprite
	    }	   
	  break;
	case UP:
	case UP_UPPER://up
	  if(direction != DIR_UP)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_UP;
	    }
	  break;
	case RIGHT_UP:
	case RIGHT_UP_UPPER://right up
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	  break;
	case LEFT:
	case LEFT_UPPER://left
	  if(direction != DIR_LEFT)
	    {	       
	      resetCurrentSliceNum(DIR_RIGHT);
	      direction = DIR_LEFT;
	    }
	  break;
	case RIGHT:
	case RIGHT_UPPER://right
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	  break;
	case LEFT_DOWN:
	case LEFT_DOWN_UPPER://left down
	  if(direction != DIR_LEFT)
	    {
	      resetCurrentSliceNum(DIR_RIGHT);
	      direction = DIR_LEFT;
	    }
	  break;
	case DOWN:
	case DOWN_UPPER://down
	  if(direction != DIR_DOWN)
	    {
	      resetCurrentSliceNum(DIR_DOWN);
	      direction = DIR_DOWN;
	    }
	  break;
	case RIGHT_DOWN:
	case RIGHT_DOWN_UPPER://right down
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	}
    }
}

void player::draw(bool updateSlice)
{
  switch(direction)
    {
    case DIR_LEFT:
      sprite::draw(0, updateSlice);
      break;
    case DIR_RIGHT:
      sprite::draw(1, updateSlice);
      break;
    case DIR_UP:
      sprite::draw(2, updateSlice);
      break;
    case DIR_DOWN:
      sprite::draw(3, updateSlice);
      break;
    default:
      //add exception here!------------------------------------------~~~~~~~~~~~~~~*******************(O)
      break;
    }    
}
