#include <sstream>
#include <string>
#include "initial/initCurses.h++"
#include "common.h++"
#include "initial/load/loadAssets.h++"
#include "initial/checkBoundsOfBounds/checkBoundsOfBounds.h++"
#include "physics/physics.h++"
#include "draw/draw.h++"
#include "draw/colorS.h++"
#include "draw/drawExit.h++"
#include "sprite/sprite.h++"
#include "sprite/player/player.h++"
#include "sprite/player/direction.h++"


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
gameFuncRetCodes branchOnInput(rules & levelRules);
//void freeResources(std::vector<sprite *> & spArray);


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
  for(int iter{}; iter < 126; ++iter)
    {
      auto startTimeInput = std::chrono::high_resolution_clock::now();// Control's background update time.
      auto currentTimeInputBackground = std::chrono::high_resolution_clock::now();// Control's background update time.
      while(std::chrono::duration_cast<std::chrono::milliseconds>(currentTimeInputBackground - startTimeInput).count()
	    < 225)
	{
	  char input {};
	  std::stringstream errorMsg;
	  physics(levelRules);
	  input = branchOnInput(levelRules);
	  if(input == M_QUIT_GAME)
	    return M_QUIT_GAME;
	  draw(background, levelRules.spriteCoords, levelRules.gamePlayer, maxyx, iter);
	  refresh();      

	  sleep(32);
	  currentTimeInputBackground =std::chrono::high_resolution_clock::now();
	}
    }
  return LEVEL_COMPLEAT;
}


gameFuncRetCodes branchOnInput(rules & levelRules)
{ /* Getch() is supposed to return ERR if noblock() has been called and no character's are entered, however I will not
     test it against ERR because doing so seems to negate the effect of calling noblock() for some reason that I
     cannot wrap my head around an cannot find any good documentation on. So am assuming that ERR contains a negative
     value. */
  char input{};
  if((input = getch()))	// Brackets inhibit compiler warining.
    {
      switch((int)input)
	{	/* I've added these because I can. Although less code is probably optimal and feature bloat
		   should be faught. */
	case 'w':
	case 'W':
	  levelRules.gamePlayer->updatePosRel('w');
	  return INPUT_ENTERED;
	case 'a':
	case 'A':
	  levelRules.gamePlayer->updatePosRel('a');
	  return INPUT_ENTERED;
	case 's':
	case 'S':
	  levelRules.gamePlayer->updatePosRel('s');
	  return INPUT_ENTERED;
	case 'd':
	case 'D':
	  levelRules.gamePlayer->updatePosRel('d');
	  return INPUT_ENTERED;
	case ESC_CHAR:	// Esc.
	  return M_QUIT_GAME;
	}
    }
  
  return NO_INPUT_ENTERED;
}
