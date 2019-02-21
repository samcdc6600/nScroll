#ifndef INITCURSES_H_
#define INITCURSES_H_

#include <string>
#include <ncurses.h>
#include <sstream>
#include "../common.h++"


constexpr int yHeight {33}, xWidth{125};        // The window must be these dimensions                                 


void initialiseCurses(yx & maxyx);
void initColorPairs();


#endif
