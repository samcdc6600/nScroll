#ifndef NON_PLAYER_SPRITE_HPP_
#define NON_PLAYER_SPRITE_HPP_
#include "sprite.hpp"


class nonPlayerSprite: public sprite
{
public:
  nonPlayerSprite
  (std::vector<std::string> & spritePaths, const yx max, const yx pos,
   const directions dir) :
    sprite(spritePaths, max, pos, dir)
  { }
  void draw(unsigned short * secondStageDrawBuffer, const bool updateSlice,
	    const yx viewPortPos);
};


#endif
