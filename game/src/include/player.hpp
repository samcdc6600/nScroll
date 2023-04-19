#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include "sprite.hpp"
#include "utils.hpp"
#include "background.hpp"


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


  // bool notBetweenWindowPaddingY(const int y, const int yPadding);
  // bool notBetweenWindowPaddingX(const int x, const int xPadding);
  // bool leftOfWindowInnerRightPadding(const int x, const int xBound,
  // 				     const yx viewPortSize);
  // bool rightOfWindowInnerLeftPadding(const int x, const int xBound);

  
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

  player
  (const backgroundData &background, std::vector<std::string> spritePaths,
   // const yx PLAYER_MOVEMENT_AREA_PADDING, const yx initialRelViewPortPos,
   const yx initialPosVPRel, const sprite::directions dir, const int h,
   const double g, const double v, const unsigned maxFallingJmpNum,
   const unsigned maxJmpNum);
  
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  static directions convertDirectionCharsToDirections(const directionChars dir);
  static bool isDirectionCharInputValid(const int input);
  virtual void updatePosRel(const sprite::directions dir);


  
  /* Starts jump (by altering various variables) and moves the player up Z
     characters, where Z is dictated by (int)gravitationalConstant and only as
     long as the player will not hit any boarder characters in coordRules.
     Returns true if the the player started a new jump (this will only happen if
     maxJumpNum hasn't been reached.) If the player didn't start a new jump then
     keepJumping should be called (but only if the player can move down). */
  template<typename T>
  bool startJumping
  (const T * coordRules)
  {
    bool retVar {false};
    if(jumpNum < maxJumpNum)
      {
	jumpNum++;
	vertVelocity = -gravitationalConstant;
	jumping = jumpingUp;
	retVar = true;

	for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	  {
	    for(auto coord: this->getXAbsRangeAsStrs(false, false))
	      {
		char rule {};
		if(getCoordRule(coord, viewPortSize, coordRules, rule) &&
		   rule == boarderRuleChars::BOARDER_CHAR)
		  {
		    // We're going to hit something.
		    goto RETURN;
		  }
	      }
	    // We don't want to hit the top of the level.
	    if(positionVPRel.y == 0)
	      {
		goto RETURN;
	      }
	    // We're not going to hit anything, so jump!
	    updatePosRel(sprite::DIR_UP);
	  }
      }
    else
      {
	/* We must call this here because this function is called (INSTEAD OF HANDLEJUMPINGANDFALLING())  when
	   sprite::DIR_UP is pressed and if we can't perform a new jump we must
	   continue the current jump / fall. */
	handleJumpingAndFalling(coordRules);
      }

  RETURN:
    return retVar;
  }

  
  /* Keeps jumping if the player is jumping. That is as long as the player will
     not collide with any boarder characters or the bottom or top of the level.
     If the player is falling keep falling unless the player is above a boarder
     character or the bottom of the level. If the player isn't above any baorder
     character and isn't at the bottom of the level then start falling. */
  template<typename T>
  void handleJumpingAndFalling
  (const T * coordRules)
  {
    if(jumping == notJumping)
      {
	handleFalling(coordRules);
      }
    else
      {
	handleJumping(coordRules);
      }

    return;			// Required by RETURN label I guess.
  }

  
  bool isJumping() {return jumping != notJumping;};
  
private:
  template<typename T>
  void handleFalling(const T * coordRules)
  {
    using namespace boarderRuleChars;

    if((positionVPRel.y + maxBottomRightOffset.y) == viewPortSize.y)
      {
	// We're at the bottom of the level.
	return;
      }
    else
      {
	for(auto coord: this->getXAbsRangeAsStrs(true, false))
	  {
	    char rule {};
	    if(getCoordRule(coord, viewPortSize, coordRules, rule) &&
	       (rule == BOARDER_CHAR ||
		rule == PLATFORM_CHAR))
	      {
		// There's a rule character below stopping us from falling.
		return;
	      }
	  }
      }
  
    jumpNum = maxJumpNum -maxFallingJumpNum;
    vertVelocity = gravitationalConstant;
    jumping = jumpingDown;

    // We're jumping down.
    handleFallingSimple(coordRules);
  }


  template<typename T>
  void handleFallingSimple(const T * coordRules)
  {
    for(int jumps {(int)-vertVelocity}; jumps > 0; jumps--)
      {
	for(auto coord: this->getXAbsRangeAsStrs(true, false))
	  {
	    char rule {};
	    if(getCoordRule(coord, viewPortSize, coordRules, rule))
	      {
		// We're going to hit something (stop jumping!)
		vertVelocity = 0;
		jumping = notJumping;
		jumpNum = 0;
		return;
	      }
	  }
	/* This is a simpler check but probably much less common, so we put it
	   second. */
	if((positionVPRel.y + maxBottomRightOffset.y) == (viewPortSize.y -1))
	  {
	    // We're going to hit the bottom of the level (stop jumping!)
	    vertVelocity = 0;
	    jumping = notJumping;
	    jumpNum = 0;
	    return;
	  }
	updatePosRel(sprite::DIR_DOWN);
      }

  }
  

  template<typename T>
  void handleJumping
  (const T * coordRules)
  {
    if(jumping == jumpingUp)
      {
	if(vertVelocity <= maxVertVelocity)
	  {
	    vertVelocity -= gravitationalConstant;
	  }
	else
	  {
	    jumping = jumpingDown;
	    vertVelocity += gravitationalConstant;
	  }
      }
    else if(jumping == jumpingDown)
      {
	if(vertVelocity > -maxVertVelocity)
	  {
	    vertVelocity += gravitationalConstant;
	  }
      }

    if(vertVelocity > 0)
      {
	// We are jumping up.
	for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	  {
	    for(auto coord: this->getXAbsRangeAsStrs(false, false))
	      {
		char rule {};
		if(getCoordRule(coord, viewPortSize, coordRules, rule) &&
		   rule == boarderRuleChars::BOARDER_CHAR)
		  {
		    // We're going to hit something.
		    return;
		  }
	      }
	    if(positionVPRel.y == 0)
	      {
		return;
	      }
	    updatePosRel(sprite::DIR_UP);
	  }
      }
    else
      {
	// We're jumping down.
	handleFallingSimple(coordRules);
      }
  }
  

  
  /* Calculates all the points between the absolute position of the left +
     leftCollisionOffset and the absolute position of the right +
     rightCollisionOffset. Return value is a vector of strings of the pos's.
     Position should be the absolute x position in the level and bottomSide
     dictates whether to calculate y from the bottom of the sprite or the top of
     the sprite. DirectContact dictates whether to use
     bottomCollisionDirectOffset or bottomCollisionOneOffOffset */
  std::vector<yx> getXAbsRangeAsStrs(const bool bottomSide,
				     const bool directContact)
  {
    const int absLeftPos
      {this->positionVPRel.x + leftCollisionDirectOffset + positionVPRel.x};
    const int absRightPos {this->positionVPRel.x + maxBottomRightOffset.x +
      rightCollisionDirectOffset + positionVPRel.x};

    const int collisionOffset {bottomSide ?
      (directContact ? bottomCollisionDirectOffset: bottomCollisionOneOffOffset) :
      (directContact ? topCollisionDirectOffset: topCollisionOneOffOffset)};
    const int y
      {(bottomSide ?
	this->maxBottomRightOffset.y: 0) +
       collisionOffset + this->positionVPRel.y};

    std::vector<yx> retCoords {};
    for(int pos {absLeftPos}; pos <= absRightPos; pos++)
      {
	retCoords.push_back(yx(y, pos));
      }

    return retCoords;
  }


  /* Calculates all the points between the absolute position of the top right +
     topCollisionOffset and the absolute position of the bottom right +
     bottomCollisionOffset. Return value is a vector of strings of the pos's. */
  std::vector<yx> getYAbsRangeAsStrs(const int position, const bool rightSide,
				     const bool directContact)
  {
    const int absTopPos {this->positionVPRel.y + topCollisionDirectOffset};
    const int absBottomPos {this->positionVPRel.y + maxBottomRightOffset.y +
      bottomCollisionDirectOffset};

    const int collisionOffset {rightSide ?
      (directContact ? rightCollisionDirectOffset: rightCollisionOneOffOffset):
      (directContact ? leftCollisionDirectOffset: leftCollisionOneOffOffset)};
    const int x {(rightSide ?
		  this->maxBottomRightOffset.x: 0) +
    collisionOffset + position + this->positionVPRel.x};

    
    std::vector<yx> retCoords {};
    for(int pos {absTopPos}; pos <= absBottomPos; pos++)
      {
	retCoords.push_back(yx(pos, x));
      }
    
    return retCoords;
  }
  
  
public:
  
  std::vector<yx>
  getBottomXAbsRangeAsStrsForOneOffContact()
  {
    return getXAbsRangeAsStrs(true, false);
  }


  std::vector<yx>
  getRightYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, false);
  }

  
  std::vector<yx>
  getLeftYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, false, false);
  }

  
  /* Since the player sprite can never go off screen we declare a simpler draw
     function for it here */
  void draw
  (const backgroundData &background, const bool updateSlice);
};

#endif
