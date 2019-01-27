#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include "../sprite.h"
#include "../../common.h"
#include "direction.h"	//contains an enumberation with direction definitions

class player: public sprite
{/*this class should expand on the ABC sprite by adding the following member's and methods:
   spriteSliceS1 (shold hold the slices that make up the sprite), cycleSpeed1 (should hold the cycle speed of the
   second sprite), direction (should hold the direction that the player is moving in. The direction should also
   dictate whitch of the two sprite's is used), health (should hold the health of the player),
   -----------------METHODS FOR THIS CLASS SHOULD BE LISTED HERE----------------------*/

  //these objects hold the slices that make up the 2nd, 3rd and 4th sprites
  spriteData sD_player1;
  spriteData sD_player2;
  spriteData sD_player3;
  int health;
  int direction;
  //this function has intentional been left in a private section of this class and with an empty
  //body because it is not intended for use.
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}
 public:
  //read's sprite files and converts them so they may be used with the classes draw function/s
 player(const char spriteFileOneName [], const char spriteFileTwoName [], const char spriteFileThreeName [],
	const char spriteFileFourName [], const yx max, const yx pos, const int h, const int d)
   : sprite(max, pos, spriteFileOneName), health(h), direction(d)
  {
    if(d != DIR_LEFT && d != DIR_RIGHT)
      {
	std::stringstream errorMsg;
	errorMsg<<"in \"player.h -> player(const std::string & spriteFileOneName, const std::string & "
	  "spriteFileTwoName, yx max, yx pos, int h, int d)\", d is != DIR_LEFT or DIR_RIGHT. Out of range!"
	  "d = "<<d;
	throw std::logic_error(errorMsg.str());
      }
    getSprite(spriteFileTwoName, sD_player1);
    getSprite(spriteFileThreeName, sD_player2);
    getSprite(spriteFileFourName, sD_player3);
  }
  
  
  virtual ~player() {};
  virtual void getHardBoundaryCoordinates(std::vector<int> & spCoords);
  virtual void getSoftBoundaryCoordinates(std::vector<int> & spCoords);
  virtual void updatePosRel(char ch);//update's position of sprite in a relative fashion with reference to the sprite.
  virtual void draw(bool updateSlice);
  //  virtual void draw() {draw(false);}
  
};

#endif
