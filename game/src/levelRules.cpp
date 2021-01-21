#include "include/levelRules.hpp"
#include "include/player.hpp"


//test level agains sprite's
char rules::intersection(const std::string & boundsInteraction,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any character other then space in
     boundsInteration return that charater */
  return 'n';
}

//test player sprite against sprite's
char rules::intersection(const std::vector<int> playerSpChoords,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return characte code or something */
  return 'n';
}

//test level agains sprite's
char rules::nearPass(const std::string & boundsInteraction,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords is next to any character other then space in
     boundsInteration return that charater */
  return 'n';
}

//test player sprite against sprite's
char rules::nearPass(const std::vector<int> playerSpChoords,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return a character code that corispondest to the side that was matched
     relative to spChoords for example 't' for top. (this might be used to
     detect that the player has "touched" the top of the sprite and if it is an
     enemy sprite then appropriate action can be taken. */
  return 'n';
}


void rules::movePlayer(const player::directionChars input)//, const yx maxyx)
{
  yx peekPos {gamePlayer->peekAtPos(input)};
  if(gamePlayer->inBounds(peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_BOARDER.y,
			  PLAYER_MOVEMENT_INNER_BOARDER.x))
    {
      gamePlayer->updatePosRel(player::directionChars(input));      
    }
  else
    { /* Move the player as long as they will stay within
	 PLAYER_MOVMENT_INNER_BOARDER of the left and right borders. If the
	 player is PLAYER_MOVMENT_INNER_BOARDER away from either the left or
	 right boarder and tries to move closer to the boarder they are closest
	 too then the background should be moved instead of the player in the
	 appropriate direction (If there is background to spare.) If we are at
	 either end of the level then the player cannot move further of
	 course. */
      switch(input)
	{
	  
	}
    }
}


void rules::physics(const int input, int & position, const yx maxyx,
		    const size_t backgroundLength)
{
  if(input == player::UP_CHAR || input == player::UP_UPPER_CHAR ||
     input == player::LEFT_CHAR || input == player::LEFT_UPPER_CHAR ||
     input == player::DOWN_CHAR || input == player::DOWN_UPPER_CHAR ||
     input == player::RIGHT_CHAR || input == player::RIGHT_UPPER_CHAR)
    {
      movePlayer(player::directionChars(input));
      //		gamePlayer->updatePosRel(sprite::directions(input));
    }
}
