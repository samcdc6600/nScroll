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


void rules::movePlayer(const player::directionChars input,
		       int & position, const yx maxyx,
		       const size_t backgroundLength)
{ /* This function like many in this program aren't very well designed
     LOL :'(. */
  yx peekPos {gamePlayer->peekAtPos(input)};
  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
				PLAYER_MOVEMENT_INNER_BOARDER.y,
				PLAYER_MOVEMENT_INNER_BOARDER.x))
    {
      //              exit("No seg fault here!", 0);
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
      /* We use this variable in the calls to inWindowInnerMargin() when peekPos is
	 out of the x boarder range. */
      constexpr int REACHED_INNER_BOARDER_X {0};
      switch(player::convertDirectionCharsToDirections(input))
	{
	case sprite::DIR_UP:
	  /* We use PLAYER_MOVEMENT_INNER_BOARDER.y here because we don't
	     support scrolling in the y dimension. If
	     PLAYER_MOVEMENT_INNER_BOARDER.y = 0 this point should not be
	     reached. */
	  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
					     PLAYER_MOVEMENT_INNER_BOARDER.y,
					     REACHED_INNER_BOARDER_X))
	    {
	      gamePlayer->updatePosRel(player::directionChars(input));
	    }
	  break;
	  // ===================================================================
	case sprite::DIR_RIGHT:
	  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
					     PLAYER_MOVEMENT_INNER_BOARDER.y,
					     REACHED_INNER_BOARDER_X) &&
	     (position + maxyx.x) < backgroundLength)
	    { // There is still background left to spare.
	      position++;	// Move background.
	    }
	  else
	    {
	      if((position + maxyx.x) == backgroundLength &&
		 (gamePlayer->inWindowInnerMargin
		  (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_BOARDER.y,
		   REACHED_INNER_BOARDER_X)))
		{ /* No background left, so move the player to the right edge of
		     the background. */
		  gamePlayer->updatePosRel(player::directionChars(input));
		}
	    }
	  ///	  exit("Right move not implemented here", 0);
	  break;
	  // ===================================================================
	case sprite::DIR_DOWN:
	  /* We use PLAYER_MOVEMENT_INNER_BOARDER.y here because we don't
	     support scrolling in the y dimension. If
	     PLAYER_MOVEMENT_INNER_BOARDER.y = 0 this point should not be
	     reached. */
	  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
				  PLAYER_MOVEMENT_INNER_BOARDER.y,
				  REACHED_INNER_BOARDER_X))
	    {
	      gamePlayer->updatePosRel(player::directionChars(input));
	    }
	  break;
	  // ===================================================================
	case sprite::DIR_LEFT:
	  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
					     PLAYER_MOVEMENT_INNER_BOARDER.y,
					     REACHED_INNER_BOARDER_X) &&
	     (position > 0))
	    { // There is still background left to spare.
	      position--;	// Move background.
	    }
	  else
	    {
	      //	      exit("Left move not implemented here", 0);
	      if(position == 0 &&
		 (gamePlayer->inWindowInnerMargin
		  (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_BOARDER.y,
		   REACHED_INNER_BOARDER_X)))
		{ /* No background left, so move the player to the left edge of
		     the background. */
		  gamePlayer->updatePosRel(player::directionChars(input));
		}
	    }
	  //		  
	  break;
	}
    }
}


void rules::physics(const int input, int & position, const yx maxyx,
		    const size_t backgroundLength)
{
  if(player::isDirectionCharInputValid(input))
    {
      movePlayer(player::directionChars(input), position, maxyx,
		 backgroundLength);
    }
}
