#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include <string>
#include "sprite.hpp"
#include "utils.hpp"


class player: public sprite
{
  int health;
  // The player sprite may be subject to gravitational forces.
  double vertVelocity {};

  // This function is not intended for use in player (and thus is private.)
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}

  // These values are used for collision checking in the physics code.
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     NOTE THAT WE INTEND TO HAVE THESE VALUES INITIALLY POPULATED FROM A SPRITE
     FILE IN THE FUTURE (That is the "direct" offsets should be set.)
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

  /* Direct offsets are used when we want to calculate an "overlap" and "one
     off's" are used when we wan't to calculate adjacency. We put the
     aforementioned collision value types in quotes because their names are only
     really true when the direct offsets are all 0. */
  const int topCollisionDirectOffset {0};
  const int topCollisionOneOffOffset {topCollisionDirectOffset -1};
  const int bottomCollisionDirectOffset {0};
  const int bottomCollisionOneOffOffset {bottomCollisionDirectOffset + 1};
  const int rightCollisionDirectOffset {-1};
  const int rightCollisionOneOffOffset {rightCollisionDirectOffset + 1};
  const int leftCollisionDirectOffset {1};
  const int leftCollisionOneOffOffset {leftCollisionDirectOffset -1};

  
public:
  enum directionChars
    { /* Input character to direction mapping.
         '---W---'
	 |...^...|
	 A<--|-->D
	 |...v...|                                            
	 .---S---. */
      UP_CHAR ='w',
      LEFT_CHAR ='a',
      RIGHT_CHAR ='d',
      DOWN_CHAR ='s',
    };

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

private:
    /* For any of the following functions that take an argument position, the
     position should be the current background position. */
    // Returns the the maximum absolute position of the bottom of the sprite.
  // std::string getMaxXAbsLevelRightAsStr(const int position,
  // 					const bool directContact)
  //   const
  // {
  //   std::stringstream ss {};
  //   ss<<(this->position.x + maxBottomRightOffset.x + rightCollisionOffset +
  // 	 position);
  //   return ss.str();
  // }
  // std::string getMaxXAbsLevelLeftAsStr(const int position,
  // 				       const bool directContact)
  //   const
  // {
  //   std::stringstream ss {};
  //   ss<<(this->position.x + leftCollisionOffset + position);
  //   return ss.str();
  // }
  // std::string getMaxYAbsAsStr(const bool directContact)
  //     const
  // {
  //   std::stringstream ss {};
  //   ss<<position.y + maxBottomRightOffset.y + bottomCollisionDirectOffset;//bottomCollisionOffset;
  //   return ss.str();
  // }
  /* Calculates all the points between the absolute position of the bottom
     left + leftCollisionOffset and the absolute position of the bottom right +
     rightCollisionOffset. Return value is a vector of strings of the pos's.
     Posistion should be the absolute x position in the level and directContact
     dictates whether to use bottomCollisionDirectOffset or
     bottomCollisionOneOffOffset */
  std::vector<std::string> getBottomXAbsRangeAsStrs(const int position,
						    const bool directContact)
  {
    const int absLeftPos {this->position.x + leftCollisionDirectOffset + position};
    const int absRightPos {this->position.x + maxBottomRightOffset.x +
      rightCollisionDirectOffset + position};

    std::vector<std::string> retCoords {};
    for(int pos {absLeftPos}; pos <= absRightPos; pos++)
      {
	std::stringstream ss {};
	if(directContact)
	  {
	    ss<<this->position.y + maxBottomRightOffset.y +
	      bottomCollisionDirectOffset;
	  }
	else
	  {
	    ss<<this->position.y + maxBottomRightOffset.y +
	      bottomCollisionOneOffOffset;
	  }
	ss<<','<<pos;
	
	retCoords.push_back(ss.str());
	ss.clear();
      }

    return retCoords;
  }

  
  /*Calculates all the points between the absolute position of the top right +
    topCollisionOffset and the absolute position of the bottom right +
    bottomCollisionOffset. Return value is a vector of strings of the pos's. */
  std::vector<std::string> getYAbsRangeAsStrs(const int position,
					      const bool rightDirection,
					      const bool directContact)
  {
    const int absTopPos {this->position.y + topCollisionDirectOffset};
    const int absBottomPos {this->position.y + maxBottomRightOffset.y +
      bottomCollisionDirectOffset};

    const int collisionOffset {rightDirection ?
      (directContact ? rightCollisionDirectOffset : rightCollisionOneOffOffset) :
      (directContact ? leftCollisionDirectOffset: leftCollisionOneOffOffset)};
    const int x {(rightDirection ?
		  this->maxBottomRightOffset.x : 0) +
    collisionOffset + position + this->position.x};

    
    std::vector<std::string> retCoords {};
    for(int pos {absTopPos}; pos <= absBottomPos; pos++)
      {
	std::stringstream ss {};
	ss<<pos<<','<<x;

	retCoords.push_back(ss.str());
	ss.clear();
      }
    
    return retCoords;
  }
  
  
public:
    std::vector<std::string>
  getBottomXAbsRangeAsStrsForDirectContact(const int position)
  {
    return getBottomXAbsRangeAsStrs(position, true);
  }

  
  std::vector<std::string>
  getBottomXAbsRangeAsStrsForOneOffContact(const int position)
  {
    return getBottomXAbsRangeAsStrs(position, false);
  }

  
  std::vector<std::string>
  getRightYAbsRangeAsStrsForDirectContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, true);
  }

  
    std::vector<std::string>
  getRightYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, 

false);
  }
  
    std::vector<std::string>
  getLeftYAbsRangeAsStrsForDirectContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, false, true);
  }

  
    std::vector<std::string>
  getLeftYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, false, false);
  }
};

#endif
