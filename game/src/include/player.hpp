#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
// tmp vvv
#include <sstream>
#include "common.hpp"
// tmp ^^^
#include <string>
#include "sprite.hpp"
#include "common.hpp"


class player: public sprite
{
  static constexpr int NUM_PLAYER_SPRITES {4};
  static constexpr int INITIAL_SPRITE_INDEX {0};
  static constexpr int ADDITIONAL_SPRITES_OFFSET {1};
  spriteData sD_player [NUM_PLAYER_SPRITES];
  int health;

  // This function is not intended for use in player (and thus is private.)
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}
  
public:
  enum directionChars
    { /* Input character to direction mapping.
         Q---W---E
	 |...^...|
	 A<--|-->D
	 |...v...|                                            
	 z---S---X */
      LEFT_UP_CHAR ='q',
      LEFT_UP_UPPER_CHAR ='Q',
      UP_CHAR ='w',
      UP_UPPER_CHAR ='W',
      RIGHT_UP_CHAR ='e',
      RIGHT_UP_UPPER_CHAR ='E',
      LEFT_CHAR ='a',
      LEFT_UPPER_CHAR ='A',
      RIGHT_CHAR ='d',
      RIGHT_UPPER_CHAR ='D',
      LEFT_DOWN_CHAR ='z',
      LEFT_DOWN_UPPER_CHAR ='Z',
      DOWN_CHAR ='s',
      DOWN_UPPER_CHAR ='S',
      RIGHT_DOWN_CHAR ='x',
      RIGHT_DOWN_UPPER_CHAR ='X'
    };

private:
  /* Returns a yx struct that is a copy of the data member position with it's y
     and x member's incremented, decremented or left unchanged depending on the
     value of dir. */
  yx getNewPos(const directionChars dir);

public:
  player(std::vector<std::string> sprites, const yx max, const yx pos,
	 const directionChars dir, const int h);
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  directions convertDirectionCharsToDirections(const directionChars dir);
  yx peekAtPos(const directionChars dir);
  virtual void updatePosRel(directionChars dir);
  virtual void draw(bool updateSlice);  
};

#endif
