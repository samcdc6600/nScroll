#ifndef BACKGROUND_SPRITE_HPP_
#define BACKGROUND_SPRITE_HPP_
#include "sprite.hpp"


class bgSprite : public sprite
{
public:
  /* Draw sprite after player sprite? Note that this doesn't effect the sprite
     draw routine and must be check and appropriately handled in the main draw
     routine. */
  const bool displayInForground; 

  
  bgSprite(const double fixedTimeStep, std::vector<std::string> spriteFileNames,
	   const yx viewPortSize, const yx pos, const directions dir,
	   const bool dispInFg) :
    sprite(fixedTimeStep, spriteFileNames, viewPortSize, pos, dir),
    displayInForground(dispInFg)
  {}


  void draw(unsigned short * secondStageDrawBuffer, const bool updateSlice,
	    const yx viewPortPos);

private:
  // Main body of draw code.
  void drawProper
  (unsigned short * secondStageDrawBuffer, const bool updateSlice,
   const yx viewPortPos);
  /* Inner loop of draw code. Should be called from within iteration over y
     axis. */
  void handleXCompAndDraw
  (unsigned short * secondStageDrawBuffer, const bool updateSlice,
   const yx viewPortPos, const int sliceLineIter, const int sliceLineCharIter);
};

#endif
