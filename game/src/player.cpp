#include "include/draw.hpp"


player::player
(const backgroundData &background, std::vector<std::string> spritePaths,
 // const yx PLAYER_MOVEMENT_AREA_PADDING, const yx initialRelViewPortPos,
 const yx initialPosVPRel, const sprite::directions dir, const int health,
 const double g, const unsigned maxFallingJmpNum,
 const unsigned maxJmpNum)
  : animateSprite(spritePaths, background.chunkSize, initialPosVPRel, dir, g,
		  maxFallingJmpNum, maxJmpNum)
{
  if(gravitationalConstant > 0)
    {
      std::stringstream err {};
      err<<"Error: the gravitational g constant can't be positive, where g was "
	"found to be set to "<<gravitationalConstant<<".";
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
