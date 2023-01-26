#ifndef BACKGROUND_SPRITE_HPP_
#define BACKGROUND_SPRITE_HPP_
#include "nonPlayerSprite.hpp"


class bgSprite : public nonPlayerSprite
{
public:
  /* Draw sprite after player sprite? Note that this doesn't effect the sprite
     draw routine and must be check and appropriately handled in the main draw
     routine. */
  const bool displayInForground; 

  
  bgSprite(std::vector<std::string> spriteFileNames, const yx max,
	   const backgroundData & background, const yx pos, const directions dir,
	   const bool dispInFg) :
    nonPlayerSprite(spriteFileNames, max, background, pos, dir),
    displayInForground(dispInFg)
  {
    
  }  
};

#endif
