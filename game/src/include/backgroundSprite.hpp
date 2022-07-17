#ifndef BACKGROUND_SPRITE_H_
#define BACKGROUND_SPRITE_H_
#include "sprite.hpp"


class bgSprite : public sprite
{
public:
  bgSprite(std::vector<std::string> spriteFileNames, const yx max,
	   const size_t bgSize, const yx pos, const directions dir) :
    sprite(spriteFileNames, max, bgSize, pos, dir)
  {
    
  }
};

#endif
