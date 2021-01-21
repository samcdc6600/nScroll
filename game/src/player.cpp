//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "include/player.hpp"
#include "include/colorS.hpp"
#include "include/draw.hpp"

extern setColorMode colorMode;//must be included for the draw function


yx player::getNewPos(const directionChars dir)
{
  return sprite::getNewPos(convertDirectionCharsToDirections(dir));
}


player::player(std::vector<std::string> sprites, const yx max, const yx pos,
	       const directionChars dir, const int h)
  : sprite(max, pos, convertDirectionCharsToDirections(dir), sprites[INITIAL_SPRITE_INDEX].c_str()), health(h)
{
  if(sprites.size() != NUM_PLAYER_SPRITES)
    {
      std::stringstream e {};
      e<<"Error when initialising player: "<<sprites.size()<<" player "
	"sprites, but expected "<<NUM_PLAYER_SPRITES<<" sprites.";
      exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }

  for(int iter {ADDITIONAL_SPRITES_OFFSET}; iter < NUM_PLAYER_SPRITES; ++iter)
    {
      loadSprite(sprites[iter].c_str(), sD_player[iter]);
    }
}


sprite::directions player::convertDirectionCharsToDirections
(const directionChars dir)
{
  directions ret {DIR_NONE};
  switch(dir)
    {
    case UP_CHAR:
    case UP_UPPER_CHAR:
      ret = DIR_UP;
      break;
    case RIGHT_UP_CHAR:
    case RIGHT_UP_UPPER_CHAR:
      ret = DIR_RIGHT_UP;
	break;
    case RIGHT_CHAR:
    case RIGHT_UPPER_CHAR:
      ret = DIR_RIGHT;
	break;
    case RIGHT_DOWN_CHAR:
    case RIGHT_DOWN_UPPER_CHAR:
      ret = DIR_RIGHT_DOWN;
	break;
    case DOWN_CHAR:
    case DOWN_UPPER_CHAR:
      ret = DIR_DOWN;
	break;
    case LEFT_DOWN_CHAR:
    case LEFT_DOWN_UPPER_CHAR:
      ret = DIR_LEFT_DOWN;
	break;
    case LEFT_CHAR:
    case LEFT_UPPER_CHAR:
      ret = DIR_LEFT;
	break;
    case LEFT_UP_CHAR:
    case LEFT_UP_UPPER_CHAR:
      ret = DIR_LEFT_UP;
	break;
    default:
      std::stringstream e {};
      e<<"Error direction with ASCII code("<<dir<<") not valid.";
      exit(e.str().c_str(), ERROR_INVALID_DIRECTION);
    }
  return ret;
}


yx player::peekAtPos(const directionChars dir)
{
  return getNewPos(dir);
}


void player::updatePosRel(const directionChars dir)
{ /* Update's position of sprite in a relative fashion with reference to the
     sprite and update's direction. */
  yx p {getNewPos(dir)};
  position = p;
  if(direction != dir)
    {				// Change direction.
      resetCurrentSliceNum();
      direction = dir;
    }  
}


void player::draw(bool updateSlice)
{
  switch(direction)
    {
    case DIR_LEFT:
      sprite::draw(DIR_LEFT, updateSlice);
      break;
    case DIR_RIGHT:
      sprite::draw(DIR_RIGHT, updateSlice);
      break;
    case DIR_UP:
      sprite::draw(DIR_UP, updateSlice);
      break;
    case DIR_DOWN:
      sprite::draw(DIR_DOWN, updateSlice);
      break;
    default:
      //add exception here!------------------------------------------~~~~~~~~~~~~~~*******************(O)
      break;
    }    
}
