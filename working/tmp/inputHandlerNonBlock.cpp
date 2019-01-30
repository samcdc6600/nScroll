#include "inputHandlerNonBlock.h"
#include <ncurses.h>

char inputHandlerNonBlock(void)
{
  nodelay(stdscr, TRUE); // Don't block while waiting for input (use non blocking sys call).
  int ch {getch()};
  return ch;
  nodelay(stdscr, FALSE);	// Set back to blocking.
}
