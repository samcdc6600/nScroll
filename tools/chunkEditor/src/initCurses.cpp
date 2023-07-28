#include "include/initCurses.hpp"
#include "include/utils.hpp"


std::vector<int> initialiseCurses(yx & maxyx)
{
  initscr();                                  // Start curses mode                                                     
  getmaxyx(stdscr, maxyx.y, maxyx.x); // Screen size in characters
  
  if(maxyx.y != yHeight || maxyx.x != xWidth) // Check the window size.                                                
    {
      std::stringstream e {};
      e<<"Error windows must be "<<yHeight<<" characters high and "<<xWidth<<" characters wide. "
        "The current height is "<<maxyx.y<<" characters and the current width is "<<maxyx.x<<" characters.\n";
      printMessageNoWin(e.str(), printCharSpeed, afterPrintSleep);
      exit(ERROR_WIN_PARAM);
    }
  if(has_colors() == FALSE)  // If the terminal does not support colors                                                
    {
      printMessageNoWin("Error your terminal does not support colour :'(\n",
			printCharSpeed, afterPrintSleep);
      exit(ERROR_WIN_PARAM);
    }
  if(tigetnum("colors") < colorParams::gameColors)
    {
      printMessageNoWin
	(concat("Error your terminal only supports ", tigetnum("colors"),
		" colors which is less than the required ",
		colorParams::gameColors, " colors.\n"),
	 printCharSpeed, afterPrintSleep);
      exit(ERROR_WIN_PARAM);
    }
  
  raw();                     // To disable line buffering
  // curs_set(0);               // Make the cursor invisible
  disableBlockingUserInput();
  start_color();             // Start color and initialise color pairs!

  /* COLOR_PAIRS is 0 before start_color() is called! */
  if(COLOR_PAIRS < colorParams::gameColorPairsNo)
    {
      printMessageNoWin
	(concat("Error this implementation of ncurses only supports ",
		COLOR_PAIRS, " color pairs which is less than the required ",
		colorParams::gameColorPairsNo, " color pairs.\n"),
	 printCharSpeed, afterPrintSleep);
      exit(ERROR_WIN_PARAM);
    }
  
  return initColorPairs();
}


std::vector<int> initColorPairs()
{  
  struct colors
  {
    int r, g, b;
  };
  std::vector<colors> newColors {};
  std::vector<int> retColorPairIndexes {};

  // Calculate RGB values of game colors.
  for(int rIter {},
	redComp {}, greenComp {}, blueComp {};
      rIter < colorParams::gameColorCompNo; rIter++)
    {
      for(int gIter {}; gIter < colorParams::gameColorCompNo; gIter++)
	{
	  for(int bIter {}; bIter < colorParams::gameColorCompNo; bIter++)
	    {
	      newColors.push_back
		(colors{redComp, blueComp, greenComp});
	      // We inc after because colors should start at 0.
	      blueComp += colorParams::gameRGBMax /
		(colorParams::gameColorCompNo -1);
	    }
	  /* Ensure colors for inner loops are in the range [0, 1000] by setting
	     them back to 0 after exiting the inner loops. */
	  blueComp = 0;
	  greenComp += colorParams::gameRGBMax /
	    (colorParams::gameColorCompNo -1);
	}
      greenComp = 0;
      redComp += colorParams::gameRGBMax /
	(colorParams::gameColorCompNo -1);
    }

  // Init colors.
  for(int colorNoIter {}; colorNoIter < (int)newColors.size(); ++colorNoIter)
    {
      /* Note that the 0th color pair (which we are not using) is reserved for
	 the default color (which is one of the other color pairs). Hence + 1.
      */
      init_color(colorNoIter +1, newColors[colorNoIter].r,
		 newColors[colorNoIter].g, newColors[colorNoIter].b);
    }

  // Init color pairs.
  for(int fgIter {}; fgIter < (int)newColors.size(); ++fgIter)
    {
      for(int bgIter {}; bgIter < (int)newColors.size();
	  ++bgIter)
	{
	  /* Note that the 0th color pair (which we are not using) is reserved
	     for the default color (which is one of the other color pairs).
	     Hence + 1. */
	  const int pairNumber
	    {fgIter * (int)newColors.size() + bgIter + 1};
	  init_pair(pairNumber, fgIter, bgIter);
	  retColorPairIndexes.push_back(pairNumber);
	}      
    }

  return retColorPairIndexes;
}
