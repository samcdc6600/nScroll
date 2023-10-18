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
  /* Number of color pairs used by the game. Ncurses must support at least this
     many color pairs. COLOR_PAIRS is 32767 on our system. We will support 125^2
     (15625) color pairs. That is 125 bg colors and 125 fg colors. */
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
  /* Code for value of transparent space is 1 higher then
     159 * effectiveGameColorPairsNo which is above the space of all ACS and
     ASCII characters whether coloured or not. */
  constexpr int TRANS_SP {159 * effectiveGameColorPairsNo};
  // Space character encoded with a color pair that has a black background.
  constexpr int blackSpaceChar {158 * 14 + ' '};
}


// This class is used to change the color mode of Ncurses.
class setColorMode
{
private:
  void setState(const int state); // Set the color state in Ncurses.
  bool inRange(const int color);
  void setColor(const int color); // Set color to color.
  
  // Stupendous.
public:
  int getColor(const int ch);	 /* Returns colour code encoded in ch. */
  void setColorFromChar(const int ch);
  void setRandomColor();
};


#endif
