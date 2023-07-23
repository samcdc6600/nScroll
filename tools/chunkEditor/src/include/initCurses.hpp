#ifndef INITCURSES_HPP_
#define INITCURSES_HPP_


#include <string>
#include <ncurses.h>
#include <sstream>
#include "utils.hpp"
#include "colorS.hpp"


std::vector<int> initialiseCurses(yx & maxyx);
std::vector<int> initColorPairs();


#endif
