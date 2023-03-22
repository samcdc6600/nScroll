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
    M_QUIT_GAME = 27,		// The player has decided to quit the game.
  };


void menu
(const yx viewPortSize);
/* Where the horror happens
   (returns a game menu switch option.) :) */
int gameLoop
(std::__1::chrono::steady_clock::time_point lastTickTime,
 backgroundData & background, rules & levelRules);


int main()
{
  /* Holds the window size (currently (should hole the view port size in the
     future.) */
  yx viewPortSize;
  initialiseCurses(viewPortSize);	// Start and setup ncurses
  
  menu(viewPortSize);
  
  endwin(); //end curses mode!

  return 0;
}


void menu
(const yx viewPortSize)
{
  std::__1::chrono::steady_clock::time_point startTick
    {std::chrono::high_resolution_clock::now()};
  
  backgroundData background
    {viewPortSize, "assets/level1/level1.background.lev"};
  rules levelRules
    {viewPortSize, "assets/level1/level1.coordRules.lev",
     "assets/level1/level1.rules.lev", background};
  background.initFirstStageBuffer
    (levelRules.gamePlayer->initialViewPortPosition);
  levelRules.initFirstStageBuffer
    (levelRules.gamePlayer->initialViewPortPosition);
  
  bool run = true;
  while(run)
    {
      switch(gameLoop(startTick, background, levelRules))
	{
	case M_QUIT_GAME:
	  run = false;
	  break;
	}
    }

  drawExit(viewPortSize);  
}


int gameLoop
(std::__1::chrono::steady_clock::time_point lastTickTime,
 backgroundData & background, rules & levelRules)
{
  levelRules.startTimers();
  
  while(true)
    {
      char input {};

      input = getch();
      switch(input)
	{
	case ESC_CHAR:
	  return M_QUIT_GAME;
	  break;
	}
      
      levelRules.physics
	(background, player::convertDirectionCharsToDirections
	 (static_cast<player::directionChars>(input)));
      draw(background, levelRules.gamePlayer, levelRules.bgSprites);
    }
}
