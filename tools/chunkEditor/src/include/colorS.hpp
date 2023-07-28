#ifndef COLOUROFFSET_HPP_
#define COLOUROFFSET_HPP_


#include <map>
#include <string>
#include <vector>


namespace colorParams
{
  /* The game has supports this many colors and as such the terminal must also
     support this many colors. */
  constexpr int gameColors {256};
  /* Number of color pairs used by the game. Ncurses must support at least this
     many color pairs. COLOR_PAIRS is 32767 on our system. We will support 125^2
     (15625) color pairs. That is 125 bg colors and 125 fg colors. */
  // Each game color component (rgb) has 5 levels.
  constexpr int gameColorCompNo {5};
  // No. of colors (not color pairs.)
  // constexpr int gameColors {125};
  constexpr int gameColorPairsNo {15625}; //gameColors * gameColors}; // 15625
  /* The RGB values should be in the range [0, 1000]. */
  constexpr int gameRGBMax {1000};
  /* This color pair is used when a character is not yet in the position in
     question. */
  constexpr int noCharColorPair {1};
}


class setColorMode//This class is used to change the color mode of Ncurses
{
private:
  void setState(const int state);//set the color state in Ncurses
  // void setDefaultColor();
  bool inRange(const int color);//is the color passed to this function in range?
  
public:
  void setColor(const int color);//set color to color
  void clearColor();//set color to default
};

#endif
