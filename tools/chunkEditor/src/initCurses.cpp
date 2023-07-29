#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include <curses.h>


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

  int redComp {}, greenComp {}, blueComp {};
  // Calculate RGB values of game colors.
  for(int rIter {};
      rIter < colorParams::gameColorCompNo; rIter++)
    {
      for(int gIter {}; gIter < colorParams::gameColorCompNo; gIter++)
	{
	  for(int bIter {}; bIter < colorParams::gameColorCompNo; bIter++)
	    {
	      newColors.push_back
		(colors{redComp, blueComp, greenComp});
	      // We inc after because colors should start at 0.
	      blueComp += (colorParams::gameRGBMax /
			   (colorParams::gameColorCompNo -1)); // * 0.8;
	    }
	  /* Ensure colors for inner loops are in the range [0, 1000] by setting
	     them back to 0 after exiting the inner loops. */
	  blueComp = 0;
	  greenComp += (colorParams::gameRGBMax /
			(colorParams::gameColorCompNo -1)); // * 0.8;
	}
      greenComp = 0;
      redComp += (colorParams::gameRGBMax /
		  (colorParams::gameColorCompNo -1)); // * 0.8;
    }

  //   // Calculate RGB values of game colors.
  // for(int rIter {};
  //     rIter < colorParams::gameColorCompNo -1; rIter++)
  //   {
  //     for(int gIter {}; gIter < colorParams::gameColorCompNo; gIter++)
  // 	{
  // 	  for(int bIter {}; bIter < colorParams::gameColorCompNo; bIter++)
  // 	    {
  // 	      newColors.push_back
  // 		(colors{redComp, blueComp, greenComp});
  // 	      // We inc after because colors should start at 0.
  // 	      blueComp += (colorParams::gameRGBMax /
  // 			   (colorParams::gameColorCompNo -1)) /2;
  // 	    }
  // 	  /* Ensure colors for inner loops are in the range [0, 1000] by setting
  // 	     them back to 0 after exiting the inner loops. */
  // 	  blueComp = 0;
  // 	  greenComp += (colorParams::gameRGBMax /
  // 			(colorParams::gameColorCompNo -1)) / 2;
  // 	}
  //     greenComp = 0;
  //     redComp += (colorParams::gameRGBMax /
  // 		  (colorParams::gameColorCompNo -1)) / 2;
  //   }

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
  int pairNumber {1};
  // Assign first half of the colors to the forground.
  for(int fgIter {}; fgIter < (int)newColors.size() / 2; ++fgIter)
    {
      // Assign second half of the colors to the background.
      for(int bgIter {(int)newColors.size() / 2}; bgIter < (int)newColors.size();
	  ++bgIter)
	{
	  /* Note that the 0th color pair (which we are not using) is reserved
	     for the default color (which is one of the other color pairs).
	     Hence + 1. */
	  init_pair(pairNumber, fgIter, bgIter);
	  retColorPairIndexes.push_back(pairNumber);
	  pairNumber++;
	}      
    }
  
  //     endwin();
  // std::cout<<pairNumber<<'\n';
  // exit(-1);

  // attron(A_REVERSE);
  // pairNumber = 1;
  // for(int yIter {}; yIter < 13; yIter++)
  //   {
  //     for(int xIter {}; xIter < 14; ++xIter)
  // 	{
  // 	  attron(COLOR_PAIR(pairNumber++));
  // 	  mvprintw(yIter, xIter, "#");
  // 	}
  //   }

  // attroff(A_REVERSE);
  // pairNumber = 1;
  // for(int yIter {13}; yIter < 27; yIter++)
  //   {
  // 	for(int xIter {}; xIter < 14; ++xIter)
  // 	  {
  // 	    attron(COLOR_PAIR(pairNumber++));
  // 	    mvprintw(yIter, xIter, "#");
  // 	  }
  //   }

    endwin();



  std::vector<yx> colors {};

  // We can have 253 color pairs (506 with inversion) if we have 23 colors.
  for(int fIter {}; fIter < 23; ++fIter)
    {
      for(int bIter {}; bIter < 23; ++bIter)
	{
	  colors.push_back(yx{fIter, bIter});
	}
    }

  
  // std::vector<yx> colorsCopy {colors};


  // std::cout<<"before\n";
  // for(auto a: colors)
  //   {
  //     std::cout<<a<<'\t';
  //   }
  // std::cout<<'\n'<<colors.size()<<std::endl;
  // std::cout<<std::endl;

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
     is colors.size() before the loop. */
  for(int oIter {}; oIter < colors.size(); ++oIter)
    {
      for(int iIter {}; iIter < colors.size(); ++iIter)
	{
	  if(oIter != iIter &&
	     colors[oIter].y == colors[iIter].x &&
	     colors[oIter].x == colors[iIter].y)
	    {
	      /* Remove reverse color (this can be printed using
		 attron(A_REVERSE) / attroff(A_REVERSE); */
	      // Remove from copy.
	      removeFrom(colors, colors[iIter]);
	      if(oIter > iIter)
		{
		  --oIter;
		}
	    }
	}
      if(colors[oIter].y == colors[oIter].x)
	{
	  // A solid character can be achieved with a ' '.
	  // Remove from copy.
	  removeFrom(colors, colors[oIter]);
	}
    }

  std::cout<<"after\n";
  for(auto a: colors)
    {
      std::cout<<a<<'\t';
    }
  std::cout<<'\n'<<colors.size()<<std::endl;
  std::cout<<std::endl;
  

  exit(-1);
  // refresh();
  // endwin();

    // for(int fgIter {}; fgIter < (int)newColors.size(); ++fgIter)
    //   {
    // 	for(int bgIter {}; bgIter < (int)newColors.size() /2;
    // 	    ++bgIter)
    // 	  {
    // 	    attron(COLOR_PAIR(fgIter * (int)newColors.size() + bgIter + ((int)newColors.size() /2) + 1));
    // 	    mvprintw(fgIter, bgIter + (int)newColors.size() /2, "%");
    // 	  }
    // 	refresh();
    //   }
    // endwin();
    exit(-1);
      // // 12
      // // 14
      // /// 34
      // attron(COLOR_PAIR(xIter + 1 + 125 * 37));
      // mvprintw(0, xIter, "X");
      // refresh();
  //   }
  // endwin();
  // exit(-1);
  
  // /* Set current color to first color pair (not we are not using the default
  //    color pair 0). */
  // attron(COLOR_PAIR());

  return retColorPairIndexes;
}
