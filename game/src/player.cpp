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
               const double g, const double v)
    : sprite(spriteFileNames, max, pos, dir), health(h),
      gravitationalConstant(g), maxVertVelocity(v)
{
  if(maxVertVelocity < 1)
    {
      std::stringstream err {};
      err<<"Error: the maximum vertical velocity for the player must be at "
	"least than 1 (the unit is in characters (position) per second.) "<<v
	 <<" was provided.";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
}


/* Returns dir as the corresponding direction, if there is none returns
   DIR_NONE. */
sprite::directions player::convertDirectionCharsToDirections
(const directionChars dir)
{
  directions ret {DIR_NONE};
  switch(dir)
    {
    case UP_CHAR:
      ret = DIR_UP;
      break;
    case RIGHT_CHAR:
      ret = DIR_RIGHT;
	break;
    case DOWN_CHAR:
      ret = DIR_DOWN;
	break;
    case LEFT_CHAR:
      ret = DIR_LEFT;
	break;
    }
  return ret;
}


// bool player::isDirectionCharInputValid(const int input)
// {
//   return (input == player::UP_CHAR || input == player::UP_UPPER_CHAR ||
// 	  input == player::LEFT_CHAR || input == player::LEFT_UPPER_CHAR ||
// 	  input == player::DOWN_CHAR || input == player::DOWN_UPPER_CHAR ||
// 	  input == player::RIGHT_CHAR || input == player::RIGHT_UPPER_CHAR);
// }


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


bool player::startJumping(const int bgPosition,
			  const std::map<std::string, char> & coordChars)
{
  bool retVar {false};
  if(jumpNum < maxJumpNum)
    {
      jumpNum++;
      vertVelocity = gravitationalConstant;
      jumping = jumpingUp;
      retVar = true;

      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	{
	  for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
	    {
	      if(coordChars.find(coord) != coordChars.end())
		{
		  // We're going to hit something.
		  goto RETURN;
		}
	    }
	  // We don't want to hit the top of the level.
	  if(position.y == 0)
	    {
	      goto RETURN;
	    }
	  // We're not going to hit anything, so jump!
	  updatePosRel(sprite::DIR_UP);
	}
    }

 RETURN:
  return retVar;
}


void player::handleJumpingAndFalling(const int bgPosition, const yx maxyx,
			 const std::map<std::string, char> & coordChars)
{
  if(jumping != notJumping)
    {
      if(jumping == jumpingUp)
	{
	  if(vertVelocity <= maxVertVelocity)
	    {
	      vertVelocity += gravitationalConstant;
	    }
	  else
	    {
	      jumping = jumpingDown;
	      vertVelocity -= gravitationalConstant;
	    }
	}
      else if(jumping == jumpingDown)
	{
	  if(vertVelocity > -maxVertVelocity)
	    {
	      vertVelocity -= gravitationalConstant;
	    }
	}

      if(vertVelocity > 0)
	{
	  // We are jumping up.
	  for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	    {
	      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
		{
		  if(coordChars.find(coord) != coordChars.end())
		    {
		      // We're going to hit something.
		      goto RETURN;
		    }
		}
	      if(position.y == 0)
		{
		  goto RETURN;
		}
	      updatePosRel(sprite::DIR_UP);
	    }
	}
      else
	{
	  // We're jumping down.
	  for(int jumps {(int)-vertVelocity}; jumps > 0; jumps--)
	    {
	      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, true, false))
		{
		  if(coordChars.find(coord) != coordChars.end())
		    {
		      // We're going to hit something (stop jumping!)
		      vertVelocity = 0;
		      jumping = notJumping;
		      jumpNum = 0;
		      goto RETURN;
		    }
		}
	      mvprintw(0, 0, "yPos = %d, maxyx.y = %d", position.y, maxyx.y);
	      refresh();
	      if((position.y + maxBottomRightOffset.y) == (maxyx.y -1))
		{
		  // We're going to hit the ground (stop jumping!)
		  vertVelocity = 0;
		  jumping = notJumping;
		  jumpNum = 0;
		  goto RETURN;
		}
	      updatePosRel(sprite::DIR_DOWN);
	    }
	}
    }
 RETURN:
  return;			// Required by RETURN label I guess.
}


// bool player::decJumpNum()
// {
//   boole retVal {true};
//   if(jumpNum > 0)
//     {
//       jumpNum--;
//       retVal = false;
//     }
//   return retVal;
// }
