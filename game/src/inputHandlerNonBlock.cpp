#include <ncurses.h>
#include "include/inputHandlerNonBlock.hpp"

char inputHandlerNonBlock(void)
{/* Fix this! Just add a call to no delay before the main game loop and then
    when entering the menu turn it on and then turn it back off when exiting
    the menue!
  nodelay(stdscr, TRUE); // Don't block while waiting for input (use non blocking sys call).
  int ch {getch()};
  return ch;
  nodelay(stdscr, FALSE);	// Set back to blocking.*/
}
