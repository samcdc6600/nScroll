#include "include/nonPlayerSprite.hpp"
#include "include/draw.hpp"
#include <curses.h>


void nonPlayerSprite::draw
(unsigned short * secondStageDrawBuffer, const bool updateSlice,
 const yx viewPortPos)
{
  // // Only display sprite if part of it is in the visible region.
  // if((positionVPRel.x + maxBottomRightOffset.x) >= viewPortPos && positionVPRel.x <=
  //    (viewPortPos + viewPortSize.x -1) && (positionVPRel.y + maxBottomRightOffset.y) >= 0
  //    && (positionVPRel.y < viewPortSize.y))
  //   {
  //     if(updateSlice)
  // 	{
  // 	  currentTime = std::chrono::high_resolution_clock::now();
  // 	  if(std::chrono::duration_cast
  // 	     <std::chrono::milliseconds>(currentTime - startTime).count() >
  // 	     spriteS[direction].cycleSpeed)
  // 	    {
  // 	      startTime = std::chrono::high_resolution_clock::now();
  // 	      currentSliceNumber++; // Move to next slice
  // 	      /* -1 because indexing from 0, so currentSliceNumber shouldn't
  // 		 be bigger then size() -1 */
  // 	      if(size_t(currentSliceNumber) >
  // 		 (spriteS[direction].spriteSlices.size() -1))
  // 		{ /* We have displayed all the slices so the value should
  // 		     wrape arround. */
  // 		  currentSliceNumber = 0;
  // 		}
  // 	    }      
  // 	}
      
  //     for(int sliceLine{}; size_t(sliceLine) <
  // 	    spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
  // 	  ++sliceLine)
  // 	{      
  // 	  for(int sliceLineIter{}; size_t(sliceLineIter) <
  // 		spriteS[direction].spriteSlices[currentSliceNumber].
  // 		slice[sliceLine].sliceLine.size(); ++sliceLineIter)
  // 	    { // Move curser to the right position.
  // 	      if(positionVPRel.y + sliceLine >= 0)
  // 		{
  // 		  if(positionVPRel.y + sliceLine < viewPortSize.y)
  // 		    {
  // 		      int xPosRel;
  // 		      xPosRel = {positionVPRel.x + spriteS[direction].
  // 			spriteSlices[currentSliceNumber].slice[sliceLine].
  // 			offset + sliceLineIter};
  // 		      int xPosAbs;
  // 		      xPosAbs = {xPosRel - viewPortPos};

  // 		      /* Sprite can be partially in the visible region, so only
  // 			 print if char is in window. */
  // 		      if(xPosRel >= viewPortPos && xPosRel <= (viewPortPos + viewPortSize.x -1))
  // 			{
  // 			  int ch;
  // 			  ch = spriteS[direction].
  // 			    spriteSlices[currentSliceNumber].slice[sliceLine].
  // 			    sliceLine[sliceLineIter];
			  
  // 			  if(ch != DRAW_NO_OP)
  // 			    {
			      
  // 			      secondStageDrawBuffer[((positionVPRel.y + sliceLine) *
  // 						     viewPortSize.x) + xPosAbs] = ch;
  // 			    }
  // 			}
  // 		    }
  // 		  else
  // 		    {
  // 		      // We've drawn to the last line of the window.
  // 		      break;
  // 		    }
  // 		}
  // 	    }
  // 	}
  //   }
}
