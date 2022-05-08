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
  const int currentDirection {gamePlayer->getDirection()};
  yx peekPos {gamePlayer->peekAtPos(input)};
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


  /*  // Handle collision with boarder if moving down.
  if(coordChars.find((gamePlayer->getMaxYAbsAsStr(1)) + "," + // Check for
							      // boarder 0 in
							      // front and 0
							      // above feet.
		     gamePlayer->getMaxXAbsLevelRightAsStr(0, position))
     != coordChars.end())
    {
      // endwin();
      // std::cout<<"currentDirection = "<<currentDirection<<", "<<"sprite::DIR_DOWN"<<" = "<<sprite::DIR_DOWN<<'\n';
      // gamePlayer->updateDirection(sprite::DIR_NONE);
      // std::cout<<"gamePlayer->getDirection() = "<<gamePlayer->getDirection()<<'\n';
      // std::cout<<"input = "<<input<<'\n';
      // exit(-1);
      if(currentDirection == sprite::DIR_DOWN)
	{
	  gamePlayer->updateDirection(sprite::DIR_NONE);
	  }
      else if(currentDirection == sprite::DIR_RIGHT_DOWN)
	{
	  gamePlayer->updateDirection(sprite::DIR_RIGHT);
	}
    }

  
  if(coordChars.find((gamePlayer->getMaxYAbsAsStr(1)) + "," + // Check for
							      // boarder 0 in
							      // front and 0
							      // above feet.
		     gamePlayer->getMaxXAbsLevelRightAsStr(0, position))
     != coordChars.end() &&
     // Check for boarder 1 in front and 1 above feet.
     coordChars.find(gamePlayer->getMaxYAbsAsStr(0) + "," +
		     gamePlayer->getMaxXAbsLevelRightAsStr(1, position))
     != coordChars.end() &&
     // Check for no boarder 1 in front and 2 above feet.
          coordChars.find(gamePlayer->getMaxYAbsAsStr(-1) + "," + 
		     gamePlayer->getMaxXAbsLevelRightAsStr(1, position))
     == coordChars.end())
    {
      gamePlayer->updatePosRel(sprite::DIR_UP);
      endwin();
      std::cout<<gamePlayer->getMaxYAbsAsStr(1) + "," + gamePlayer->getMaxXAbsLevelRightAsStr(0, position)<<'\n';
      std::cout<<gamePlayer->getMaxYAbsAsStr(0) + "," + gamePlayer->getMaxXAbsLevelRightAsStr(1, position)<<'\n';
      exit(-1);
      }*/



  if(currentDirection == sprite::DIR_DOWN || input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(input, position);
    }
  else if(currentDirection == sprite::DIR_RIGHT || input == sprite::DIR_RIGHT)
    {
    }
  
  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
				PLAYER_MOVEMENT_INNER_MARGIN.y,
				PLAYER_MOVEMENT_INNER_MARGIN.x))
    {				// The player is moving within the inner margin
      gamePlayer->updatePosRel(input);
    }
  else
    {
      movePlayerWhenInteractingWithInnerMargin(input, position, maxyx,
					       backgroundLength, peekPos);
    }
}


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
      /* We use PLAYER_MOVEMENT_INNER_MARGIN.y here because we don't
	 support scrolling in the y dimension. If
	 PLAYER_MOVEMENT_INNER_MARGIN.y = 0 this point should not be
	 reached. */
      if(gamePlayer->inWindowInnerMargin
	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	  REACHED_INNER_MARGIN_X))
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
      	  // if(encountered)
	  //   {
      	  //     mvprintw(0, 0, "DIR_DOWN and encountered = true Coord = %s ", gamePlayer->getMaxYAbsAsStr().c_str());
	  //     nodelay(stdscr, FALSE);
	  //     refresh();
	  //     getch();
	  //     nodelay(stdscr, TRUE);
	  //     encountered = true;
	  //   }
      /* We use PLAYER_MOVEMENT_INNER_MARGIN.y here because we don't
	 support scrolling in the y dimension. If
	 PLAYER_MOVEMENT_INNER_MARGIN.y = 0 this point should not be
	 reached. */
      if(gamePlayer->inWindowInnerMargin
	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	  REACHED_INNER_MARGIN_X))
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
  if(gamePlayer->inWindowInnerMargin
     (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
      REACHED_INNER_MARGIN_X) &&
     gamePlayer->leftOfWindowInnerRightMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x, maxyx))
    { // We are to the left of the inner right margin.
      gamePlayer->updatePosRel(input);
    }
  else
    {
      if(gamePlayer->inWindowInnerMargin
	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	  REACHED_INNER_MARGIN_X) && (position + maxyx.x) < backgroundLength)
	{ // There is still background left to spare.
	  gamePlayer->updateDirection(input);
	  position++;	// Move background.
	}
      else
	{
	  if((position + maxyx.x) == backgroundLength &&
	     (gamePlayer->inWindowInnerMargin
	      (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	       REACHED_INNER_MARGIN_X)))
	    { /* No background left, so move the player to the right
		 edge of the background. */
	      gamePlayer->updatePosRel(input);
	    }
	}
    }
}


void rules::movePlayerLeftWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->inWindowInnerMargin(peekPos.y, peekPos.x,
				     PLAYER_MOVEMENT_INNER_MARGIN.y,
				     REACHED_INNER_MARGIN_X) &&
     gamePlayer->rightOfWindowInnerLeftMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x))
    { // We are to the righ of the inner left margin.
      gamePlayer->updatePosRel(input);
    }
  else
    {
      if(gamePlayer->inWindowInnerMargin
	 (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	  REACHED_INNER_MARGIN_X) && (position > 0))
	{ // There is still background left to spare.
	  gamePlayer->updateDirection(input);
	  position--;	// Move background.
	}
      else
	{
	  if(position == 0 &&
	     (gamePlayer->inWindowInnerMargin
	      (peekPos.y, peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.y,
	       REACHED_INNER_MARGIN_X)))
	    { /* No background left, so move the player to the left
		 edge of the background. */
	      gamePlayer->updatePosRel(input);
	    }
	}
    }
}


sprite::directions rules::handleGroundCollision(sprite::directions input,
						const int & position)
{
  sprite::directions retDir {input};
  for(std::string coord: gamePlayer->getBottomXAbsRangeAsStrs(position))
    {
      if(coordChars.find(coord) != coordChars.end())
	{
	  retDir = sprite::DIR_NONE;
	  gamePlayer->updateDirection(input);
	  break;
	}
    }
  return retDir;
}


sprite::directions rules::handleRightStepCollision(sprite::directions input,
						   const int & position)
{
}


void rules::physics
(const int input, int & position, const yx maxyx, const size_t backgroundLength,
 std::__1::chrono::steady_clock::time_point & secStartTime)
{
  const sprite::directions currDir {gamePlayer->getDirection()};
  if(player::isDirectionCharInputValid(input) && input != currDir)
    { 
      movePlayer(player::convertDirectionCharsToDirections
		 (player::directionChars(input)), position, maxyx,
		 backgroundLength);
    }
  else
    {	// Keep moving in the current direction.
      movePlayer(currDir, position, maxyx,
		 backgroundLength);
    }
  sleep(engineSleepTime);
  resetOldTime(secStartTime);
}
