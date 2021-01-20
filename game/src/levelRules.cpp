#include "include/levelRules.hpp"
#include "include/sprite.hpp"


//test level agains sprite's
char rules::intersection(const std::string & boundsInteraction, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any character other then space in boundsInteration
  //return that charater
  return 'n';
}

//test player sprite against sprite's
char rules::intersection(const std::vector<int> playerSpChoords, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any value in playerSpChoords
  //return characte code or something
  return 'n';
}

//test level agains sprite's
char rules::nearPass(const std::string & boundsInteraction, const std::vector<int> spChoords)
{
  //if any value in spChoords is next to any character other then space in boundsInteration
  //return that charater
  return 'n';
}

//test player sprite against sprite's
char rules::nearPass(const std::vector<int> playerSpChoords, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any value in playerSpChoords
  //return a character code that corispondest to the side that was matched relative to spChoords
  //for example 't' for top. (this might be used to detect that the player has "touched" the top
  //of the sprite and if it is an enimy sprite then appropriate action can be taken.
  return 'n';
}


void rules::movePlayer(const sprite::directions input)//, const yx maxyx)
{
  yx peekPos {gamePlayer->peekAtPos(input)};
  if(gamePlayer->inBounds(peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_BOARDER.y,
			  PLAYER_MOVEMENT_INNER_BOARDER.x))
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


void rules::physics(const int input, int & position, const yx maxyx,
		    const size_t backgroundLength)
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
