#include <string>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "include/utils.hpp"
#include "include/drawExit.hpp"
#include "include/draw.hpp"


void drawExit(const yx viewPortSize)
{
  // const std::string swanSong {"~~*-F-A-R-E-W-E-L-L-*~~"};
  // int padding {1};
  // bool direction {true};	// True indicates a "growing" direction (i.e. depletion zone around swanSong.)
  // int sleepTime {1};	// :)
  // constexpr int sleepTimeStep {1};	// How much to increment or decrement sleep time by.
  // constexpr int frames {1};
  // int ch {};  
  // srand(time(NULL));
  
  // for(size_t iter {}; iter < frames; ++iter)
  //   {
  //     for(size_t xIter {}; xIter < viewPortSize.x; ++xIter)
  // 	{
  // 	  for(size_t yIter {}; yIter < viewPortSize.y; ++yIter)
  // 	    {
  // 	      if(swanSong.size() != 0) // Check that message has non zero size.
  // 		{		  
  // 		  if((xIter > ((viewPortSize.x /2) - (padding + swanSong.size() /2))) &&
  // 		     (xIter < ((viewPortSize.x /2) + (padding + swanSong.size() /2))) &&
  // 		     (yIter > ((viewPortSize.y /2) - padding)) &&
  // 		     (yIter < ((viewPortSize.y /2) + padding)))
  // 		    {
  // 		      if(xIter == ((viewPortSize.x /2) - (swanSong.size() /2)) &&
  // 			 yIter == (viewPortSize.y /2))
  // 			 mvprintw(yIter, xIter, swanSong.c_str());
  // 		    }
  // 		  else
  // 		    {
  // 		      setCursor(yIter, xIter, viewPortSize);
  // 		      ch = (rand() % MONO_CH_MAX) * (rand() % COLOR_CH_MAX);
  // 		      drawCh(ch);
  // 		    }
									       
  // 		}
  // 	    }
  // 	}
  //     refresh();
  //     sleep(sleepTime);
  //     if(iter == (frames /2))
  // 	direction = false;	/* We've printed half the frames so reverse the direction of growth of the 
  // 				   depletion zone around swanSong and start decresing sleeptime. :) */
  //     if(direction)
  // 	{
  // 	  ++padding;
  // 	  if(iter < frames /4)
  // 	    sleepTime += sleepTimeStep -2;
  // 	  else
  // 	    sleepTime += sleepTimeStep;
  // 	}
  //     else
  // 	{
  // 	  --padding;
  // 	  if(iter > (frames - (frames /4)))
  // 	    sleepTime -= sleepTimeStep -2;
  // 	  else
  // 	    sleepTime -= sleepTimeStep;
  // 	}
  //   }
}
