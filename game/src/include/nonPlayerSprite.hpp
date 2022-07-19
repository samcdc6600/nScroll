#ifndef NON_PLAYER_SPRITE_HPP_
#define NON_PLAYER_SPRITE_HPP_
#include "sprite.hpp"


class nonPlayerSprite: public sprite
{
public:
  nonPlayerSprite
  (std::vector<std::string> & spritePaths, const yx max, const size_t bgSize,
   const yx pos, const directions dir) :
    sprite(spritePaths, max, bgSize, pos, dir)
  { }
    void draw(const bool updateSlice, const int bgPos);
};


#endif
