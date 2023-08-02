#ifndef INITCURSES_HPP_
#define INITCURSES_HPP_


#include <string>
#include <ncurses.h>
#include <sstream>
#include "utils.hpp"
#include "colorS.hpp"


void initialiseCurses(yx & maxyx);
void initColorPairs();


#endif
