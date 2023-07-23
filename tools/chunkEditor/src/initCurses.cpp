#include "include/initCurses.hpp"
#include "include/utils.hpp"


std::vector<int> initialiseCurses(yx & maxyx)
{
  initscr();                                  // Start curses mode                                                     
  getmaxyx(stdscr, maxyx.y, maxyx.x); // Screen size in characters
  
  if(maxyx.y != yHeight || maxyx.x != xWidth) // Check the window size.                                                
    {
      std::stringstream e {};
      e<<"error windows must be "<<yHeight<<" characters high and "<<xWidth<<" characters wide. "
        "The current height is "<<maxyx.y<<" characters and the current width is "<<maxyx.x<<" characters.\n";
      exit(e.str(), ERROR_WIN_PARAM);
    }
  if(has_colors() == FALSE)  // If the terminal does not support colors                                                
    {
      exit("Error your terminal does not support colour :'(\n", ERROR_WIN_PARAM);
    }
  if(tigetnum("colors") < colorParams::gameColors)
    {
      exit(concat("Error your terminal only supports ", tigetnum("colors"),
		  " colors which is less than the required ",
		  colorParams::gameColors, " colors.\n"),
	   ERROR_WIN_PARAM);
    }
  
  raw();                     // To disable line buffering                                                              
  // curs_set(0);               // Make the cursor invisible                                                              
  noecho();                  // Turn echoing off on the terminal                                                       
  start_color();             // Start color and initialise color pairs!

  /* COLOR_PAIRS is 0 before start_color() is called! */
  if(COLOR_PAIRS < colorParams::gameColorPairsNo)
    {
      exit(concat("Error this implementation of ncurses only supports ",
		  COLOR_PAIRS, " color pairs which is less than the required ",
		  colorParams::gameColorPairsNo, " color pairs.\n"),
	   ERROR_WIN_PARAM);
    }
  
  // Dissable blocking while waiting for input (use non blocking sys call.)
  nodelay(stdscr, TRUE);
  
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
      init_color(colorNoIter, newColors[colorNoIter].r,
		 newColors[colorNoIter].g, newColors[colorNoIter].b);
    }

  // Init color pairs.
  for(int fgIter {}; fgIter < (int)newColors.size(); ++fgIter)
    {
      for(int bgIter {}; bgIter < (int)newColors.size();
	  ++bgIter)
	{
	  const int pairNumber
	    {fgIter * (int)newColors.size() + bgIter};
	  init_pair(pairNumber, fgIter, bgIter);
	  retColorPairIndexes.push_back(pairNumber);
	}      
    }
  
  // Set current color to default color.
  attron(COLOR_PAIR(colorParams::defaultColorPair));

  return retColorPairIndexes;
}
