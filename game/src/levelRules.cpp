#include "include/levelRules.hpp"
#include "include/player.hpp"
#include <string>


void rules::resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime)
{
  std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::high_resolution_clock::now()};
  if((duration_cast<std::chrono::duration<double>>
      (currentTime - oldTime)).count() >= rules::second)
    {
      oldTime = currentTime;
    }
}

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

// =================== TMP TMP TMP ==========================
#include <curses.h>
#include <iostream>
#include <fstream>
bool encountered {false};
// =================== TMP TMP TMP ==========================

void rules::movePlayer(sprite::directions input,
		       int & position, const yx maxyx,
		       const size_t backgroundLength)
{ /* Move the player as long as they will stay within
     PLAYER_MOVMENT_INNER_MARGIN of the left and right borders. If the
     player is PLAYER_MOVMENT_INNER_MARGIN away from either the left or
     right boarder and tries to move closer to the boarder they are closest
     too then the background should be moved instead of the player in the
     appropriate direction (If there is background to spare.) If we are at
     either end of the level then the player cannot move further of
     course. */
  const int currDir {gamePlayer->getDirection()};
  double vertVelocity {gamePlayer->getVertVelocity()};

  /* We intend to alter this later to have it read in from the level rules
     file */
  double g {-3.2666};
  /*  if(noVertCollision)
      {*/
  /*  if(timer)
    {*/
  vertVelocity *= g;
    //    }
  //    }

  
  /**/


  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(position);
    }
  else if((currDir == sprite::DIR_RIGHT &&
	   input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      input = handleRightCollision(position);
    }

  handleFinalPlayerMovementAndWindowAndMarginInteractions
    (input, position, maxyx, backgroundLength, gamePlayer->peekAtPos(input));
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractions
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
    /* Make any final movement, check for window margin contact and take
     appropriate action if such contact is made. */
  if(((newDir == sprite::DIR_LEFT || newDir == sprite::DIR_RIGHT) &&
      gamePlayer->notInWindowInnerMarginX
      (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x)) ||      
     newDir == sprite::DIR_NONE)	// Only need to check for DIR_NONE here.
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else if(((newDir == sprite::DIR_DOWN || newDir == sprite::DIR_UP) &&
	   gamePlayer->notInWindowInnerMarginY
	   (peekPos.y, PLAYER_MOVEMENT_INNER_MARGIN.y)))
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else
    {
      movePlayerWhenInteractingWithInnerMargin(newDir, position, maxyx,
					       backgroundLength, peekPos);
    }
    // if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
  // 				PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 				PLAYER_MOVEMENT_INNER_MARGIN.x))
  //   {				// The player is moving within the inner margin
  //     gamePlayer->updatePosRel(newDir);
  //   }
  // else
  //   {
      
  //   }
}


/* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT THE PLAYER IS
   NOT IN THE INNER MARGIN.*/
void rules::movePlayerWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
  /* We use this variable in the call's to inWindowInnerMargin() when peekPos
     is out of the x boarder range. */
  constexpr int REACHED_INNER_MARGIN_X {0};
  switch(input)
    {
    case sprite::DIR_NONE:
      break;
    case sprite::DIR_UP:
      if(gamePlayer->inWindowY(peekPos.y))
	{
	  gamePlayer->updatePosRel(input);
	}
      break;
    case sprite::DIR_RIGHT:
      movePlayerRightWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;

    case sprite::DIR_DOWN:
      if(gamePlayer->inWindowY(peekPos.y))
	{
	  gamePlayer->updatePosRel(input);
	}
      break;
    case sprite::DIR_LEFT:
      movePlayerLeftWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    }
}


void rules::movePlayerRightWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  // if(gamePlayer->inWindowInnerMargin
  //    (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
  //     REACHED_INNER_MARGIN_X) &&
  //    gamePlayer->leftOfWindowInnerRightMargin
  //    (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x, maxyx))
  //   { // We are to the left of the inner right margin.
  //     gamePlayer->updatePosRel(input);
  //   }
  // else
  //   {
  //     if(gamePlayer->inWindowInnerMargin
  // 	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 	  REACHED_INNER_MARGIN_X) && (position + maxyx.x) < backgroundLength)
  // 	{ // There is still background left to spare.
  // 	  gamePlayer->updateDirection(input);
  // 	  position++;	// Move background.
  // 	}
  //     else
  // 	{
  // 	  if((position + maxyx.x) == backgroundLength &&
  // 	     (gamePlayer->inWindowInnerMargin
  // 	      (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 	       REACHED_INNER_MARGIN_X)))
  // 	    { /* No background left, so move the player to the right
  // 		 edge of the background. */
  // 	      gamePlayer->updatePosRel(input);
  // 	    }
  // 	}
  //   }
}


void rules::movePlayerLeftWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  // if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
  // 				     PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 				     REACHED_INNER_MARGIN_X) &&
  //    gamePlayer->rightOfWindowInnerLeftMargin
  //    (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x))
  //   { // We are to the righ of the inner left margin.
  //     gamePlayer->updatePosRel(input);
  //   }
  // else
  //   {
  //     if(gamePlayer->inWindowInnerMargin
  // 	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 	  REACHED_INNER_MARGIN_X) && (position > 0))
  // 	{ // There is still background left to spare.
  // 	  gamePlayer->updateDirection(input);
  // 	  position--;	// Move background.
  // 	}
  //     else
  // 	{
  // 	  if(position == 0 &&
  // 	     (gamePlayer->inWindowInnerMargin
  // 	      (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
  // 	       REACHED_INNER_MARGIN_X)))
  // 	    { /* No background left, so move the player to the left
  // 		 edge of the background. */
  // 	      gamePlayer->updatePosRel(input);
  // 	    }
  // 	}
  //   }
}


/* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
sprite::directions rules::handleGroundCollision(const int & position)
{
  sprite::directions retDir {sprite::DIR_DOWN};
  for(std::string coord:
	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact(position))
    {
      if(coordChars.find(coord) != coordChars.end())
	{
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
sprite::directions rules::handleRightCollision(const int & position)
{
  sprite::directions retDir {sprite::DIR_RIGHT};
  const std::vector<std::string> playerCoords
    {gamePlayer->getRightYAbsRangeAsStrsForOneOffContact(position)};
  const std::string bottomRightPlayerCoord
    {playerCoords[playerCoords.size() -1]};
  bool stoppingContact {false};
    
  for(std::string playerCoord: playerCoords)
    {
      // If there is near contact and it's not with the bottom right coord.
      if(playerCoord != bottomRightPlayerCoord &&
          coordChars.find(playerCoord) != coordChars.end())
	{
	  stoppingContact = true;
	  retDir = sprite::DIR_NONE;

	  break;
	}
    }
  
  if(!stoppingContact &&
     coordChars.find(bottomRightPlayerCoord) != coordChars.end())
    {
      if(gamePlayer->getPos().y > 0)
	{
	  /* If we've hit a "step" (as in the things that constitute staircases)
	     and we are not at the minimum (top of window) y position, then
	     "step up" :). */
	  gamePlayer->updatePosRel(sprite::DIR_UP);
	}
      else
	{
	  // We've hit the top of the window.
	  retDir = sprite::DIR_NONE;
	}
    }
  
  return retDir;
}


void rules::physics
(const player::directionChars input, int & position, const yx maxyx,
 const size_t backgroundLength,
 std::__1::chrono::steady_clock::time_point & secStartTime)
{
  movePlayer(player::convertDirectionCharsToDirections(input), position,
	     maxyx, backgroundLength);
  
  sleep(engineSleepTime);
  resetOldTime(secStartTime);
}
