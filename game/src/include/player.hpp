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
  int health;
  // The player sprite may be subject to gravitational forces.
  double vertVelocity {};

  // This function is not intended for use in player (and thus is private.)
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}

  // These values are used for collision checking in the physics code.
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // NOTE THAT WE INTEND TO 
  const int topCollisionOffset {0};
  const int bottomCollisionOffset {1};
  const int rightCollisionOffset {-1};
  const int leftCollissionOffset {1};
  
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
  // /* Returns a yx struct that is a copy of the data member position with it's y
  //    and x member's incremented, decremented or left unchanged depending on the
  //    value of dir. */
  // yx getNewPos(const directionChars dir);

public:
  player(std::vector<std::string> spriteFileNames, const yx max,
	 const yx pos, const sprite::directions dir, const int h,
	 const int v);
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  static directions convertDirectionCharsToDirections(const directionChars dir);
  static bool isDirectionCharInputValid(const int input);
  virtual void updatePosRel(const sprite::directions dir);
  double getVertVelocity();
  void setVertVelocity(const double newVV);
  /* Return the the maximum absolute position of the bottom of the sprite.
     For any of the following functions that take an argument position, the
     position should be the current background position. */
  std::string getMaxXAbsLevelRightAsStr(const int position)
    const
  {
    std::stringstream ss {};
    ss<<(this->position.x + maxBottomRightOffset.x + rightCollisionOffset +
	 position);
    return ss.str();
  }
  std::string getMaxXAbsLevelLeftAsStr(const int position)
    const
  {
    std::stringstream ss {};
    ss<<(this->position.x + leftCollissionOffset + position);
    return ss.str();
  }
  std::string getMaxYAbsAsStr()
    const
  {
    std::stringstream ss {};
    ss<<position.y + maxBottomRightOffset.y + bottomCollisionOffset;
    return ss.str();
  }
  /* Calculates all the points between the absolute position of the bottom
     left + leftCollissionOffset and the absolute position of the bottom right +
     rightCollissionOffset. Return value is a vector of strings of the pos's.*/
  std::vector<std::string> getBottomXAbsRangeAsStrs(const int position)
  {
    const int absLeftPos {this->position.x + leftCollissionOffset + position};
    const int absRightPos {this->position.x + maxBottomRightOffset.x +
      rightCollisionOffset + position};

    std::vector<std::string> retCoords {};
    for(int pos {absLeftPos}; pos <= absRightPos; pos++)
      {
	std::stringstream ss {};
	ss<<this->position.y + maxBottomRightOffset.y + bottomCollisionOffset
	  <<","<<pos;
	
	retCoords.push_back(ss.str());
	ss.clear();
      }

    return retCoords;
  }
};

#endif
