#include <ncurses.h>
#include <sstream>
#include <iostream>
#include <system_error>
#include <stdexcept>
#include <string>
#include "common.h"
#include "io/inputHandlerNonBlock.h"
#include "initial/load/loadAssets.h"
#include "initial/collapse/collapse.h"
#include "initial/checkBoundsOfBounds/checkBoundsOfBounds.h"
#include "physics/physics.h"
#include "draw/draw.h"
#include "draw/colorS.h"
#include "draw/drawExit.h"
#include "sprite/sprite.h"
#include "sprite/player/player.h"
#include "sprite/player/direction.h"


// Game menu switch options (to be returned by gameLoop.)
#define M_QUIT_GAME      1
#define M_LEVEL_COMPLEAT 2
constexpr int yHeight {33}, xWidth{125};	// The window must be these dimensions


void initialiseCurses(yx & maxyx);
void initColorPairs();
void menu(const yx maxyx); // Game menu
/* This function should initialise the background variable (it's second argument), which should then contain the
   "grphical data" of the level. It should initialise and return the levelRules variable (it's third argument),
   which should then contain the "rules and non player sprites" as well as the player sprite of the level. */
void loadAssets(const yx maxyx, std::vector<int> & background, rules & levelRules);
// Where the magic (thats glued together with cheap craft glue) happens (returns a game menu switch option.) :)
int gameLoop(const yx maxyx, const std::vector<int> & background, rules & levelRules);
//void freeResources(std::vector<sprite *> & spArray);


int main()
{
  yx maxyx;		// Holds the window size.
  initialiseCurses(maxyx);	// Start and setup ncurses
  menu(maxyx);
  
  endwin();//end curses mode!
  return 0;
}

void initialiseCurses(yx & maxyx)
{
  initscr();				      // Start curses mode
  getmaxyx(stdscr, maxyx.y, maxyx.x); // Screen size in characters
  if(maxyx.y != yHeight || maxyx.x != xWidth) // Check the window size.
    {
      endwin();
      std::cerr<<"error windows must be "<<yHeight<<" characters high and "<<xWidth<<" characters wide. "
	"The current height is "<<maxyx.y<<" characters and the current width is "<<maxyx.x<<" characters.\n";
      exit(ERR_WIN_PARAM);
    }   
  if(has_colors() == FALSE)  // If the terminal does not support colors
    {
      endwin();
      std::cerr<<"error your terminal does not support colour :'(\n";
      exit(ERR_WIN_PARAM);
    }
  raw();		     // To disable line buffering
  curs_set(0);		     // Make the cursor invisible
  noecho();		     // Turn echoing off on the terminal
  start_color();	     // Start color and initialise color pairs!
  initColorPairs();	     // Initialise the color pairs
}

void initColorPairs()
{
  attron(COLOR_PAIR(56));	//default color. Must also be changed in draw.cpp!!!!!!!!!!!!!!!!!!!!!!!!
  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_RED);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_BLACK, COLOR_BLUE);
  init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(7, COLOR_BLACK, COLOR_CYAN);
  init_pair(8, COLOR_BLACK, COLOR_WHITE);
  init_pair(9, COLOR_RED, COLOR_BLACK);
  init_pair(10, COLOR_RED, COLOR_RED);
  init_pair(11, COLOR_RED, COLOR_GREEN);
  init_pair(12, COLOR_RED, COLOR_YELLOW);
  init_pair(13, COLOR_RED, COLOR_BLUE);
  init_pair(14, COLOR_RED, COLOR_MAGENTA);
  init_pair(15, COLOR_RED, COLOR_CYAN);
  init_pair(16, COLOR_RED, COLOR_WHITE);
  init_pair(17, COLOR_GREEN, COLOR_BLACK);
  init_pair(18, COLOR_GREEN, COLOR_RED);
  init_pair(19, COLOR_GREEN, COLOR_GREEN);
  init_pair(20, COLOR_GREEN, COLOR_YELLOW);
  init_pair(21, COLOR_GREEN, COLOR_BLUE);
  init_pair(22, COLOR_GREEN, COLOR_MAGENTA);
  init_pair(23, COLOR_GREEN, COLOR_CYAN);
  init_pair(24, COLOR_GREEN, COLOR_WHITE);
  init_pair(25, COLOR_YELLOW, COLOR_BLACK);
  init_pair(26, COLOR_YELLOW, COLOR_RED);
  init_pair(27, COLOR_YELLOW, COLOR_GREEN);
  init_pair(28, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(29, COLOR_YELLOW, COLOR_BLUE);
  init_pair(30, COLOR_YELLOW, COLOR_MAGENTA);
  init_pair(31, COLOR_YELLOW, COLOR_CYAN);
  init_pair(32, COLOR_YELLOW, COLOR_WHITE);
  init_pair(33, COLOR_BLUE, COLOR_BLACK);
  init_pair(34, COLOR_BLUE, COLOR_RED);
  init_pair(35, COLOR_BLUE, COLOR_GREEN);
  init_pair(36, COLOR_BLUE, COLOR_YELLOW);
  init_pair(37, COLOR_BLUE, COLOR_BLUE);
  init_pair(38, COLOR_BLUE, COLOR_MAGENTA);
  init_pair(39, COLOR_BLUE, COLOR_CYAN);
  init_pair(40, COLOR_BLUE, COLOR_WHITE);
  init_pair(41, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(42, COLOR_MAGENTA, COLOR_RED);
  init_pair(43, COLOR_MAGENTA, COLOR_GREEN);
  init_pair(44, COLOR_MAGENTA, COLOR_YELLOW);
  init_pair(45, COLOR_MAGENTA, COLOR_BLUE);
  init_pair(46, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(47, COLOR_MAGENTA, COLOR_CYAN);
  init_pair(48, COLOR_MAGENTA, COLOR_WHITE);
  init_pair(49, COLOR_CYAN, COLOR_BLACK);
  init_pair(50, COLOR_CYAN, COLOR_RED);
  init_pair(51, COLOR_CYAN, COLOR_GREEN);
  init_pair(52, COLOR_CYAN, COLOR_YELLOW);
  init_pair(53, COLOR_CYAN, COLOR_BLUE);
  init_pair(54, COLOR_CYAN, COLOR_MAGENTA);
  init_pair(55, COLOR_CYAN, COLOR_CYAN);
  init_pair(56, COLOR_CYAN, COLOR_WHITE);
  init_pair(57, COLOR_WHITE, COLOR_RED);
  init_pair(58, COLOR_WHITE, COLOR_GREEN);
  init_pair(59, COLOR_WHITE, COLOR_YELLOW);
  init_pair(60, COLOR_WHITE, COLOR_BLUE);
  init_pair(61, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(62, COLOR_WHITE, COLOR_CYAN);
  init_pair(63, COLOR_WHITE, COLOR_WHITE);
}


void loadAssets(const yx maxyx, std::vector<int> & background, rules & levelRules)
{
  try
    {   
      {				// Initialise background.
	std::string backgroundFileName {"assets/level1/level1.backgound.lev"};
	std::string levelBackGround {};
	if(!loadASCIIFile(backgroundFileName, levelBackGround))
	  {
	    std::stringstream errorMsg;
	    errorMsg<<"\"./assets/level1/level1.background.lev\" not found!\n";
	    throw std::logic_error(errorMsg.str());
	  }
	collapse(levelBackGround, background); //collapse nonColor escape sequences.
      }

      // THESE SHOULD BE SPECIFIED IN THE HEADER OF THE RULES FILE!!!!!!!!!!!!!!!1
      yx spritePos = {10, 10}; // Set inital position for player.
      levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
				      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
				      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player      
    }
  catch(std::logic_error errorMsg)
    {     
      mvprintw(0, 0, "%s", errorMsg.what());
      refresh();
      sleep(20000);
      endwin();
      exit(-1);
    }
}


void menu(const yx maxyx)
{
  std::vector<int> background {};	// Hold's the background
  // Hold's the "rules" for the current level. (see physics.h and rules.lev.txt.)
  rules levelRules {};
  /* Note this should be done in the menu or loop or some sub function
     called from within it since multiple level's can be played. It is
     placed here right now only for testing and development purposes. */
  loadAssets(maxyx, background, levelRules);   	// Load game files.
  
  bool run = true;
  while(run)
    {
      switch(gameLoop(maxyx, background, levelRules))
	{
	case M_QUIT_GAME:
	run = false;
	break;
	case M_LEVEL_COMPLEAT:
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
      char input{};	// Hold's character returned from the HandlerNonBlock() function.
      auto startTimeInput = std::chrono::high_resolution_clock::now();// Control's background update time.
      auto currentTimeInputBackground = std::chrono::high_resolution_clock::now();// Control's background update time.
      while(std::chrono::duration_cast<std::chrono::milliseconds>(currentTimeInputBackground - startTimeInput).count()
	    < 225)
	{
	  		  std::stringstream errorMsg;
	  physics(levelRules);
	  if((input = inputHandlerNonBlock()))
	    {
	      switch((int)input)
		{	/* I've added these because I can. Although less code is probably optimal and feature bloat
			   should be faught. */
		case 'w':
		case 'W':
		  levelRules.gamePlayer->updatePosRel('w');
		  break;
		case 'a':
		case 'A':
		  levelRules.gamePlayer->updatePosRel('a');
		  break;
		case 's':
		case 'S':
		  levelRules.gamePlayer->updatePosRel('s');
		  break;
		case 'd':
		case 'D':
		  levelRules.gamePlayer->updatePosRel('d');
		  break;
		case 27:	// Esc.
		  return M_QUIT_GAME;
		}
	      draw(background, levelRules.spriteCoords, levelRules.gamePlayer, maxyx, iter);
	      refresh();      

	      sleep(32);
	      currentTimeInputBackground =std::chrono::high_resolution_clock::now();
	    }
	}
    }
  return M_LEVEL_COMPLEAT;
}

/*void freeResources(rules & levelRules)
{
  delete(levelRules.gamePlayer);
  }*/
