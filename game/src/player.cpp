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

  // /* The player is allowed to move 1 character outside of the view port. This
  //    then allows the chunk code to detect that the view port should be
  //    moved. After the chunk code that handles this is run
  //    player::moveIntoViewPort() should be called to move the player back into
  //    the view port if the player has moved out of the view port (this of course
  //    should be done before displaying 2nd stage the draw buffer. */
  // if(p.y > -2 && p.y < (viewPortSize.y + 1) &&
  //    p.x > -2 && p.x < (viewPortSize.x + 1))
  //   {
      positionVPRel = p;
      if(direction != dir)
	{			// Change sprite direction animation.
	  resetCurrentSliceNum();
	  direction = spriteAnimationDirections[dir];
	}
    // }
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
     (playerMovementAreaPadding, positionVPRel, maxBottomRightOffset, true))
    {
      // The top of player is in or above the top padding region.
      positionVPRel.y = playerMovementAreaPadding.y;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, positionVPRel, maxBottomRightOffset,
	   true, viewPortSize))
    {
      // The bottom of player is in or below the bottom padding region.
      positionVPRel.y = (viewPortSize.y -playerMovementAreaPadding.y
			 -(maxBottomRightOffset.y + 1));
    }
  else if(testIntersectionWithPaddingInTopOrLeft
	  (playerMovementAreaPadding, positionVPRel, maxBottomRightOffset,
	   false))
    {
      // The left of the player is in or to the left of the left padding region.
      positionVPRel.x = playerMovementAreaPadding.x;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, positionVPRel, maxBottomRightOffset,
	   false, viewPortSize))
    {
      /* The right of the player is in or to the right of the right padding
         region. */
      positionVPRel.x = (viewPortSize.x -playerMovementAreaPadding.x
			 -(maxBottomRightOffset.x +1));
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
