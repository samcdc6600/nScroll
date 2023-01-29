#include <sstream>
#include <string>
#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include "include/background.hpp"
#include "include/levelRules.hpp"
#include "include/draw.hpp"
#include "include/colorS.hpp"
#include "include/drawExit.hpp"
#include "include/sprite.hpp"
#include "include/player.hpp"

  // TMP ======================================== VVVVVVV
#include <iostream>
  // TMP ======================================== ^^^^^^^


// Values returned by branchOnInput
enum gameFuncRetCodes
  {
    NO_INPUT_ENTERED,		// The player did not enter any input.
    INPUT_ENTERED,		// The player entered input.
    M_QUIT_GAME,		// The player has decided to quit the game.    
    LEVEL_COMPLEAT		// The player has compleated the level.
  };


void menu(const yx viewPortSize, unsigned short * secondStageDrawBuffer);
/* Where the horror happens
   (returns a game menu switch option.) :) */
int gameLoop
(unsigned short * secondStageDrawBuffer, backgroundData & background,
 rules & levelRules, const yx viewPortSize);


int main()
{
  /* Holds the window size (currently (should hole the view port size in the
     future.) */
  yx viewPortSize;
  initialiseCurses(viewPortSize);	// Start and setup ncurses
  // Allocate memory for drawBuffer.
  unsigned short * secondStageDrawBuffer
    = new unsigned short [viewPortSize.y * viewPortSize.x];
  
  menu(viewPortSize, secondStageDrawBuffer);
  
  endwin(); //end curses mode!
  delete [] secondStageDrawBuffer;
  return 0;
}


void menu(const yx viewPortSize, unsigned short * secondStageDrawBuffer)
{
  // std::vector<int> background {};	// Hold's the background
  backgroundData background
    {viewPortSize, "assets/level1/level1.background.lev"};
  /* Hold's the "rules" for the current level. (see physics.h and
     rules.lev.txt.) */
  rules levelRules
    {viewPortSize, "assets/level1/level1.coordRules.lev",
     "assets/level1/level1.rules.lev", background};

  /* Note this should be done in the menu or loop or some sub function
     called from within it since multiple level's can be played. It is
     placed here right now only for testing and development purposes. */
  
  /*  loadAssets(viewPortSize, "assets/level1/level1.backgound.lev", background,
      "assets/level1/level1.rules.lev", levelRules);*/
  
  
  bool run = true;
  while(run)
    {
      /* REPLACE THE BELOW WITH THE FOLLOWING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 REPLACE THE BELOW WITH THE FOLLOWING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 REPLACE THE BELOW WITH THE FOLLOWING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 REPLACE THE BELOW WITH THE FOLLOWING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 REPLACE THE BELOW WITH THE FOLLOWING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	switch(gameLoop
	(secondStageDrawBuffer, background, levelRules, )) */
      switch(gameLoop
	     (secondStageDrawBuffer, background, levelRules, viewPortSize))
	{
	case M_QUIT_GAME:
	  run = false;
	  break;
	case LEVEL_COMPLEAT:
	  run = false;		// TEMPORARY!!!!!!!!!!!!!!!
	  break;
	}
    }

  drawExit(viewPortSize);  
}


int gameLoop
(unsigned short * secondStageDrawBuffer, backgroundData & background,
 rules & levelRules, const yx viewPortSize)
{
  std::__1::chrono::steady_clock::time_point secStartTime
    {std::chrono::high_resolution_clock::now()};
  // NOTE THAT VIEWPORTPOSITION SHOULD BE INITIALISED RELATIVE TO THE PLAYER
  // POSITION! (I.E. WE NEED TO FIX THIS AT SOME POINT.)
  yx viewPortPosition {24, 170};
  
  while(true)
    {
      int input {};

      // const size_t backgroundLen {background.size() /
      // 	viewPortSize.y};

      input = getch();
      switch(input)
	{
	case ESC_CHAR:
	  return M_QUIT_GAME;
	  break;
	case LEVEL_COMPLEAT:
	  return LEVEL_COMPLEAT;
	  break;
	}

      // levelRules.physics(player::directionChars(input), viewPortPosition, viewPortSize,
      // 			 backgroundLen, secStartTime);
      draw(secondStageDrawBuffer, background, levelRules.gamePlayer,
	   levelRules.bgSprites, viewPortSize, viewPortPosition);
    }
}
