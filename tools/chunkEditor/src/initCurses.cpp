#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include <curses.h>


void initialiseCurses(yx & maxyx)
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
  if(!has_colors())  // If the terminal does not support colors                                                
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

  mousemask(0, nullptr);	// Disable mouse input.
  raw();			// To disable line buffering
  // curs_set(0);               // Make the cursor invisible
  disableBlockingUserInput();
  start_color();             // Start color and initialise color pairs!

  /* COLOR_PAIRS is 0 before start_color() is called! */
  if(COLOR_PAIRS < colorParams::gameColorPairsNoMax)
    {
      printMessageNoWin
	(concat("Error this implementation of ncurses only supports ",
		COLOR_PAIRS, " color pairs which is less than the required ",
		colorParams::gameColorPairsNoMax, " color pairs.\n"),
	 printCharSpeed, afterPrintSleep);
      exit(ERROR_WIN_PARAM);
    }
  
  initColorPairs();
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
  for(int fIter {}; fIter < newColors.size(); ++fIter)
    {
      for(int bIter {}; bIter < newColors.size(); ++bIter)
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
  for(int oIter {}; oIter < indexesOfColorsForPairs.size(); ++oIter)
    {
      for(int iIter {}; iIter < indexesOfColorsForPairs.size(); ++iIter)
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


  // for(yx indexs: indexesOfColorsForPairs)
  //   {
  //     if(indexs.y == 14 && indexs.x == 9 ||
  // 	 indexs.x == 14 && indexs.y == 9)
  // 	{
  // 	  endwin();
  // 	  std::cout<<"found y = "<<indexs.y<<" and x = "<<indexs.x<<'\n';
  // 	  exit(-1);
  // 	}
  //   }

  // endwin();
  // std::cout<<indexesOfColorsForPairs.size();
  // exit(-1);
  

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

  // attron(A_REVERSE);
  // pairNumber = 1;
  // for(int a: retColorPairIndexes)
  //   {
  //     attron(COLOR_PAIR(pairNumber++));
  //     printw(" ");
  //   }
  // move(2, 0);
  // attroff(A_REVERSE);
  // pairNumber = 1;
  // for(int a: retColorPairIndexes)
  //   {
  //     attron(COLOR_PAIR(pairNumber++));
  //     printw(" ");
  //   }  
  // refresh();
  // endwin();
  // exit(-1);
}
