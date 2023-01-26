//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "include/player.hpp"
#include "include/colorS.hpp"
#include "include/draw.hpp"


player::player
(std::vector<std::string> spritePaths, const yx max,
 const backgroundData & background, const yx pos, const sprite::directions dir,
 const int h, const double g, const double v, const unsigned maxFallingJmpNum,
 const unsigned maxJmpNum)
 : sprite(spritePaths, max, background, pos, dir, true), health(h),
    gravitationalConstant(g), maxVertVelocity(v),
    maxFallingJumpNum(maxFallingJmpNum), maxJumpNum(maxJmpNum)
{
  if(gravitationalConstant > 0)
    {
      std::stringstream err {};
      err<<"Error: the gravitational g constant can't be positive, where g was "
	"found to be set to "<<gravitationalConstant<<".";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  else if(maxVertVelocity < 1)
    {
      std::stringstream err {};
      err<<"Error: the maximum vertical velocity for the player must be at "
	"least than 1 (the unit is in characters (position) per second.) "<<v
	 <<" was provided.";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  else if(maxFallingJmpNum > maxJmpNum)
    {
      std::stringstream err {};
      err<<"Error: maxFallingJmpNum ("<<maxFallingJmpNum<<") can't be more then"
	" maxJmpNum. ("<<maxJmpNum<<").";
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


bool player::startJumping
(const int bgPosition, yx maxyx, const std::vector<char> & coordRules)
{
  bool retVar {false};
  if(jumpNum < maxJumpNum)
    {
      jumpNum++;
      vertVelocity = -gravitationalConstant;
      jumping = jumpingUp;
      retVar = true;

      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	{
	  for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
	    {
	      char rule {};
	      if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
		 rule == boarderRuleChars::BOARDER_CHAR)
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
  else
    {
      /* We must call this here because this function is called (INSTEAD OF HANDLEJUMPINGANDFALLING())  when
         sprite::DIR_UP is pressed and if we can't perform a new jump we must
         continue the current jump / fall. */
      handleJumpingAndFalling(bgPosition, maxyx, coordRules);
    }

 RETURN:
  return retVar;
}


void player::handleJumpingAndFalling(const int bgPosition, const yx & maxyx,
				     const std::vector<char> & coordRules)
{
  if(jumping == notJumping)
    {
      handleFalling(bgPosition, maxyx, coordRules);
    }
  else
    {
      handleJumping(bgPosition, maxyx, coordRules);
    }

  return;			// Required by RETURN label I guess.
}


void player::handleFalling
(const int bgPosition, const yx &maxyx, const std::vector<char> &coordRules)
{
  using namespace boarderRuleChars;
  
  if((position.y + maxBottomRightOffset.y) == (maxyx.y -1))
    {
      // We're at the bottom of the level.
      return;
    }
  else
    {
      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, true, false))
	{
	  char rule {};
	  if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
	     (rule == BOARDER_CHAR ||
	      rule == PLATFORM_CHAR))
	    {
	      // There's a rule character below stopping us from falling.
	      return;
	    }
	}
    }
  
  jumpNum = maxJumpNum -maxFallingJumpNum;
  vertVelocity = gravitationalConstant;
  jumping = jumpingDown;

  // We're jumping down.
  handleFallingSimple(bgPosition, maxyx, coordRules);
}


void player::handleJumping
(const int bgPosition, const yx & maxyx, const std::vector<char> & coordRules)
{
  if(jumping == jumpingUp)
    {
      if(vertVelocity <= maxVertVelocity)
	{
	  vertVelocity -= gravitationalConstant;
	}
      else
	{
	  jumping = jumpingDown;
	  vertVelocity += gravitationalConstant;
	}
    }
  else if(jumping == jumpingDown)
    {
      if(vertVelocity > -maxVertVelocity)
	{
	  vertVelocity += gravitationalConstant;
	}
    }

  if(vertVelocity > 0)
    {
      // We are jumping up.
      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	{
	  for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
	    {
	      char rule {};
	      if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
		 rule == boarderRuleChars::BOARDER_CHAR)
		{
		  // We're going to hit something.
		  return;
		}
	    }
	  if(position.y == 0)
	    {
	      return;
	    }
	  updatePosRel(sprite::DIR_UP);
	}
    }
  else
    {
      // We're jumping down.
      handleFallingSimple(bgPosition, maxyx, coordRules);
    }
}


void player::handleFallingSimple
(const int bgPosition, const yx & maxyx, const std::vector<char> & coordRules)
{
  for(int jumps {(int)-vertVelocity}; jumps > 0; jumps--)
    {
      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, true, false))
	{
	  char rule {};
	  if(getCoordRule(coord, coordRules, maxyx.y, rule))
	    {
	      // We're going to hit something (stop jumping!)
	      vertVelocity = 0;
	      jumping = notJumping;
	      jumpNum = 0;
	      return;
	    }
	}
      /* This is a simpler check but probably much less common, so we put it
	 second. */
      if((position.y + maxBottomRightOffset.y) == (maxyx.y -1))
	{
	  // We're going to hit the bottom of the level (stop jumping!)
	  vertVelocity = 0;
	  jumping = notJumping;
	  jumpNum = 0;
	  return;
	}
      updatePosRel(sprite::DIR_DOWN);
    }

}


void player::draw(unsigned short * secondStageDrawBuffer, const bool updateSlice)
{
  for(size_t sliceLine{}; sliceLine <
	spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
      ++sliceLine)
    {      
      for(size_t sliceLineIter{}; sliceLineIter <
	    spriteS[direction].spriteSlices[currentSliceNumber].
	    slice[sliceLine].sliceLine.size();
	  ++sliceLineIter)
	{
	  int ch;
	  ch = spriteS[direction].spriteSlices[currentSliceNumber].
	    slice[sliceLine].sliceLine[sliceLineIter];

	  if(ch != DRAW_NO_OP)
	    {
	      // Add character (if not DRAW_NO_OP.)
	      secondStageDrawBuffer
		[((position.y + sliceLine) * maxyx.x) +
		 position.x + spriteS[direction].
		 spriteSlices[currentSliceNumber].slice[sliceLine].offset +
		 sliceLineIter] = ch;
	    }

	  if(updateSlice)
	    {
	      currentTime = std::chrono::high_resolution_clock::now();
	      if(std::chrono::duration_cast<std::chrono::milliseconds>
		 (currentTime - startTime).count() >
		 spriteS[direction].cycleSpeed)
		{
		  startTime = std::chrono::high_resolution_clock::now();
		  currentSliceNumber++; // Move to next slice.
		  /* -1 because indexing from 0, so currentSliceNumber shouldn't
		     be bigger then size() -1. */
		  if(size_t(currentSliceNumber) >
		     (spriteS[direction].spriteSlices.size() -1))
		    {
		      /* We have displayed all the slices so the value should
			 wrape arround. */
		      currentSliceNumber = 0;
		    }
		}      
	    }
	}
    }
}
