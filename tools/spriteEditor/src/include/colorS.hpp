#ifndef COLOUROFFSET_HPP_
#define COLOUROFFSET_HPP_


#include <map>
#include <string>
#include <vector>


namespace colorParams
{  
  constexpr int gameColorCompNo {3};
  /* The game supports this many colors and as such the terminal must also
     support this many colors. */
  constexpr int gameColors
    {gameColorCompNo * gameColorCompNo * (gameColorCompNo -1) + 5};
  /* Ncurses supports 256 color pairs. We will use
     (setq colors(+ (* 3 3 2) 5)) = 23
     (-(/ (expt colors 2)2)(/ colors 2)) = 253,
     that is 253 color pairs. The above equation is the number of color pairs
     once pairs that could be obtained through inversion and pairs with both
     colors being equal (which can just be drawn using spaces) are removed.
     then use the function calls attron(A_REVERSE) and  attroff(A_REVERSE) to
     effectively double that number and regain the pairs that were removed. */
  constexpr int gameColorPairsNoMax {256};
  constexpr int gameColorPairsNo {253};
  constexpr int effectiveGameColorPairsNo {gameColorPairsNo * 2};
  /* The RGB values should be in the range [0, 1000]. */
  constexpr int gameRGBMax {1000};
}


// This class is used to change the color mode of Ncurses.
class setColorMode
{
private:
  void setState(const int state); // Set the color state in Ncurses.
  bool inRange(const int color);
  
  // Stupendous.
public:
  void setColor(const int color); // Set color to color.
};


#endif
