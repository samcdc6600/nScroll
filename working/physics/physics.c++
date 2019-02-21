#include "physics.h++"
#include "../sprite/sprite.h++"
#include "levelInteractionHandler.h++"


#include <sstream>
void rules::physics(const int input, int & position, const size_t backgroundLength)
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


void rules::movePlayer(const sprite::directions input)
{
  yx peekPos {gamePlayer->peekAtPos(input)};
  if(gamePlayer->inBounds(peekPos.y, peekPos.x, playerInnerBoarder.y, playerInnerBoarder.x))
    {
      gamePlayer->updatePosRel(sprite::directions(input));      
    }
}
