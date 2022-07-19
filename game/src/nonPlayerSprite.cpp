#include "include/nonPlayerSprite.hpp"
#include "include/draw.hpp"


void nonPlayerSprite::draw(const bool updateSlice, const int bgPos)
{
  // Only display sprite if part of it is in the visible region.
  if((position.x + maxBottomRightOffset.x) >= bgPos && position.x <=
     (bgPos + maxyx.x -1) && (position.y + maxBottomRightOffset.y) >= 0
     && (position.y < maxyx.y))
    {
      if(updateSlice)
	{
	  currentTime = std::chrono::high_resolution_clock::now();
	  if(std::chrono::duration_cast
	     <std::chrono::milliseconds>(currentTime - startTime).count() >
	     spriteS[direction].cycleSpeed)
	    {
	      startTime = std::chrono::high_resolution_clock::now();
	      currentSliceNumber++; // Move to next slice
	      /* -1 because indexing from 0, so currentSliceNumber shouldn't
		 be bigger then size() -1 */
	      if(currentSliceNumber >
		 (spriteS[direction].spriteSlices.size() -1))
		{ /* We have displayed all the slices so the value should
		     wrape arround. */
		  currentSliceNumber = 0;
		}
	    }      
	}
      
      for(int sliceLine{}; sliceLine <
	    spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
	  ++sliceLine)
	{      
	  for(int sliceLineIter{}; sliceLineIter <
		spriteS[direction].spriteSlices[currentSliceNumber].
		slice[sliceLine].sliceLine.size(); ++sliceLineIter)
	    { // Move curser to the right position.
	      if(position.y + sliceLine >= 0)
		{
		  if(position.y + sliceLine < maxyx.y)
		    {
		      int xPosRel;
		      xPosRel = {position.x + spriteS[direction].
			spriteSlices[currentSliceNumber].slice[sliceLine].offset +
			sliceLineIter};
		      int xPosAbs;
		      xPosAbs = {xPosRel - bgPos};

		      /* Sprite can be partially in the visible region, so only
			 print if char is in window. */
		      if(xPosRel >= bgPos && xPosRel <= (bgPos + maxyx.x -1))
			{
			  setCursor(position.y + sliceLine, xPosAbs, maxyx);
			  // Get the character.
			  int ch {spriteS[direction].spriteSlices[currentSliceNumber].
			    slice[sliceLine].sliceLine[sliceLineIter]};
			  drawCh(ch);
			}
		    }
		  else
		    {
		      // We've drawn to the last line of the window.
		      break;
		    }
		}
	    }
	}
    }
}
