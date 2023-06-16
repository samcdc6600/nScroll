#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include "animateSprite.hpp"


class player: public animateSprite
{
  // int health;
  // These values are used for collision checking in the physics code.
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     NOTE THAT WE INTEND TO HAVE THESE VALUES INITIALLY POPULATED FROM A SPRITE
     FILE IN THE FUTURE (That is the "direct" offsets should be set.)
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


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
   const yx initialPosVPRel, const sprite::directions dir, const int health,
   const double g, const unsigned maxFallingJmpNum,
   const unsigned maxJmpNum);
  
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  static directions convertDirectionCharsToDirections(const directionChars dir);
  static bool isDirectionCharInputValid(const int input);
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
     based on the values of coordRules and input (as well as an internal "time
     elapsed" member variable of a parent class.) Where coordRules controls
     where the player can move and input controls the desired
     direction. */
  template<typename T>
  void movePlayer
  (const T coordRules, sprite::directions input);
  
public:
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
(const T coordRules, sprite::directions input)
{
  const int currDir {getDirection()};
  // const yx potentialTravel {gamePlayer->getPotentialDistTravelled()};
  yx distTravelled {};
  
  if(input == sprite::DIR_UP)
    {
      distTravelled.y = startJumping(coordRules);
      // We wan't to keep moving in the direction we were moving in before.
      input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
       distTravelled.y = handleJumpingAndFalling(coordRules);
    }

  if(currDir == sprite::DIR_NONE && input == sprite::DIR_NONE)
    {
      /* If we still have left / right momentum we need to check for horizontal
	 collisions. */
      distTravelled.x = handleHorizontalMovementsWhenStopping(coordRules);
    }
  else if(input == sprite::DIR_DOWN)
    {
      // The player is falling and their is no input or the player inputs down.
      input = handleGroundCollision(coordRules);      
      /* If we still have left / right momentum we need to check for horizontal
	 collisions. */
      distTravelled.x = handleHorizontalMovementsWhenStopping(coordRules);
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      distTravelled.x = handleLeftOrRightCollision(coordRules, input, true);
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      distTravelled.x = handleLeftOrRightCollision(coordRules, input, false);
    }

#ifdef DEBUG
  /* No sprite should more move than one character per tick of the
    engine. Otherwise things could break. */
  if(abs(distTravelled.x) > 1 || abs(distTravelled.y) > 1)
    {
      exit(concat("Error: sprite found to have travelled more than |1| "
		  "character in 1 tick. DistTravelled = ", distTravelled),
	   ERROR_BAD_LOGIC);
    }
#endif

  positionVPRel += distTravelled;
  updateDirection(input);
}


#endif
