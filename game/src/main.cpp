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


void menu
(const yx viewPortSize, backgroundData::drawBufferType * secondStageDrawBuffer);
/* Where the horror happens
   (returns a game menu switch option.) :) */
int gameLoop
(backgroundData::drawBufferType * secondStageDrawBuffer,
 backgroundData & background, rules & levelRules);


int main()
{
  /* Holds the window size (currently (should hole the view port size in the
     future.) */
  yx viewPortSize;
  initialiseCurses(viewPortSize);	// Start and setup ncurses
  // Allocate memory for drawBuffer.
  backgroundData::drawBufferType * secondStageDrawBuffer
    = new backgroundData::drawBufferType [viewPortSize.y * viewPortSize.x];
  
  menu(viewPortSize, secondStageDrawBuffer);
  
  endwin(); //end curses mode!
  delete [] secondStageDrawBuffer;
  return 0;
}


void menu
(const yx viewPortSize, backgroundData::drawBufferType * secondStageDrawBuffer)
{
  backgroundData background
    {viewPortSize, "assets/level1/level1.background.lev"};
  rules levelRules
    {viewPortSize, "assets/level1/level1.coordRules.lev",
     "assets/level1/level1.rules.lev", background};
  background.initFirstStageDrawBuffer
    (levelRules.gamePlayer->initialViewPortPosition);
  
  bool run = true;
  while(run)
    {
      switch(gameLoop(secondStageDrawBuffer, background, levelRules))
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
(backgroundData::drawBufferType * secondStageDrawBuffer,
 backgroundData & background, rules & levelRules)
{
  std::__1::chrono::steady_clock::time_point secStartTime
    {std::chrono::high_resolution_clock::now()};
  
  while(true)
    {
      sprite::directions input {};

      // const size_t backgroundLen {background.size() /
      // 	viewPortSize.y};

      input = player::convertDirectionCharsToDirections
	(static_cast<player::directionChars>(getch()));
      switch(input)
	{
	case ESC_CHAR:
	  return M_QUIT_GAME;
	  break;
	case LEVEL_COMPLEAT:
	  return LEVEL_COMPLEAT;
	  break;
	}

      levelRules.physics
	(background, input, secStartTime);
      draw(secondStageDrawBuffer, background, levelRules.gamePlayer,
	   levelRules.bgSprites);
    }
}
