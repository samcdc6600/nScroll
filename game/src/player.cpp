#include "include/draw.hpp"


player::player
(const backgroundData &background, std::vector<std::string> spritePaths,
 // const yx PLAYER_MOVEMENT_AREA_PADDING, const yx initialRelViewPortPos,
 const yx initialPosVPRel, const sprite::directions dir, const int h,
 const double g, const double v, const unsigned maxFallingJmpNum,
 const unsigned maxJmpNum)
  : sprite(spritePaths, background.chunkSize, initialPosVPRel, dir, true),
    health(h),
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

  /* The player is allowed to move 1 character outside of the view port. This
     then allows the chunk code to detect that the view port should be
     moved. After the chunk code that handles this is run
     player::moveIntoViewPort() should be called to move the player back into
     the view port if the player has moved out of the view port (this of course
     should be done before displaying 2nd stage the draw buffer. */
  if(p.y > -2 && p.y < (viewPortSize.y + 1) &&
     p.x > -2 && p.x < (viewPortSize.x + 1))
    {
      positionVPRel = p;
      if(direction != dir)
	{			// Change sprite direction animation.
	  resetCurrentSliceNum();
	  direction = spriteAnimationDirections[dir];
	}
    }
}


// Tests if the player is above or to the left of the padding.
bool player::testPaddingDirectionInDimension
(const yx playerMovementAreaPadding, const yx playerPos,
 const yx playerMaxBottomRightOffset, bool yDimension)
{
  if(yDimension)
    {
      return playerPos.y - playerMovementAreaPadding.y < 0;
    }
  else
    {
      return playerPos.x - playerMovementAreaPadding.x < 0;
    }
};


/* Test if the player is outisde of the zone inside of the padding in the y
   or x dimension */  
bool player::testPaddingInDimension
(const yx playerMovementAreaPadding, const yx playerPos,
 const yx playerMaxBottomRightOffset, bool yDimension)
 {
   if(yDimension)
     { 
       return (testPaddingDirectionInDimension
	       (playerMovementAreaPadding, playerPos,
		playerMaxBottomRightOffset, true) ||
	       (playerPos.y + playerMaxBottomRightOffset.y + 1) >
	       (viewPortSize.y - playerMovementAreaPadding.y));
     }
   else
     {
       return (testPaddingDirectionInDimension
	       (playerMovementAreaPadding, playerPos,
		playerMaxBottomRightOffset, false) ||
	       (playerPos.x + playerMaxBottomRightOffset.x + 1) >
	       (viewPortSize.x - playerMovementAreaPadding.x));
     }
 };


void player::moveIntoViewPort()
{
  testPaddingDirectionInDimension()


  
  if(positionVPRel.y < 0)
    {
      positionVPRel.y = 0;
    }
  else if(positionVPRel.y > (viewPortSize.y -1))
    {
      positionVPRel.y = (viewPortSize.y -1);
    }
  if(positionVPRel.x < 0)
    {
      positionVPRel.x = 0;
    }
  else if(positionVPRel.x > (viewPortSize.x -1))
    {
      positionVPRel.x = (viewPortSize.x -1);
    }
}


void player::draw
(const backgroundData &background, const bool updateSlice)
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
	      background.secondStageDrawBuffer
		[(positionVPRel.y + sliceLine) * viewPortSize.x +
		 positionVPRel.x + sliceLineIter +
		 spriteS[direction].
		 spriteSlices[currentSliceNumber].slice[sliceLine].offset] = ch;
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
