#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "../sprite/sprite.h++"
#include "../sprite/player/player.h++"

struct rules
{
  struct spriteInfo		/* For sprites (holds sprite data (slices) and the rule for the sprite. */
  {
    /* Sprite data to go here (when I do it.) */
    sprite * sp {};		/* This has to be a pointer because sprite is an ABC. */
    std::string rule {};
  };
  /* The key's for these maps should be of the form "Y,X" Where Y and X are numbers and the whole thing is a string */
  std::map<std::string, char> coordChars {}; /* For coordinate rules. */
   /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer;		/* Info about user controlled sprite (AKA player.) */
  ~rules()
  {
    delete(gamePlayer);
  }

private:

  // Moves the player 
  void movePlayer(const sprite::directions input, const yx maxyx);

public:
  void physics(const int input, int & position, const yx maxyx, const size_t backgroundLength);
};

#endif
