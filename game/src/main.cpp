#include <sstream>
#include <string>
#include "include/initCurses.hpp"
#include "include/common.hpp"
#include "include/loadAssets.hpp"
#include "include/checkBoundsOfBounds.hpp"
#include "include/physics.hpp"
#include "include/draw.hpp"
#include "include/colorS.hpp"
#include "include/drawExit.hpp"
#include "include/sprite.hpp"
#include "include/player.hpp"


// Values returned by branchOnInput
enum gameFuncRetCodes
  {
    NO_INPUT_ENTERED,		// The player did not enter any input.
    INPUT_ENTERED,		// The player entered input.
    M_QUIT_GAME,		// The player has decided to quit the game.    
    LEVEL_COMPLEAT		// The player has compleated the level.
  };


void menu(const yx maxyx); // Game menu
// Where the magic (thats glued together with cheap craft glue) happens (returns a game menu switch option.) :)
int gameLoop(const yx maxyx, const std::vector<int> & background, rules & levelRules);


int main()
{
  yx maxyx;		// Holds the window size.
  initialiseCurses(maxyx);	// Start and setup ncurses
  menu(maxyx);
  
  endwin();//end curses mode!
  return 0;
}


void menu(const yx maxyx)
{
  std::vector<int> background {};	// Hold's the background
  // Hold's the "rules" for the current level. (see physics.h and rules.lev.txt.)
  rules levelRules {};
  /* Note this should be done in the menu or loop or some sub function
     called from within it since multiple level's can be played. It is
     placed here right now only for testing and development purposes. */
  loadAssets(maxyx, "assets/level1/level1.backgound.lev", background,
	     "assets/level1/level1.rules.lev", levelRules);   	// Load game files.
  
  bool run = true;
  while(run)
    {
      switch(gameLoop(maxyx, background, levelRules))
	{
	case M_QUIT_GAME:
	  run = false;
	  break;
	case LEVEL_COMPLEAT:
	  run = false;		// TEMPORARY!!!!!!!!!!!!!!!
	  break;
	}
    }
  //  freeResources(levelRules);
  drawExit(maxyx);  
}


int gameLoop(const yx maxyx, const std::vector<int> & background, rules & levelRules)
{
  auto startTimeInput = std::chrono::high_resolution_clock::now();// Control's background update time.
  auto currentTimeInputBackground = std::chrono::high_resolution_clock::now();// Control's background update time.
  while(true)
    {
      do
	{
	  int input {};
	  const size_t backgroundLen {background.size() / BACKGROUND_HEIGHT};
	  int position {};
	  
	  input = getch();
	  if(input == ESC_CHAR)
	    return M_QUIT_GAME;
	  else
	    if(input == LEVEL_COMPLEAT)
	      return LEVEL_COMPLEAT;

	  levelRules.physics(input, position, maxyx, backgroundLen);
	  draw(background, levelRules.spriteCoords, levelRules.gamePlayer, maxyx, position);
	  refresh();
	  
	  sleep(32);
	  currentTimeInputBackground = std::chrono::high_resolution_clock::now();
	}
      while(std::chrono::duration_cast<std::chrono::milliseconds>
	    (currentTimeInputBackground - startTimeInput).count() < 255);
      startTimeInput = std::chrono::high_resolution_clock::now();
    }
}
