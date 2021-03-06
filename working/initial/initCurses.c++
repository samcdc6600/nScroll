#include "initCurses.h++"


void initialiseCurses(yx & maxyx)
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
  raw();                     // To disable line buffering                                                              
  curs_set(0);               // Make the cursor invisible                                                              
  noecho();                  // Turn echoing off on the terminal                                                       
  start_color();             // Start color and initialise color pairs!                                                
  nodelay(stdscr, TRUE);     // Dissable blocking while waiting for input (use non blocking sys call.)                 
  initColorPairs();          // Initialise the color pairs                                                             
}


void initColorPairs()
{
  attron(COLOR_PAIR(56));       //default color. Must also be changed in draw.cpp!!!!!!!!!!!!!!!!!!!!!!!!              
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
