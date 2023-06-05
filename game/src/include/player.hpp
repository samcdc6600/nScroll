#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include "sprite.hpp"
// #include "utils.hpp"
// #include "background.hpp"


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
  //  /* Only updates the position in dir if the players relative position will not
  //     move more than 1 character out of the view port. If the player moves 1
  //     character out of the view port. The players position should be reset using
  //     moveIntoViewPort() after the chunk positions have been updated, but before
  //     displaying 2nd stage the draw buffer. */
  virtual void updatePosRel(const sprite::directions dir);
  /* Tests if the top of the player is in (or above) the top padding region or
     if the left of the player is in (or to the left of) the left padding
     region. The former test is performed if yDimension is true and the later is
     performed if yDimension is false. */
  static bool testIntersectionWithPaddingInTopOrLeft
  (const yx playerMovementAreaPadding, const yx playerPosVPRel,
   const yx playerMaxBottomRightOffset, bool yDimension);
  /* Tests if the bottom of the player is in (or below) the bottom padding
     region or if the right of the player is in (or to the right of) the right
     padding region. The former test is performed if yDimension is true and the
     later is performed if yDimension is false. */
  static bool testIntersectionWithPaddingInBottomOrRight
  (const yx playerMovementAreaPadding, const yx playerPosVPRel,
   const yx playerMaxBottomRightOffset, bool yDimension, const yx viewPortSize);
  // /* Test if the player is outisde of the zone inside of the padding in the y
  //    or x dimension */  
  // bool testPaddingInDimension
  // (const yx playerMovementAreaPadding, const yx playerPos,
  //  const yx playerMaxBottomRightOffset, bool yDimension);
  /* Moves the player back into the view port if it has moved out of the view
     port. Move the player in so they are touching the edge they were closest
     to. */
  void moveIntoViewPort(const yx playerMovementAreaPadding);
  /* Potentially moves the player. The player will be moved (or not moved)
     based on the values of coordRules, input and timeElapsed. Where coordRules
     controls where the player can move, input controls the desired direction
     and timeElapsed is used to calculate how much the player should be moved
     relative to the last frame (coordRules and input also factor into this of
     course.) */
  template<typename T>
  void movePlayer
  (const T coordRules, sprite::directions input,
   const double timeElapsed);
private:
  /* Handles collision with boarder characters when the player is moving down.
     Returns updated direction. */
  template<typename T>
  sprite::directions handleGroundCollision(const T coordRules);
  /* Checks for contact with boarder characters when moving right. Moves the
     player up one character if a "step" is encountered. Returns the direction
     the player should move
     in. */
  template<typename T>
  sprite::directions handleRightCollision(const T coordRules);
  template<typename T>
  sprite::directions handleLeftCollision(const T coordRules);
  /* Starts jump (by altering various variables) and moves the player up Z
     characters, where Z is dictated by (int)gravitationalConstant and only as
     long as the player will not hit any boarder characters in coordRules.
     Returns true if the the player started a new jump (this will only happen if
     maxJumpNum hasn't been reached.) If the player didn't start a new jump then
     keepJumping should be called (but only if the player can move down). */
  template<typename T>
  bool startJumping(const T coordRules);  
  /* Keeps jumping if the sprite is jumping. That is as long as the sprite will
   not collide with any boarder characters. If the sprite is falling keeps the
   sprite falling unless the sprite is above a boarder character. If the sprite
   isn't above any boarder character then start falling if the sprite is not
   currently jumping or falling. */
  template<typename T>
  void handleJumpingAndFalling(const T coordRules);

  
  bool isJumping() {return jumping != notJumping;}

  template<typename T>
  void handleFalling(const T coordRules);
  template<typename T>
  void handleFallingSimple(const T coordRules);
  template<typename T>
  void handleJumping(const T coordRules);    
  /* Calculates all the points between the absolute position of the left +
     leftCollisionOffset and the absolute position of the right +
     rightCollisionOffset. Return value is a vector of strings of the pos's.
     Position should be the absolute x position in the level and bottomSide
     dictates whether to calculate y from the bottom of the sprite or the top of
     the sprite. DirectContact dictates whether to use
     bottomCollisionDirectOffset or bottomCollisionOneOffOffset */
  inline std::vector<yx> getXAbsRangeAsStrs(const bool bottomSide,
				     const bool directContact) const;
  inline std::vector<yx> getYAbsRangeAsStrs(const bool rightSide,
				     const bool directContact) const;
  
public:
  inline std::vector<yx> getBottomXAbsRangeAsStrsForOneOffContact() const;
  inline std::vector<yx> getRightYAbsRangeAsStrsForOneOffContact() const;
  inline std::vector<yx> getLeftYAbsRangeAsStrsForOneOffContact() const;

  
  /* Since the player sprite can never go off screen we declare a simpler draw
     function for it here */
  void draw
  (const backgroundData &background, const bool updateSlice);
};


/* NOTE THAT WE HAVE DECIDED TO PUT SOME MEMBER FUNCTIONS HERE IN THE HEADER
   FILE BUT AFTER THE CLASS DEFINITION BECAUSE WE WISH TO MAKE SOME OF THEM
   INLINE AS WELL AS TEMPLATE FUNCTIONS AND AS SUCH THEY CANNOT BE PUT IN THE
   .CPP FILE. WE DO NOT WANT TO CLUTTER THE CLASS DEFINITION HOWEVER. SO WE ARE
   PUTTING THESE FUNCTION DEFINITIONS HERE. */
template<typename T>
void player::movePlayer
(const T coordRules, sprite::directions input,
 const double timeElapsed)
{
  const int currDir {getDirection()};
  // const yx potentialTravel {gamePlayer->getPotentialDistTravelled()};
  yx distTravelled {};
  
  if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
     )
    {
      // // Start jump.
      // gamePlayer->startJumping(coordRules);
      // // We wan't to keep moving in the direction we were moving in before.
      // input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
      handleJumpingAndFalling(coordRules);
    }
  if(currDir == sprite::DIR_NONE && input == sprite::DIR_NONE)
    {
      // slow down sprite until it stops.. or something like this. All this
      // code needs to be cleaned up and refined since it's very ugly right now!
    }

  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(coordRules);
      if(input == sprite::DIR_NONE)
	{
	  // velComp.setVlctY(0);
	}
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      input = handleRightCollision(coordRules);
      if(input == sprite::DIR_RIGHT)
	{
	  distTravelled = velComp.getAndSetDistTravelled();
	  velComp.addToXComp(0.1);
	}
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      input = handleLeftCollision(coordRules);
      if(input == sprite::DIR_LEFT)
	{
	  distTravelled = velComp.getAndSetDistTravelled();
	  velComp.addToXComp(-0.1);
	}
    }

  // std::cout<<"distTravelled = "<<distTravelled<<'\n';

  positionVPRel += distTravelled;
  updateDirection(input);
}


/* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
template<typename T>
sprite::directions player::handleGroundCollision(const T coordRules)
{
  sprite::directions retDir {sprite::DIR_DOWN};
  for(yx coord:
	getBottomXAbsRangeAsStrsForOneOffContact())
    {
      char rule {};
      if(getCoordRule(coord, viewPortSize, coordRules, rule) &&
	 (rule == boarderRuleChars::BOARDER_CHAR ||
	  rule == boarderRuleChars::PLATFORM_CHAR))
	{
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
template<typename T>
sprite::directions player::handleRightCollision(const T coordRules)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_RIGHT};
  const std::vector<yx> playerCoords
    {getRightYAbsRangeAsStrsForOneOffContact()};
  const yx bottomRightPlayerCoord
    {playerCoords[playerCoords.size() -1]};
  bool stoppingContact {false};

  char rule {};
  for(yx playerCoord: playerCoords)
    {
      // If there is near contact and it's not with the bottom right coord.
      if(playerCoord != bottomRightPlayerCoord &&
	 getCoordRule(playerCoord, viewPortSize, coordRules, rule) &&
	 rule == BOARDER_CHAR)
	{
	  stoppingContact = true;
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  
  if(!stoppingContact && getCoordRule
     (bottomRightPlayerCoord, viewPortSize, coordRules, rule) &&
     (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
    {
	  /* If we've hit a "step" (as in the things that constitute
	     staircases), then "step up" :). */
	  updatePosRel(sprite::DIR_UP);
    }
  
  return retDir;
}


template<typename T>
sprite::directions player::handleLeftCollision(const T coordRules)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_LEFT};
  const std::vector<yx> playerCoords
    {getLeftYAbsRangeAsStrsForOneOffContact()};
  const yx bottomLeftPlayerCoord
    {playerCoords[playerCoords.size() -1]};
  bool stoppingContact {false};

  char rule {};
  for(yx playerCoord: playerCoords)
    {
      // If there is near contact and it's not with the bottom right coord.
      if(playerCoord != bottomLeftPlayerCoord &&
	 getCoordRule(playerCoord, viewPortSize, coordRules, rule) &&
	 rule == BOARDER_CHAR)
	{
	  stoppingContact = true;
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }

  if(!stoppingContact && getCoordRule
     (bottomLeftPlayerCoord, viewPortSize, coordRules, rule) &&
     (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
    {
      updatePosRel(sprite::DIR_UP);
    }

  return retDir;
}


template<typename T>
bool player::startJumping(const T coordRules)
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


template<typename T>
void player::handleJumpingAndFalling
(const T coordRules)
{
  if(jumping == notJumping)
    {
      handleFalling(coordRules);
    }
  else
    {
      handleJumping(coordRules);
    }
}


template<typename T>
void player::handleFalling(const T coordRules)
{
  using namespace boarderRuleChars;

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
  
  jumpNum = maxJumpNum -maxFallingJumpNum;
  vertVelocity = gravitationalConstant;
  jumping = jumpingDown;

  // We're jumping down.
  handleFallingSimple(coordRules);
}


template<typename T>
void player::handleFallingSimple(const T coordRules)
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
void player::handleJumping(const T coordRules)
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


std::vector<yx> player::getXAbsRangeAsStrs(const bool bottomSide,
					   const bool directContact) const
{
  const int absLeftPos
    {this->positionVPRel.x + leftCollisionDirectOffset};
  const int absRightPos {this->positionVPRel.x + maxBottomRightOffset.x +
			 rightCollisionDirectOffset};

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


std::vector<yx> player::getYAbsRangeAsStrs(const bool rightSide,
					   const bool directContact) const
{
  const int absTopPos {this->positionVPRel.y + topCollisionDirectOffset};
  const int absBottomPos {this->positionVPRel.y + maxBottomRightOffset.y +
			  bottomCollisionDirectOffset};

  const int collisionOffset {rightSide ?
			     (directContact ? rightCollisionDirectOffset: rightCollisionOneOffOffset):
			     (directContact ? leftCollisionDirectOffset: leftCollisionOneOffOffset)};
  const int x {(rightSide ?
		this->maxBottomRightOffset.x: 0) +
	       collisionOffset + positionVPRel.x};

    
  std::vector<yx> retCoords {};
  for(int pos {absTopPos}; pos <= absBottomPos; pos++)
    {
      retCoords.push_back(yx(pos, x));
    }
    
  return retCoords;
}


inline std::vector<yx> player::getBottomXAbsRangeAsStrsForOneOffContact() const
{
  return getXAbsRangeAsStrs(true, false);
}


inline std::vector<yx> player::getRightYAbsRangeAsStrsForOneOffContact() const
{
  return getYAbsRangeAsStrs(true, false);
}

  
inline std::vector<yx> player::getLeftYAbsRangeAsStrsForOneOffContact() const
{
  return getYAbsRangeAsStrs(false, false);
}


#endif
