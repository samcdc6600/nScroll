#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include "sprite.hpp"
#include "utils.hpp"


class player: public sprite
{
  int health;
  const double gravitationalConstant;
  /* Negated for negative velocities (1 = 1 character per frame.). */
  const double maxVertVelocity;	// Should be a minimum of 1.
  double vertVelocity;	// Used for jumping.
  enum jumpingStates
    {
      notJumping,
      jumpingUp,
      jumpingDown		// AKA falling.
    };
  // Should hold only take on values from jumpingStates.
  int jumping {notJumping};
  /* Used to count current jump number (e.g., triple jump.) Where 0 is not
     jumping. */
  unsigned jumpNum {};
  // If the player starts falling (when not jumping) how many jumps can they do?
  const unsigned maxFallingJumpNum;
  const unsigned maxJumpNum;

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
               const double g, const double v, const int maxFallingJmpNum,
	 const int maxJmpNum);
  
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  static directions convertDirectionCharsToDirections(const directionChars dir);
  static bool isDirectionCharInputValid(const int input);
  virtual void updatePosRel(const sprite::directions dir);
  double getVertVelocity();
  void setVertVelocity(const double newVV);
  /* Starts jump (by altering various variables) and moves the player up X
     characters, where position is the absolute x position of the player and X
     is dictated by (int)gravitationalConstant and only as long as the player
     will not hit any boarder characters in coordChars or the top of the level.
     Returns true if the the player started a new jump (this will only happen if
     maxJumpNum hasn't been reached.) If the player didn't start a new jump then
     keepJumping should be called (but only if the player can move down). */
  bool startJumping(const int position,
		    const std::map<std::string, char> & coordChars);
  /* Keeps jumping if the player is jumping. That is as long as the player will
     not collide with any boarder characters or the bottom or top of the level.
     If the player is falling keep falling unless the player is above a boarder
     character or the bottom of the level. If the player isn't above any baorder
     character and isn't at the bottom of the level then start falling. */
  void handleJumpingAndFalling(const int bgPosition, const yx & maxyx,
		   const std::map<std::string, char> & coordChars);
  bool isJumping() {return jumping != notJumping;};
  
private:
  void handleFalling(const int bgPosition, const yx & maxyx,
		     const std::map<std::string, char> & coordChars);
  void handleJumping(const int bgPosition, const yx & maxyx,
		     const std::map<std::string, char> & coordChars);
  /* Calculates all the points between the absolute position of the left +
     leftCollisionOffset and the absolute position of the right +
     rightCollisionOffset. Return value is a vector of strings of the pos's.
     Position should be the absolute x position in the level and bottomSide
     dictates whether to calculate y from the bottom of the sprite or the top of
     the sprite directContact dictates whether to use
     bottomCollisionDirectOffset or bottomCollisionOneOffOffset */
  std::vector<std::string> getXAbsRangeAsStrs(const int position,
					      const int bottomSide,
					      const bool directContact)
  {
    const int absLeftPos {this->position.x + leftCollisionDirectOffset + position};
    const int absRightPos {this->position.x + maxBottomRightOffset.x +
      rightCollisionDirectOffset + position};

    const int collisionOffset {bottomSide ?
      (directContact ? bottomCollisionDirectOffset: bottomCollisionOneOffOffset) :
      (directContact ? topCollisionDirectOffset: topCollisionOneOffOffset)};
    const int y {(bottomSide ? this->maxBottomRightOffset.y: 0) + collisionOffset + this->position.y};

    std::vector<std::string> retCoords {};
    for(int pos {absLeftPos}; pos <= absRightPos; pos++)
      {
	std::stringstream ss {};
	ss<<y<<','<<pos;
	
	retCoords.push_back(ss.str());
	ss.clear();
      }

    return retCoords;
  }


  /* Calculates all the points between the absolute position of the top right +
     topCollisionOffset and the absolute position of the bottom right +
     bottomCollisionOffset. Return value is a vector of strings of the pos's. */
  std::vector<std::string> getYAbsRangeAsStrs(const int position,
					      const bool rightSide,
					      const bool directContact)
  {
    const int absTopPos {this->position.y + topCollisionDirectOffset};
    const int absBottomPos {this->position.y + maxBottomRightOffset.y +
      bottomCollisionDirectOffset};

    const int collisionOffset {rightSide ?
      (directContact ? rightCollisionDirectOffset: rightCollisionOneOffOffset):
      (directContact ? leftCollisionDirectOffset: leftCollisionOneOffOffset)};
    const int x {(rightSide ?
		  this->maxBottomRightOffset.x: 0) +
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
    return getXAbsRangeAsStrs(position, true, true);
  }

  
  std::vector<std::string>
  getBottomXAbsRangeAsStrsForOneOffContact(const int position)
  {
    return getXAbsRangeAsStrs(position, true, false);
  }


  std::vector<std::string>
  getTopXAbsRangeAsStrsForDirectContact(const int position)
  {
    return getXAbsRangeAsStrs(position, false, true);
  }


    std::vector<std::string>
  getTopXAbsRangeAsStrsForOneOffContact(const int position)
  {
    return getXAbsRangeAsStrs(position, false, false);
  }

  
  std::vector<std::string>
  getRightYAbsRangeAsStrsForDirectContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, true);
  }


  std::vector<std::string>
  getRightYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, false);
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

  std::string
  getOneBelowBottomRight(const int position)
  {
    const int absLeftPos {this->position.x + leftCollisionDirectOffset + position};
    const int absRightPos {this->position.x + maxBottomRightOffset.x +
      rightCollisionDirectOffset + position};
  }

  std::string
  getOneBelowBottomLeft(const int position)
  {
  }
};

#endif
