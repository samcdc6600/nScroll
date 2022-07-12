#ifndef BACKGROUND_SPRITE_H_
#define BACKGROUND_SPRITE_H_
#include "sprite.hpp"


class bgSprite : public sprite
{
public:
  bgSprite(std::vector<std::string> spriteFileNames, const yx max, const yx pos,
	   const directions dir) : sprite(spriteFileNames, max, pos, dir)
  {
    
  }

  void draw(const bool updateSlice, const int bgPos);
};

#endif
