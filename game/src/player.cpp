//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "include/player.hpp"
#include "include/colorS.hpp"
#include "include/draw.hpp"

//extern setColorMode colorMode; // must be included for the draw function


player::player(std::vector<std::string> spriteFileNames, const yx max,
	       const yx pos, const sprite::directions dir, const int h,
	       const int v)
  : sprite(spriteFileNames, max, pos, dir),
    health(h), vertVelocity(v)
{}


sprite::directions player::convertDirectionCharsToDirections
(const directionChars dir)
{
  directions ret {DIR_NONE};
  switch(dir)
    {
    case ERR:			// There wasn't any any input.
      ret = DIR_NONE;
	break;
    case UP_CHAR:
    case UP_UPPER_CHAR:
      ret = DIR_UP;
      break;
    case RIGHT_UP_CHAR:
    case RIGHT_UP_UPPER_CHAR:
      ret = DIR_RIGHT_UP;
	break;
    case RIGHT_CHAR:
    case RIGHT_UPPER_CHAR:
      ret = DIR_RIGHT;
	break;
    case RIGHT_DOWN_CHAR:
    case RIGHT_DOWN_UPPER_CHAR:
      ret = DIR_RIGHT_DOWN;
	break;
    case DOWN_CHAR:
    case DOWN_UPPER_CHAR:
      ret = DIR_DOWN;
	break;
    case LEFT_DOWN_CHAR:
    case LEFT_DOWN_UPPER_CHAR:
      ret = DIR_LEFT_DOWN;
	break;
    case LEFT_CHAR:
    case LEFT_UPPER_CHAR:
      ret = DIR_LEFT;
	break;
    case LEFT_UP_CHAR:
    case LEFT_UP_UPPER_CHAR:
      ret = DIR_LEFT_UP;
	break;
    default:
      std::stringstream e {};
      e<<"Error direction with ASCII code("<<dir<<") not valid.";
      exit(e.str().c_str(), ERROR_INVALID_DIRECTION);
    }
  return ret;
}


bool player::isDirectionCharInputValid(const int input)
{
  return (input == ERR ||	// Err is a special case and is passed in when
				// there is no input.
	  input == player::UP_CHAR || input == player::UP_UPPER_CHAR ||
	  input == player::LEFT_CHAR || input == player::LEFT_UPPER_CHAR ||
	  input == player::DOWN_CHAR || input == player::DOWN_UPPER_CHAR ||
	  input == player::RIGHT_CHAR || input == player::RIGHT_UPPER_CHAR);
}


void player::updatePosRel(const sprite::directions dir)
{ /* Update's position of sprite in a relative fashion with reference to the
     sprite and update's direction. */
  checkDirection(dir);
  yx p {getNewPos(dir)};	// GetNewPos will check if dir is valid!
  position = p;
  if(direction != dir)
    {				// Change direction.
      resetCurrentSliceNum();
      direction = spriteAnimationDirections[dir];
    }
}


double player::getVertVelocity()
{
  return vertVelocity;
}


void player::setVertVelocity(const double newVV)
{
  vertVelocity = newVV;
}
