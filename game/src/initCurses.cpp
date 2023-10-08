#include "include/initCurses.hpp"
#include "include/utils.hpp"


void initialiseCurses(yx & maxyx)
{
  initscr();                                  // Start curses mode                                                     
  getmaxyx(stdscr, maxyx.y, maxyx.x); // Screen size in characters
  
  if(maxyx.y != yHeight || maxyx.x != xWidth) // Check the window size.                                                
    {
      exit(concat
	   ("error windows must be ", yHeight, " characters high and ", xWidth,
	    " characters wide. The current height is ", maxyx.y, " characters "
	    "and the current width is ", maxyx.x, " characters.\n"),
	   ERROR_WIN_PARAM);
    }
  if(!has_colors())  // If the terminal does not support colors                                                
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
  curs_set(0);               // Make the cursor invisible                                                              
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
  initColorPairs();          // Initialise the color pairs                                                             
}


void initColorPairs()
{  
  struct colors
  {
    int r, g, b;
  };
  std::vector<colors> newColors {};

  int redComp {}, greenComp {}, blueComp {};

  /* See colorS.hpp for more info about how we are calculating the colors. */
  // CALCULATE RGB VALUES OF GAME COLORS. ======================================
  for(int bIter {};
      bIter < colorParams::gameColorCompNo; bIter++)
    {
      blueComp += (colorParams::gameRGBMax /
		  colorParams::gameColorCompNo); // * 0.8;
      for(int gIter {}; gIter < colorParams::gameColorCompNo; gIter++)
	{
	  /* Ensure colors for inner loops are in the range [333, 1000] by setting
	     them back to 0 after exiting the inner loops. */
	  greenComp += (colorParams::gameRGBMax /
			colorParams::gameColorCompNo); // * 0.8;
	  for(int rIter {}; rIter < colorParams::gameColorCompNo -1; rIter++)
	    {
	      // We inc after because colors shouldn't start at 0.
	      redComp += (colorParams::gameRGBMax /
			  (colorParams::gameColorCompNo -1)); // * 0.8;
	      newColors.push_back
		({redComp, blueComp, greenComp});
	    }
	  redComp = 0;
	}
      greenComp = 0;
    }

  /* Add 5 extra colors. We need 23. */
  // Add black.
  newColors.push_back
    (colors{0, 0, 0});
  // Add darkish brown.
  newColors.push_back(colors
		      {colorParams::gameRGBMax / colorParams::gameColorCompNo,
		       colorParams::gameRGBMax / colorParams::gameColorCompNo,
		       colorParams::gameRGBMax / colorParams::gameColorCompNo});
  // Add middle brown.
  newColors.push_back(colors{colorParams::gameRGBMax / 2,
			     colorParams::gameRGBMax / 2,
			     colorParams::gameRGBMax / 2});
  // Add off middle brown (with less green.)
  newColors.push_back
    (colors{colorParams::gameRGBMax / 2,
	    colorParams::gameRGBMax / 2 -
	    (colorParams::gameRGBMax / colorParams::gameColorCompNo) / 2,
	    colorParams::gameRGBMax / 2});
  // Add off middle brown (with less blue.)
  newColors.push_back
    (colors{colorParams::gameRGBMax / 2,
	    colorParams::gameRGBMax / 2,
	    colorParams::gameRGBMax / 2 -
	    (colorParams::gameRGBMax / colorParams::gameColorCompNo) / 2});

  /* CALCULATE INDEXES OF COLORS FOR COLOR PAIRS. ==============================
     These will be used to index into the newColors vector when creating color
     pairs. */
  std::vector<yx> indexesOfColorsForPairs {};

  // We can have 253 color pairs (506 with inversion) if we have 23 colors.
  for(int fIter {}; (size_t)fIter < newColors.size(); ++fIter)
    {
      for(int bIter {}; (size_t)bIter < newColors.size(); ++bIter)
	{
	  indexesOfColorsForPairs.push_back(yx{fIter, bIter});
	}
    }

  // Remove elm from vec.
  auto removeFrom = [](std::vector<yx> & vec, const yx elm)
  {
    auto it = std::find(vec.begin(), vec.end(), elm);

    if (it != vec.end())
      {
	vec.erase(it);
      }    
  };

  /* Size of colors after for loop will be (cS * cS) / 2 - (cS / 2), where cS
     is indexesOfColorsForPairs.size() before the loop. */
  for(int oIter {}; (size_t)oIter < indexesOfColorsForPairs.size(); ++oIter)
    {
      for(int iIter {}; (size_t)iIter < indexesOfColorsForPairs.size(); ++iIter)
	{
	  if(oIter != iIter &&
	     indexesOfColorsForPairs[oIter].y ==
	     indexesOfColorsForPairs[iIter].x &&
	     indexesOfColorsForPairs[oIter].x ==
	     indexesOfColorsForPairs[iIter].y)
	    {
	      /* Remove reverse color (this can be printed using
		 attron(A_REVERSE) / attroff(A_REVERSE); */
	      // Remove from copy.
	      removeFrom(indexesOfColorsForPairs,
			 indexesOfColorsForPairs[iIter]);
	      if(oIter > iIter)
		{
		  --oIter;
		}
	    }
	}
      if(indexesOfColorsForPairs[oIter].y == indexesOfColorsForPairs[oIter].x)
	{
	  // A solid character can be achieved with a ' '.
	  // Remove from copy.
	  removeFrom(indexesOfColorsForPairs, indexesOfColorsForPairs[oIter]);
	}
    }

  // Init colors. ==============================================================
  for(int colorNoIter {}; colorNoIter < (int)newColors.size(); ++colorNoIter)
    {
      /* Note that the 0th color pair (which we are not using) is reserved for
	 the default color (which is one of the other color pairs). Hence + 1.
      */
      init_color(colorNoIter +1, newColors[colorNoIter].r,
		 newColors[colorNoIter].g, newColors[colorNoIter].b);
    }

  // Init color pairs. =========================================================
  /* Pairs start at 1 (we are not using the default pair functionality of
     ncurses, where the default pair is 0.) */
  int pairNumber {1};
  for(yx newPairsColorIndexes: indexesOfColorsForPairs)
    {
      init_pair(pairNumber, newPairsColorIndexes.y, newPairsColorIndexes.x);
      pairNumber++;
    }
}
