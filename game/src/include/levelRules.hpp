#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"


class rules
{
public:
  // The player cannot pass widthin this many character's of the window boarder's (y, x).
  const yx PLAYER_MOVEMENT_INNER_MARGIN {0, 40};
  static constexpr size_t second {1};
  // Contains position based rules.
  std::vector<char> coordRules {};
  //For sprites (holds sprite data (slices) and the rule for the sprite.)
  struct spriteInfo
  { // Sprite data to go here (when I do it.)
    sprite * sp {}; // This has to be a pointer because sprite is an ABC.
    std::string rule {};
  };
  /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer; /* Info about user controlled sprite (AKA player.) */
  /* The player cannot pass widthin this many character's of the left and right
     window boarders (y, x). */
  const yx playerMovementInnerLRBoarder {0, 44};
  const int backgroundHeight {33};
  
private:
  const size_t millisecondsInSec {1000*second};
  // The duration of time we call sleep() for (in milliseconds.)
  const size_t engineSleepTime {32};
  const double sleepTimeAsAPercentageOfASecond {double(engineSleepTime / millisecondsInSec)};

  /* Set's oldTime to the current time if
     (oldTime - (the current time) >= second). */
  void resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime);
  // check level agains sprite
  char intersection(const std::string & boundsInteraction,
		    const std::vector<int> spChoords);
  // check player against sprite
  char intersection(const std::vector<int> playerSpChoords,
		    const std::vector<int> spChoords);
  // check level against sprite
  char nearPass(const std::string & boundsInteraction,
		const std::vector<int> spChoords);
  // check player against sprite
  char nearPass(const std::vector<int> playerSpChoords,
		const std::vector<int> spChoords);
  // Moves the player 
  void movePlayer(sprite::directions input,
		  int & position, const yx maxyx,
		  const size_t backgroundLength);
  /* No functions that change the position of the player should be called after
     this one for a given frame. */
  /* Calls handleFinalPlayerMovementAndWindowAndMarginInteractions after
     checking that the player won't go outside of the level after it's position
     is updated to peekPos. */
  void handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength);
  void handleFinalPlayerMovementAndWindowAndMarginInteractions
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* Moves the player when interacting with the inner margin (NOTE THAT THIS
     FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
     THE INNER MARGIN AND NOT OUTSIDE THE BOUNDS OF THE LEVEL AS DEFINED BY THE
     BACKGROUND.) */
  void movePlayerWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerRightWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerLeftWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  // /* Returns number of characters untill the player bits a boarder character
  //    (if moving up). Will return -1 if there are no boarder characters above */
  // int getClosestBoarderCharAbove(const int position, const bool directContact);
  /* Handles collision with boarder characters when the player is moving down
     Input should be the player input direction and the current absolute
     position of the player sprite in the x dimension. Returns updated
     direction. */
  sprite::directions handleGroundCollision(const int position);
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character unless the player has a y position of 0, in which case the
     player will be stopped. The player sprite will also be stopped if there is
     more then one character directly to the right of it. */
  sprite::directions handleRightCollision(const int position);
  // // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision(const int position);

#ifdef DEBUG
  // Where maxX should be the maximum size of the window and not the level.
  void printRuleChars(const int position, const int maxY, const int maxX);
#endif

public:
  void physics(const player::directionChars input, int & position, const yx maxyx,
	       const size_t backgroundLength,
	       std::__1::chrono::steady_clock::time_point & secStartTime);
  ~rules()
  {
    delete(gamePlayer);
  }

  // // Converts linear address to y x pos.
  // char getCoordRule(const int linearAddress)
  // {
  //   int y {linearAddress / backgroundHeight};
  //   int x {linearAddress % backgroundHeight};
    
  //   if(y > backgroundHeight)
  //     {
  // 	// Error
  //     }
  //   if(x > (coordRules.size() % backgroundHeight))
  //     {
  // 	// Error
  //     }

  //   getCoordRule(y, x);
  // }
};


#endif
