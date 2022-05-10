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
  static constexpr size_t second {1};
  /* The key's for these maps should be of the form "Y,X" Where Y and X are 
     numbers and the whole thing is a string */
  std::map<std::string, char> coordChars {}; /* For coordinate rules. */
  // For sprites (holds sprite data (slices) and the rule for the sprite.)
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
  // Moves the player when interacting with the inner margin.
  void movePlayerWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  void movePlayerRightWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  void movePlayerLeftWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  /* Handles collision with boarder characters when the player is moving down
     Input should be the player input direction and the current absolute
     position of the player sprite in the x dimension. Returns updated
     direction. */
  sprite::directions handleGroundCollision(sprite::directions input,
					   const int & position);
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character unless the player has a y position of 0, in which case the
     player will be stopped. The player sprite will also be stopped if there is
     more then one character directly to the right of it. */
  sprite::directions handleRightCollision(const sprite::directions input,
					      const int & position);

public:
  void physics(const int input, int & position, const yx maxyx,
	       const size_t backgroundLength,
	       std::__1::chrono::steady_clock::time_point & secStartTime);
  ~rules()
  {
    delete(gamePlayer);
  }
};


#endif
