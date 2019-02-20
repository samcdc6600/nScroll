#include "physics.h++"
#include "../sprite/sprite.h++"
#include "levelInteractionHandler.h++"


int rules::physics(const int input)
{
  if(input == sprite::UP || input == sprite::UP_UPPER ||
	       input == sprite::LEFT || input == sprite::LEFT_UPPER ||
	       input == sprite::DOWN || input == sprite::DOWN_UPPER ||
	       input == sprite::RIGHT || input == sprite::RIGHT_UPPER)
              {
		movePlayer(sprite::directions(input));
		gamePlayer->updatePosRel(sprite::directions(input));
	      }
  return playerPosition;
}


void rules::movePlayer(const sprite::directions input)
{
}
