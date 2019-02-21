#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include "../sprite.h++"
#include "../../common.h++"


class player: public sprite
{/*this class should expand on the ABC sprite by adding the following member's and methods:
   spriteSliceS1 (shold hold the slices that make up the sprite), cycleSpeed1 (should hold the cycle speed of the
   second sprite), direction (should hold the direction that the player is moving in. The direction should also
   dictate whitch of the two sprite's is used), health (should hold the health of the player),
   -----------------METHODS FOR THIS CLASS SHOULD BE LISTED HERE----------------------*/

  //these objects hold the slices that make up the 2nd, 3rd and 4th sprites
  static constexpr int NUM_PLAYER_SPRITES {4};
  static constexpr int INITIAL_SPRITE_INDEX {0};
  static constexpr int ADDITIONAL_SPRITES_OFFSET {1};
  spriteData sD_player [NUM_PLAYER_SPRITES];
  int health;
  int direction;
  //this function has intentionally been left in a private section of this class and with an empty
  //body because it is not intended for use.
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}
 public:
  //read's sprite files and converts them so they may be used with the classes draw function/s
 player(std::vector<std::string> sprites, const yx max, const yx pos, const int h, const int d)
   : sprite(max, pos, sprites[INITIAL_SPRITE_INDEX].c_str()), health(h), direction(d)
  {
    if(!checkDirection(directions(d)))
      {
	std::stringstream e {};
	e<<"Error when initialising player: direction ("<<d<<") out of range.";
	exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
      }
    if(sprites.size() != NUM_PLAYER_SPRITES)
      {
	std::stringstream e {};
	e<<"Error when initialising player: "<<sprites.size()<<" player sprites, but expected "
	 <<NUM_PLAYER_SPRITES<<" sprites.";
	exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
      }

    for(int iter {ADDITIONAL_SPRITES_OFFSET}; iter < NUM_PLAYER_SPRITES; ++iter)
      {
	getSprite(sprites[iter].c_str(), sD_player[iter]);
      }
  }
  
  
  virtual ~player() {};
  virtual void updatePosRel(directions dir);
  virtual void draw(bool updateSlice);
  //  virtual void draw() {draw(false);}
  
};

#endif
