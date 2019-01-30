#include <ncurses.h>
#include "inputHandlerNonBlock.h++"

char inputHandlerNonBlock(void)
{
  nodelay(stdscr, TRUE); // Don't block while waiting for input (use non blocking sys call).
  int ch {getch()};
  return ch;
  nodelay(stdscr, FALSE);	// Set back to blocking.
}
