#include <string>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "include/common.hpp"
#include "include/drawExit.hpp"
#include "include/draw.hpp"


void drawExit(const yx maxyx)
{
  const std::string swanSong {"~~*-F-A-R-E-W-E-L-L-*~~"};
  int padding {1};
  bool direction {true};	// True indicates a "growing" direction (i.e. depletion zone around swanSong.)
  int sleepTime {10};	// :)
  constexpr int sleepTimeStep {4};	// How much to increment or decrement sleep time by.
  constexpr int frames {62};
  int ch {};  
  srand(time(NULL));
  
  for(size_t iter {}; iter < frames; ++iter)
    {
      for(size_t xIter {}; xIter < maxyx.x; ++xIter)
	{
	  for(size_t yIter {}; yIter < maxyx.y; ++yIter)
	    {
	      if(swanSong.size() != 0) // Check that message has non zero size.
		{		  
		  if((xIter > ((maxyx.x /2) - (padding + swanSong.size() /2))) &&
		     (xIter < ((maxyx.x /2) + (padding + swanSong.size() /2))) &&
		     (yIter > ((maxyx.y /2) - padding)) &&
		     (yIter < ((maxyx.y /2) + padding)))
		    {
		      if(xIter == ((maxyx.x /2) - (swanSong.size() /2)) &&
			 yIter == (maxyx.y /2))
			 mvprintw(yIter, xIter, swanSong.c_str());
		    }
		  else
		    {
		      setCursor(yIter, xIter, maxyx);
		      ch = (rand() % MONO_CH_MAX) * (rand() % COLOR_CH_MAX);
		      drawCh(ch);
		    }
									       
		}
	    }
	}
      refresh();
      sleep(sleepTime);
      if(iter == (frames /2))
	direction = false;	/* We've printed half the frames so reverse the direction of growth of the 
				   depletion zone around swanSong and start decresing sleeptime. :) */
      if(direction)
	{
	  ++padding;
	  if(iter < frames /4)
	    sleepTime += sleepTimeStep -2;
	  else
	    sleepTime += sleepTimeStep;
	}
      else
	{
	  --padding;
	  if(iter > (frames - (frames /4)))
	    sleepTime -= sleepTimeStep -2;
	  else
	    sleepTime -= sleepTimeStep;
	}
    }
}
