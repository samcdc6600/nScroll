#include "physics.h++"
#include "../sprite/sprite.h++"
#include "levelInteractionHandler.h++"


#include <sstream>
void rules::physics(const int input, int & position, const yx maxyx, const size_t backgroundLength)
{
  if(input == sprite::UP || input == sprite::UP_UPPER ||
	       input == sprite::LEFT || input == sprite::LEFT_UPPER ||
	       input == sprite::DOWN || input == sprite::DOWN_UPPER ||
	       input == sprite::RIGHT || input == sprite::RIGHT_UPPER)
              {
		movePlayer(sprite::directions(input));
		//		gamePlayer->updatePosRel(sprite::directions(input));
	      }
}


void rules::movePlayer(const sprite::directions input, const yx maxyx)
{
  yx peekPos {gamePlayer->peekAtPos(input)};
  if(gamePlayer->inBounds(peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_BOARDER.y, PLAYER_MOVEMENT_INNER_BOARDER.x))
    {
      gamePlayer->updatePosRel(sprite::directions(input));      
    }
  else
    {				/* Move background as long as player will stay within PLAYER_MOVMENT_INNER_BOARDER.
				   Otherwide just move player and if player will move off of level don't move player
				   at all. */
      //      switch(input)
	{
	  //      if(peekPos -
	 }
    }
}
