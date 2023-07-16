#include "include/draw.hpp"


player::player
(const double fixedTimeStep, const backgroundData &background,
 std::vector<std::string> spritePaths, const yx initialPosVPRel,
 const sprite::directions dir, const yx cRBS, const int health, const double g,
 const double jumpingPower, const unsigned maxFallingJmpNum,
 const unsigned maxJmpNum, const double maxVelocity,
 const double maxYVelocityFalling, const double leftAcceleration,
 const double rightAcceleration)
  : animateSprite
    (fixedTimeStep, spritePaths, background.chunkSize, initialPosVPRel, dir,
     cRBS, g, jumpingPower,  maxFallingJmpNum, maxJmpNum, maxVelocity,
     maxYVelocityFalling, leftAcceleration, rightAcceleration)
{
  verifyIfMovementLimitsLessThanFixedTimeStep
    (fixedTimeStep, g, jumpingPower, maxVelocity, maxYVelocityFalling,
     leftAcceleration, rightAcceleration);
  // TODO: Remove this function and the corresponding code in animateSprite?
  spritePosition = removeRulesBufferOffset(spritePosition, background.chunkSize, cRBS);
}


/*yx player::getPos() const
{
  yx ret {spritePosition};
  if(secondStageCoordRulesBufferSize.y > viewPortSize.y)
    {
      ret.y -= ((secondStageCoordRulesBufferSize.y - viewPortSize.y) / 2);
      ret.x -= ((secondStageCoordRulesBufferSize.x - viewPortSize.x) / 2);
    }
  return ret;
  }*/


void player::verifyIfMovementLimitsLessThanFixedTimeStep
(const double fixedTimeStep, const double g,
 const double jumpingPower, const double maxVelocity,
 const double maxYVelocityFalling, const double leftAcceleration,
 const double rightAcceleration) const
{
  const double movementLimit {1 / fixedTimeStep};

  if(g > movementLimit)
    {
      exit(concat("Error when initialising player. gravConst cannot be more "
		  "than ", movementLimit, "."), ERROR_INVALID_MOVEMENT_LIMIT);
    }
  else if(jumpingPower > movementLimit)
    {
      exit(concat("Error when initialising player. jumpingPower cannot be more "
		  "than ", movementLimit, "."), ERROR_INVALID_MOVEMENT_LIMIT);
    }
  else if(maxVelocity > movementLimit)
    {
      exit(concat("Error when initialising player. maxXVelocity cannot be more "
		  "than ", movementLimit, "."), ERROR_INVALID_MOVEMENT_LIMIT);
    }
  else if(maxYVelocityFalling > movementLimit)
    {
      exit(concat("Error when initialising player. maxFallingVelocity cannot be"
		  " more than ", movementLimit, "."),
	   ERROR_INVALID_MOVEMENT_LIMIT);
    }
  else if(leftAcceleration > movementLimit)
    {
      exit(concat("Error when initialising player. leftAcceleration cannot be "
		  "more than ", movementLimit, "."),
	   ERROR_INVALID_MOVEMENT_LIMIT);
    }
  else if(rightAcceleration > movementLimit)
    {
      exit(concat("Error when initialising player. rightAcceleration cannot be "
		  "more than ", movementLimit, "."),
	   ERROR_INVALID_MOVEMENT_LIMIT);
    }
}


yx player::removeRulesBufferOffset
(const yx vPRelPos, const yx vPS, const yx cRBS) const
{
    yx ret {vPRelPos};
  /* Only add offset if second stage coord rules buffer is larger than view
     port in y. It should also be larger in x if this is the case and the size
     should already have been verified to be an odd multiple. */
  if(cRBS.y > vPS.y)
    {
      ret.y -= ((cRBS.y - vPS.y) / 2);
      ret.x -= ((cRBS.x - vPS.x) / 2);
    }

   return ret;
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


// This function is a static member function.
bool player::testIntersectionWithPaddingInTopOrLeft
(const yx playerMovementAreaPadding, const yx playerPosVPRel,
 const yx playerMaxBottomRightOffset, bool yDimension)
{
  if(yDimension)
    {
      return playerPosVPRel.y - playerMovementAreaPadding.y < 0;
    }
  else
    {
      return playerPosVPRel.x - playerMovementAreaPadding.x < 0;
    }
};


// This function is a static member function.
bool player::testIntersectionWithPaddingInBottomOrRight
(const yx playerMovementAreaPadding, const yx playerPosVPRel,
 const yx playerMaxBottomRightOffset, bool yDimension, const yx viewPortSize)
{
  if(yDimension)
    {
      return (playerPosVPRel.y + playerMaxBottomRightOffset.y) >=
	(viewPortSize.y - playerMovementAreaPadding.y);
    }
  else
    {
      return (playerPosVPRel.x + playerMaxBottomRightOffset.x) >=
	(viewPortSize.x - playerMovementAreaPadding.x);
    }
};


// /* Test if the player is outisde of the zone inside of the padding in the y
//    or x dimension */  
// bool player::testPaddingInDimension
// (const yx playerMovementAreaPadding, const yx playerPos,
//  const yx playerMaxBottomRightOffset, bool yDimension)
// {
//   if(yDimension)
//     { 
//       return (testIntersectionWithPaddingInTopOrLeft
// 	      (playerMovementAreaPadding, playerPos,
// 	       playerMaxBottomRightOffset, true) ||
// 	      testIntersectionWithPaddingInBottomOrRight
// 	      (playerMovementAreaPadding, playerPos,
// 	       playerMaxBottomRightOffset, true));
//     }
//   else
//     {
//       return (testIntersectionWithPaddingInTopOrLeft
// 	      (playerMovementAreaPadding, playerPos,
// 	       playerMaxBottomRightOffset, false) ||
// 	      testIntersectionWithPaddingInBottomOrRight
// 	      (playerMovementAreaPadding, playerPos,
// 	       playerMaxBottomRightOffset, false));
//     }
// };


void player::moveIntoViewPort(const yx playerMovementAreaPadding)
{
  if(testIntersectionWithPaddingInTopOrLeft
     (playerMovementAreaPadding, spritePosition, maxBottomRightOffset, true))
    {
      // The top of player is in or above the top padding region.
      spritePosition.y = playerMovementAreaPadding.y;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, spritePosition, maxBottomRightOffset,
	   true, viewPortSize))
    {
      // The bottom of player is in or below the bottom padding region.
      spritePosition.y = (viewPortSize.y -playerMovementAreaPadding.y
			 -(maxBottomRightOffset.y + 1));
    }
  else if(testIntersectionWithPaddingInTopOrLeft
	  (playerMovementAreaPadding, spritePosition, maxBottomRightOffset,
	   false))
    {
      // The left of the player is in or to the left of the left padding region.
      spritePosition.x = playerMovementAreaPadding.x;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, spritePosition, maxBottomRightOffset,
	   false, viewPortSize))
    {
      /* The right of the player is in or to the right of the right padding
         region. */
      spritePosition.x = (viewPortSize.x -playerMovementAreaPadding.x
			 -(maxBottomRightOffset.x +1));
    }
}


void player::draw
(const backgroundData &background, const bool updateSlice)
{
  timers.updateSliceTimer();
    
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
		[(spritePosition.y + sliceLine) * viewPortSize.x +
		 spritePosition.x + sliceLineIter +
		 spriteS[direction].
		 spriteSlices[currentSliceNumber].slice[sliceLine].offset] = ch;
	    }

	  if(updateSlice)
	    {
	      if(timers.getTimeSinceLastSliceUpdate() >
		 spriteS[direction].cycleSpeed)
		{
		  timers.resetTimeSinceLastSliceUpdate();
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
