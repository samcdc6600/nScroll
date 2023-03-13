#ifndef COLOUROFFSET_HPP_
#define COLOUROFFSET_HPP_
#include <map>
#include <string>
#include <vector>


class colourMap//this classes main function is to help in converting the background input from char's to int's
{
private:
  std::map<std::string, int> map;//stores color types to int offsets
  
public:
  const std::vector<std::string> colorPairs
  {//BLACK_WHITE is defined as pair zero and cannot be changed and thus must be first
    //      "WHITE_BLACK",
    "BLACK_BLACK", "BLACK_RED", "BLACK_GREEN", "BLACK_YELLOW", "BLACK_BLUE", "BLACK_MAGENTA", "BLACK_CYAN",
      "BLACK_WHITE", "RED_BLACK", "RED_RED", "RED_GREEN", "RED_YELLOW", "RED_BLUE", "RED_MAGENTA", "RED_CYAN",
      "RED_WHITE", "GREEN_BLACK", "GREEN_RED", "GREEN_GREEN", "GREEN_YELLOW", "GREEN_BLUE", "GREEN_MAGENTA",
      "GREEN_CYAN", "GREEN_WHITE", "YELLOW_BLACK", "YELLOW_RED", "YELLOW_GREEN", "YELLOW_YELLOW", "YELLOW_BLUE",
      "YELLOW_MAGENTA", "YELLOW_CYAN", "YELLOW_WHITE", "BLUE_BLACK", "BLUE_RED", "BLUE_GREEN", "BLUE_YELLOW",
      "BLUE_BLUE", "BLUE_MAGENTA", "BLUE_CYAN", "BLUE_WHITE", "MAGENTA_BLACK", "MAGENTA_RED", "MAGENTA_GREEN",
      "MAGENTA_YELLOW", "MAGENTA_BLUE", "MAGENTA_MAGENTA", "MAGENTA_CYAN", "MAGENTA_WHITE", "CYAN_BLACK",
      "CYAN_RED", "CYAN_GREEN", "CYAN_YELLOW", "CYAN_BLUE", "CYAN_MAGENTA", "CYAN_CYAN", "CYAN_WHITE", "WHITE_RED",
      "WHITE_GREEN", "WHITE_YELLOW", "WHITE_BLUE", "WHITE_MAGENTA", "WHITE_CYAN", "WHITE_WHITE"
      };
  
  colourMap();
  
  int getRange(const std::string str)
  {
    return map.at(str);//return tange (offset)
  }
};

class setColorMode//This class is used to change the color mode of Ncurses
{
private:
  const int colorMax {63};
  const int defaultColor {};
  void setState(const int state);//set the color state in Ncurses
  bool inRange(const int color);//is the color passed to this function in range?
public:
  setColorMode(const int color);
  void setColor(const int color);//set color to color
  void clearColor();//set color to default
};

#endif
