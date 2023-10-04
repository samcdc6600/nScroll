#include "include/backgroundSprite.hpp"
#include "include/draw.hpp"


void bgSprite::draw
(bgSecondStageDrawBufferType * secondStageDrawBuffer, const bool updateSlice,
 const yx viewPortPos)
{
  timers.updateSliceTimer();

    // Only display sprite if part of it is in the visible region.
  if((spritePosition.x + maxBottomRightOffset.x) >= viewPortPos.x &&
     spritePosition.x <= (viewPortPos.x + viewPortSize.x -1) &&
     (spritePosition.y + maxBottomRightOffset.y) >= viewPortPos.y &&
     spritePosition.y < (viewPortPos.y + viewPortSize.y -1))
    {
      /* TODO: move this code into a function in backgroundSprite or sprite?
	 See how animate / player sprites slices are updates and if we can put
	 this in sprite. */
      if(updateSlice)
	{
	  if(timers.getTimeSinceLastSliceUpdate() >
	     spriteS[direction].cycleSpeed)
	    {
	      timers.resetTimeSinceLastSliceUpdate();
	      currentSliceNumber++; // Move to next slice
	      /* -1 because indexing from 0, so the currentSliceNumber shouldn't
		 be bigger then size() -1. */
	      if(size_t(currentSliceNumber) >
		 (spriteS[direction].spriteSlices.size() -1))
		{ /* We have displayed all the slices so the value should
		     wrape arround. */
		  currentSliceNumber = 0;
		}
	    }

	  drawProper(secondStageDrawBuffer, updateSlice, viewPortPos);
	}
    }
}


void bgSprite::drawProper
(bgSecondStageDrawBufferType * secondStageDrawBuffer, const bool updateSlice,
 const yx viewPortPos)
{
  // Iterate over sprite slice lines.
  for(int sliceLineIter {}; sliceLineIter <
	(int)spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
      ++sliceLineIter)
    {
      /* If current slice line is in the view port (in terms of the y
	 dimension.) */
      if((spritePosition.y + sliceLineIter) >= viewPortPos.y &&
	 (spritePosition.y + sliceLineIter) <
	 (viewPortPos.y + viewPortSize.y))
	{
	  // Iterate over characters within slice line.
	  for(int sliceLineCharIter{}; size_t(sliceLineCharIter) <
		spriteS[direction].spriteSlices[currentSliceNumber].
		slice[sliceLineIter].sliceLine.size(); ++sliceLineCharIter)
	    {
	      handleXCompAndDraw
		(secondStageDrawBuffer, updateSlice, viewPortPos, sliceLineIter, sliceLineCharIter);
	    }
	}
    }
}


void bgSprite::handleXCompAndDraw
(bgSecondStageDrawBufferType * secondStageDrawBuffer, const bool updateSlice,
 const yx viewPortPos, const int sliceLineIter, const int sliceLineCharIter)
{
  /* If current char of current slice line is in the view port
     (in terms of the x dimension.) */
  if((spritePosition.x + spriteS[direction].
      spriteSlices[currentSliceNumber].slice[sliceLineIter].
      offset + sliceLineCharIter) >= viewPortPos.x &&
     (spritePosition.x + spriteS[direction].
      spriteSlices[currentSliceNumber].slice[sliceLineIter].
      offset + sliceLineCharIter) <
     (viewPortPos.x + viewPortSize.x))
    {
      const int ch
	{spriteS[direction].
	 spriteSlices[currentSliceNumber].slice[sliceLineIter].
	 sliceLine[sliceLineCharIter]};

      if(ch != DRAW_NO_OP)
	{
	  // Relative position of the sprite character.
	  const yx relCharPos
	    {spritePosition.y + sliceLineIter - viewPortPos.y,
	     spritePosition.x + spriteS[direction].
	     spriteSlices[currentSliceNumber].slice[sliceLineIter].
	     offset + sliceLineCharIter - viewPortPos.x};

	  secondStageDrawBuffer[relCharPos.y * viewPortSize.x +
				relCharPos.x] = ch;
	}
    }
}
