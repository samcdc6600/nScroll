#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"


class rules
{
private:
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
  void movePlayer(const player::directionChars input,
		  int & position, const yx maxyx,
		  const size_t backgroundLength);//, const yx maxyx);

public:
  // The duration of time we call sleep() for in the game loop.
  const size_t engineSleepTime {32};
  /* The player cannot pass widthin this many character's of the left and right
      window boarders (y, x). */
  const yx playerMovementInnerLRBoarder {0, 44};
  const int backgroundHeight {33};
  // For sprites (holds sprite data (slices) and the rule for the sprite.)
  struct spriteInfo
  { // Sprite data to go here (when I do it.)
    sprite * sp {}; // This has to be a pointer because sprite is an ABC.
    std::string rule {};
  };
  /* The key's for these maps should be of the form "Y,X" Where Y and X are 
     numbers and the whole thing is a string */
  std::map<std::string, char> coordChars {}; /* For coordinate rules. */
  /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer; /* Info about user controlled sprite (AKA player.) */
  
  void physics(const int input, int & position, const yx maxyx,
	       const size_t backgroundLength);
  ~rules()
  {
    delete(gamePlayer);
  }
};


#endif
