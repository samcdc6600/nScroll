#include "include/backgroundSprite.hpp"
#include "include/draw.hpp"


void bgSprite::draw(const bool updateSlice, const int bgPos)
{
  if(position.x > bgPos && position.x < (bgPos + 125))
    {
      for(size_t sliceLine{}; sliceLine <
	    spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
	  ++sliceLine)
	{      
	  for(size_t sliceLineIter{}; sliceLineIter <
		spriteS[direction].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine.size();
	      ++sliceLineIter)
	    { // Move curser to the right position.
	      setCursor(position.y + sliceLine, position.x -bgPos +
			spriteS[direction].spriteSlices[currentSliceNumber].slice[sliceLine].offset +
			sliceLineIter, maxyx);
	      // Get the character.
	      int ch {spriteS[direction].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine[sliceLineIter]};
	      drawCh(ch);

	      if(updateSlice)
		{
		  currentTime = std::chrono::high_resolution_clock::now();
		  if(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
									   startTime).count() >
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
	    }
	}
    }
}
