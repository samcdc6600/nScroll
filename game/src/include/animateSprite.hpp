#ifndef ANIMATE_SPRITE_HPP_
#define ANIMATE_SPRITE_HPP_
#include "sprite.hpp"


/* All "animate" sprite classes should inherit from this class. E.g., player. */
class animateSprite: public sprite
{
 public:
  class velocity
  {
  public:
    /* The sprites position should be updated by 1 character in any direction at
       most this often. This is in seconds. */
    static constexpr double spriteMovementUpdatingTime {1 / 100.0};
    struct velocityComps
    {
      double y {};
      double x {};

      
      velocityComps() {}


      velocityComps(const double y, const double x)
      {
	this->y = y, this->x = x;
      }
      
      
      velocityComps(yx comps)
      {
	y = comps.y, x = comps.x;
      }
    };
    
  private:
    chronological lastYUpdate {}, lastXUpdate {};
    /* We obtained these numbers by measuring the width and height of a
       character in our terminal emulator. */
    static constexpr double yVelocitySailFactor {14 / 28};
    /* Used to store distance travelled across calls to functions such as
       getAndSetDistTravelled(). Note that this is not the total distance
       travelled as member functions may alter distTravelled. For example
       getAndSetDistTravelled() will subtract 1 from any component of
       distTravelled that is calculated to be more than 1 when it is called. */
    velocityComps distTravelled {};
    velocityComps comps {};
    
  public:
    /* The sprites x and y velocity components should not exceed this value at
       any point (the sum of the x and y velocity components can be greater. */
    const double maxVelocity {36};


    void startTimers()
    {
      
      lastYUpdate.start();
      lastXUpdate.start();
    }
    
    // double getY() const {return comps.y * yVelocitySailFactor;}
    // double getX() const {return comps.x;}


    /* This function should be used when the sprite needs to be compleatly
       stopposed. For example when the sprite hits a boarder character. */
    void setComponentsToZero();
    /* Used when the sprite is told to stop, but still has momentum. Where a
       will be added or subtracted from the x velocity component until the
       component changes sign. When this happens it will be set to zero. */
    void velocityTowardsZeroInX(const double a);
    void addToYComp(const double a);
    void addToXComp(const double a);
    // void setVlctY(const double newY);
    // void setVlctX(const double newX);
    // void scailY(const double scailFactor);
    // void scailX(const double scailFactor);
    /* Returns 1 or 0. Returns 1 if the distance travelled is more than
       1. Otherwise returns 0. If 0 is returned the distance travelled is
       remembered and added to the next distance. If 1 is returned, 1 is
       subtracted for the distance and any remainder is added to the distance
       travelled. This function assumes that the distance travelled can never
       be greater than 2. */
    yx getAndSetDistTravelled();
    /* This function is the same as the above but with the exception that it
       does  not change the state of this class. */
    yx getPotentialDistTravelled() const;
  };
  
  velocity velComp;
  const double gravitationalConstant;
  // /* Negated for negative velocities (1 = 1 character per frame.). */
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


  animateSprite(std::vector<std::string> & spritePaths, const yx max,
		const yx pos, const directions dir, const double g,
		const double v, const unsigned maxFallingJmpNum,
		const unsigned maxJmpNum):
    sprite(spritePaths, max, pos, dir),
    gravitationalConstant(g), maxVertVelocity(v),
    maxFallingJumpNum(maxFallingJmpNum), maxJumpNum(maxJmpNum)
  {}


protected:
  //  /* Only updates the position in dir if the players relative position will not
  //     move more than 1 character out of the view port. If the player moves 1
  //     character out of the view port. The players position should be reset using
  //     moveIntoViewPort() after the chunk positions have been updated, but before
  //     displaying 2nd stage the draw buffer. */
  /* Updates the position of the player relative to the current position based
     on the value of dir. Also performs actions related to change of direction
     such as updating the sprite. */
  virtual void updatePosRel(const sprite::directions dir);
    /* Used when the player stops moving horazontally (there may still be
     momentum (i.e. a non zero x velocity and distance). This and any
     collisions that may result from it while the player is slowing down needs
     to be handled. */
  template<typename T>
  yx handleHorizontalMovementsWhenStopping(const T coordRules);
  /* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
  template<typename T>
  sprite::directions handleGroundCollision(const T coordRules);
  /* Checks if the player can move left or right. If they can set's input to
     DIR_RIGHT or DIR_LEFT (depending on which direction is checked) and
     returns the amount the player moved by (if they moved more than the minimum
     movement threshold (see the class velocity)). If the player can't move in
     the direction that is being checked the player's velocity components (and
     distance components) are set to 0. Where checkRight controls which movement
     direction is checked. */
  template<typename T>
  yx handleLeftOrRightCollision
  (const T coordRules, sprite::directions & input, const bool checkRight);
  /* Checks for contact with boarder characters when moving right. Moves the
     player up one character if a "step" is encountered. Returns the direction
     the player should move
     in. */
  template<typename T>
  sprite::directions handleRightCollision(const T coordRules);
  template<typename T>
  sprite::directions handleLeftCollision(const T coordRules);
  /* If the player is moving left and turns right right before they would be
   stopped by a boarder character they will still have leftward velocity and
   direction components. We must check for this and set these components to
   zero in the event that this would happen. This function checks for this
   case. It returns true if the components should be set to zero. */
  template <typename T>
  bool checkForLeftCollisionWhenInputRight(const T coordRules);
  template <typename T>
  bool checkForRightCollisionWhenInputLeft(const T coordRules);
  /* There is at least one special case in which we need to look ahead 2
     characters to make sure an edge case where the player can move into a
     boarder character region does not happen. This function is used to detect
     when this edge case could occur. */
  template <typename T>
  bool checkForRightCollisionInAdvance(const T coordRules);
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
  inline std::vector<yx> getBottomXAbsRangeAsStrsForOneOffContact() const;
  inline std::vector<yx> getRightYAbsRangeAsStrsForOneOffContact() const;
  inline std::vector<yx> getLeftYAbsRangeAsStrsForOneOffContact() const;
};


template<typename T>
yx animateSprite::handleHorizontalMovementsWhenStopping(const T coordRules)
{
  yx distTravelled {};
  animateSprite::velocity::velocityComps potentialDist
    {velComp.getPotentialDistTravelled()};
      
  if(potentialDist.x > 0)
    {
      const sprite::directions input = handleRightCollision(coordRules);
      if(input == sprite::DIR_RIGHT)
	{
	  distTravelled = velComp.getAndSetDistTravelled();
	  velComp.velocityTowardsZeroInX(8.5);
	}
      else
	{
	  /* We were blocked by something. Reset velocity and distance
	     components */
	  velComp.setComponentsToZero();
	}
    }
  else if(potentialDist.x < 0)
    {
      const sprite::directions input = handleLeftCollision(coordRules);
      if(input == sprite::DIR_LEFT)
	{
	  distTravelled = velComp.getAndSetDistTravelled();
	  velComp.velocityTowardsZeroInX(8.5);
	}
      else
	{
	  /* We were blocked by something. Reset velocity and distance
	     components */
	  velComp.setComponentsToZero();
	}
    }

  return distTravelled;
}


template<typename T>
sprite::directions animateSprite::handleGroundCollision(const T coordRules)
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


template<typename T>
yx animateSprite::handleLeftOrRightCollision
(const T coordRules, sprite::directions & input, const bool checkRight)
{
  yx distTravelled {};

  input = checkRight ?
    handleRightCollision(coordRules): handleLeftCollision(coordRules);
  if(input == (checkRight ? sprite::DIR_RIGHT: sprite::DIR_LEFT))
    {
      if(checkRight ? checkForLeftCollisionWhenInputRight(coordRules):
	 checkForRightCollisionWhenInputLeft(coordRules))
	{
	  /* Detected that sprite would move into a boarder character in the
	     opposite direction of the desired moment. */
	  velComp.setComponentsToZero();
	}
      distTravelled = velComp.getAndSetDistTravelled();
      velComp.addToXComp(checkRight ? 2.9: - 2.9);
    }
  else
    {
      /* We were blocked by something. Reset velocity and distance
	 components */
      velComp.setComponentsToZero();
    }

  return distTravelled;
}


template<typename T>
sprite::directions animateSprite::handleRightCollision(const T coordRules)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_RIGHT};

  // Only check for collision if we will be moving right.
  if(velComp.getPotentialDistTravelled().x >= 1)
    {
      const std::vector<yx> spriteCoords
	{getRightYAbsRangeAsStrsForOneOffContact()};
      const yx bottomRightSpriteCoord
	{spriteCoords[spriteCoords.size() -1]};
      bool stoppingContact {false};

      char rule {};
      for(yx spriteCoord: spriteCoords)
	{
	  // If there is near contact and it's not with the bottom right coord.
	  if(spriteCoord != bottomRightSpriteCoord &&
	     getCoordRule(spriteCoord, viewPortSize, coordRules, rule) &&
	     rule == BOARDER_CHAR)
	    {
	      stoppingContact = true;
	      retDir = sprite::DIR_NONE;
	      break;
	    }
	}
  
      if(!stoppingContact && getCoordRule
	 (bottomRightSpriteCoord, viewPortSize, coordRules, rule) &&
	 (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
	{
	  /* If we've hit a "step" (as in the things that constitute
	     staircases), then "step up" :). */
	  updatePosRel(sprite::DIR_UP);
	}
    }
  
  return retDir;
}


template<typename T>
sprite::directions animateSprite::handleLeftCollision(const T coordRules)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_LEFT};

  // Only check for collision if we will be moving left.
  if(velComp.getPotentialDistTravelled().x <= -1)
    {
      const std::vector<yx> spriteCoords
	{getLeftYAbsRangeAsStrsForOneOffContact()};
      const yx bottomLeftSpriteCoord
	{spriteCoords[spriteCoords.size() -1]};
      bool stoppingContact {false};

      char rule {};
      for(yx spriteCoord: spriteCoords)
	{
	  // If there is near contact and it's not with the bottom right coord.
	  if(spriteCoord != bottomLeftSpriteCoord &&
	     getCoordRule(spriteCoord, viewPortSize, coordRules, rule) &&
	     rule == BOARDER_CHAR)
	    {
	      stoppingContact = true;
	      retDir = sprite::DIR_NONE;
	      break;
	    }
	}

      if(!stoppingContact && getCoordRule
	 (bottomLeftSpriteCoord, viewPortSize, coordRules, rule) &&
	 (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
	{
	  updatePosRel(sprite::DIR_UP);
	}
    }

  return retDir;
}


template <typename T>
bool animateSprite::checkForLeftCollisionWhenInputRight(const T coordRules)
{
  using namespace boarderRuleChars;
  bool ret {false};		// Return false if no collision detected.

  // Only check if there can be a leftward collision when moving right.
  if(velComp.getPotentialDistTravelled().x <= -1)
    {
      const std::vector<yx> spriteCoords
	{getLeftYAbsRangeAsStrsForOneOffContact()};
      const yx bottomLeftSpriteCoord
	{spriteCoords[spriteCoords.size() -1]};

      char rule {};
      for(yx spriteCoord: spriteCoords)
	{
	  // If there is near contact and it's not with the bottom left coord.
	  if(spriteCoord != bottomLeftSpriteCoord &&
	     getCoordRule(spriteCoord, viewPortSize, coordRules, rule) &&
	     rule == BOARDER_CHAR)
	    {
	      ret = true;
	      break;
	    }
	}
    }

  return ret;
}


template <typename T>
bool animateSprite::checkForRightCollisionWhenInputLeft(const T coordRules)
{
  using namespace boarderRuleChars;
  bool ret {false};		// Return false if no collision detected.

  // Only check if there can be a rightward collision when moving left.
  if(velComp.getPotentialDistTravelled().x >= 1)
    {
      const std::vector<yx> spriteCoords
	{getRightYAbsRangeAsStrsForOneOffContact()};
      const yx bottomRightSpriteCoord
	{spriteCoords[spriteCoords.size() -1]};

      char rule {};
      for(yx spriteCoord: spriteCoords)
	{
	  // If there is near contact and it's not with the bottom right coord.
	  if(spriteCoord != bottomRightSpriteCoord &&
	     getCoordRule(spriteCoord, viewPortSize, coordRules, rule) &&
	     rule == BOARDER_CHAR)
	    {
	      ret = true;
	      break;
	    }
	}
    }

  return ret;
}


template<typename T>
bool animateSprite::startJumping(const T coordRules)
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
  
  
 //  bool retVar {false};
 //  if(jumpNum < maxJumpNum)
 //    {
 //      jumpNum++;
 //      vertVelocity = -gravitationalConstant;
 //      jumping = jumpingUp;
 //      retVar = true;

 //      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
 // 	{
 // 	  for(auto coord: this->getXAbsRangeAsStrs(false, false))
 // 	    {
 // 	      char rule {};
 // 	      if(getCoordRule(coord, viewPortSize, coordRules, rule) &&
 // 		 rule == boarderRuleChars::BOARDER_CHAR)
 // 		{
 // 		  // We're going to hit something.
 // 		  goto RETURN;
 // 		}
 // 	    }
 // 	  // We don't want to hit the top of the level.
 // 	  if(positionVPRel.y == 0)
 // 	    {
 // 	      goto RETURN;
 // 	    }
 // 	  // We're not going to hit anything, so jump!
 // 	  updatePosRel(sprite::DIR_UP);
 // 	}
 //    }
 //  else
 //    {
 //      /* We must call this here because this function is called (INSTEAD OF HANDLEJUMPINGANDFALLING())  when
 // 	 sprite::DIR_UP is pressed and if we can't perform a new jump we must
 // 	 continue the current jump / fall. */
 //      handleJumpingAndFalling(coordRules);
 //    }

 // RETURN:
 //  return retVar;
}


template<typename T>
void animateSprite::handleJumpingAndFalling
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
void animateSprite::handleFalling(const T coordRules)
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
void animateSprite::handleFallingSimple(const T coordRules)
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
void animateSprite::handleJumping(const T coordRules)
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


std::vector<yx> animateSprite::getXAbsRangeAsStrs(const bool bottomSide,
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


std::vector<yx> animateSprite::getYAbsRangeAsStrs(const bool rightSide,
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


inline std::vector<yx> animateSprite::getBottomXAbsRangeAsStrsForOneOffContact() const
{
  return getXAbsRangeAsStrs(true, false);
}


inline std::vector<yx> animateSprite::getRightYAbsRangeAsStrsForOneOffContact() const
{
  return getYAbsRangeAsStrs(true, false);
}

  
inline std::vector<yx> animateSprite::getLeftYAbsRangeAsStrsForOneOffContact() const
{
  return getYAbsRangeAsStrs(false, false);
}


#endif
