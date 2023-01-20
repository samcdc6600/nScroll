#include "include/levelRules.hpp"
#include "include/player.hpp"
#include <string>


/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places than this. */
constexpr int MAX_COORD_LEN{10};


rules::coordRulesType rules::loadAndInitialiseCoordRules
(const yx expectedChunkSize, const backgroundData & background,
 const char bgFileName [], const char coordRulesFileName [])
{
  std::string rawCoordRules {};
  coordRulesType coordRules;
  
  if(!loadFileIntoString(coordRulesFileName, rawCoordRules))
    {
      std::stringstream e {};
      e<<"Error: unable to open \""<<coordRulesFileName<<"\".";
      exit(e.str(), ERROR_OPENING_FILE);
    }

  // parseRulesHeader(expectedChunkSize, coordRulesFileName, levelRules, bgSize, rawCoordRules, buffPos);
  initialiseCoordRules
    (expectedChunkSize, background, bgFileName, coordRulesFileName, coordRules,
     rawCoordRules);
  return coordRules;
}


void rules::initialiseCoordRules
(const yx expectedChunkSize, const backgroundData & background, const char bgFileName [],
 const char coordRulesFileName [], rules::coordRulesType & coordRuless,
 const std::string & coordRulesData)
{
  // Main body of rules file should start on a new line (account for if.)
  // std::string::const_iterator buffPos {coordRulesData.begin()};
  // const size_t expectedLineLength {bgSize / expectedChunkSize.y};
  // int lineNumber {};
  // int lineLength {};
  yx chunkCoord {};
  std::string chunk {};
  coordRulesChunk rawChunk {};
  std::string::const_iterator buffPos {std::begin(coordRulesData)};
  ssize_t chunksReadIn {};

  // Extract chunks from coordRulesData ========================================
  while(true)
    {
      /* Each chunk should have a header that contains it's coordinates
	 in the level. The unit of the coordinates should be chunks. So
	 for a chunk that starts at (0, 170) in character coordinates
	 (assuming the size of chunks for this file are 170 in the x
	 dimension), the coordinate in the header would be (0, 1) and
	 for (0, 340) it would be (0, 2), etc... */
      if(getChunkCoordinate(coordRulesData, buffPos,
			    concat("trying  to read coord no. ", chunksReadIn,
				   " from coordRules.lev file \"",
				   coordRulesFileName, "\""), chunkCoord))
	{
	  skipSpaceUpToNextLine
	    (coordRulesData, buffPos,
	     concat("Error: trying to read chunk no. ", chunksReadIn,
		    ". Expected '\\n' after reading chunk coordinate ",
		    "from coordRules.lev file \"", coordRulesFileName, "\". "
		    "Encountered other character or EOF."));
	  // Get chunk from coord rules read from file.
	  getChunk(coordRulesData, buffPos,
		   concat("Error: reading in chunk no. ", chunksReadIn,
			  ". from coordRules.lev file \"", coordRulesFileName,
			  "\"."), chunk, expectedChunkSize);
	  // Decompress chunk and return in rawChunk.
	  decompressChunk
	    (chunk, rawChunk, expectedChunkSize, chunksReadIn,
	     coordRulesFileName);
	  // Verify decompressed size...
	  // Insert chunk into...

	  chunksReadIn++;
	  // GetChunk() clears it's argument (chunk).
	  rawChunk.clear();
	}
      else
	{
	  break;
	}
    }

  // Decompress chunks =========================================================
  
  /*  while(buffPos < rawCoordRules.end())
    {
      if(*buffPos == levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR)
	{
	  // We've encountered a run-length encoding character.
	  const char ruleChar {*(++buffPos)};
	  ++buffPos;

	  checkRuleChar
	    (ruleChar,
	     concat("parsing main section of \"", coordRulesFileName,
		    "\" (on line ", lineNumber, " of main section)"));

	  const int runLength
	    {readSingleNum
	     (rawCoordRules, buffPos,
	      concat("reading run-length encoding length as a result of "
		     "encountering run-length encoding character \"",
		     levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR,
		     "\" while parsing main section of \"", coordRulesFileName,
		     "\""), false)};
	  --buffPos;

	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
	    {
	      coordRuless.coordRules.push_back(ruleChar);
	    }
	  lineLength += runLength;
	}
      else
	{
	  checkRuleChar(*buffPos,
			concat("parsing main section of \"",
			       coordRulesFileName,  "\" (on line ", lineNumber,
			       " of main section)"));
	  coordRuless.coordRules.push_back(*buffPos);
	  lineLength++;
	}
      
      buffPos++;
      if(*buffPos == '\n')
	{
	  if(size_t(lineLength) != expectedLineLength)
	    {
	      std::stringstream e {};
	      e<<"Error: reading rules.lev header file \""<<coordRulesFileName
	       <<"\". Encountered line of length ("<<lineLength<<") (on line "
	       <<lineNumber<<" of main section) when reading main section of "
		"file. Expected a line of length ("<<expectedLineLength<<").";

	      exit(e.str().c_str(), ERROR_BACKGROUND);
	    }
	  lineLength = 0;
	  lineNumber++;
	  buffPos++;
	}
    }

  if(size_t(lineLength) != expectedLineLength)
    {
      std::stringstream e {};
      e<<"Error: reading rules.lev header file \""<<coordRulesFileName
       <<"\". Encountered line of length ("<<lineLength<<") (on line "
       <<lineNumber<<") when reading main section of file. Expected a line "
	"of length ("<<expectedLineLength<<").";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }

  if(coordRuless.coordRules.size() != bgSize)
    {
      std::stringstream e {};
      e << "Error: reading rules.lev header file \"" << coordRulesFileName
	<< "\". Size ("<<coordRuless.coordRules.size()<<") of main section of "
	"file not equal to size ("<<bgSize<<") of background file \""
	<<bgFileName<<"\".";
      exit(e.str().c_str(), ERROR_BACKGROUND);
      }*/
}


void rules::decompressChunk
(const std::string & chunkIn, coordRulesChunk & rawChunk,
 const yx expectedChunkSize, const ssize_t chunksReadIn,
 const char coordRulesFileName[])
{
  const char runLenBeginChar
    {levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR};
  std::string::const_iterator buffPos {std::begin(chunkIn)};
  int lineNumber {};
  int lineLength {};
    
  while(buffPos < chunkIn.end())
    {
      if(*buffPos == runLenBeginChar)
	{
	  // We've encountered a run-length encoding character.
	  const char ruleChar {*(++buffPos)};
	  ++buffPos;

	  checkRuleChar
	    (ruleChar,
	     concat("decompressing chunk no. ", chunksReadIn, " from file \"",
		    coordRulesFileName, "\" (on line ", lineNumber,
		    " of chunk.) "));

	  const int runLength
	    {readSingleNum
	     (chunkIn, buffPos,
	      concat("reading run-length encoding length as a result of "
		     "encountering run-length encoding character \"",
		     runLenBeginChar, "\" while  decompressing chunk no. ",
		     chunksReadIn, " from file \"",
		     coordRulesFileName, "\" (on line ", lineNumber,
		     " of chunk.) "), false)};
	  --buffPos;

	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
	    {
	      rawChunk.push_back(ruleChar);
	    }
	  lineLength += runLength;
	}
      else
	{
	  checkRuleChar
	    (*buffPos,
	     concat("decompressing chunk no. ", chunksReadIn, " from file \"",
		    coordRulesFileName, "\" (on line ", lineNumber,
		    " of chunk.) "));
	  rawChunk.push_back(*buffPos);
	  lineLength++;
	}
      
      buffPos++;
      if(*buffPos == '\n')
	{
	  if(size_t(lineLength) != expectedChunkSize.x)
	    {
	      std::stringstream e {};
	      e<<"Error: reading rules.lev header file \""<<coordRulesFileName
	       <<"\". Encountered line of length ("<<lineLength<<") when "
		"decompressing chunk no. "<<chunksReadIn<<" (on line "
	       <<lineNumber<<" of chunk.) Expected a line of length ("
	       <<expectedChunkSize.x<<").";
	      exit(e.str().c_str(), ERROR_BACKGROUND);
	    }
	  lineLength = 0;
	  lineNumber++;
	  buffPos++;
	}
    }

  if(size_t(lineLength) != expectedChunkSize.x)
    {
      std::stringstream e {};
      e<<"Error: reading rules.lev header file \""<<coordRulesFileName
       <<"\". Encountered line of length ("<<lineLength<<") when "
	"decompressing chunk no. "<<chunksReadIn<<" (on line "
       <<lineNumber<<" of chunk.) Expected a line of length ("
       <<expectedChunkSize.x<<").";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }

  const ssize_t expectedChunkSizeLinear
    {expectedChunkSize.y * expectedChunkSize.x};
  if(rawChunk.size() != expectedChunkSizeLinear)
    {
      std::stringstream e {};
      e<<"Error: reading rules.lev header file \""<<coordRulesFileName
       <<"\".  Size ("<<rawChunk.size()<<") of chunk no. "<<chunksReadIn<<" not"
	" equal to expected size ("<<expectedChunkSizeLinear<<") of chunk.";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }
}


void checkRuleChar(const char potentialRule, const std::string eMsg)
{
  for(char rule: boarderRuleChars::CHARS)
    {
      if(potentialRule == rule || potentialRule == ' ')
	{
	  return;
	}
    }
  std::stringstream e {};
  e<<"Error: found that character \""<<potentialRule<<"\" is not a space, new "
    "line, \""<<levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR<<"\" or in "
    "the set of rule characters (";
  for(char rule: boarderRuleChars::CHARS)
    {
      e<<rule<<", ";
    }
  e<<") when "<<eMsg<<'.';
  exit(e.str().c_str(), ERROR_CHARACTER_RANGE);
}


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
  
  if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
     )
    {
      // Start jump.
      //      gamePlayer->startJumping(position, maxyx, coordRulesCurrentContextBuffer);
      // We wan't to keep moving in the direction we were moving in before.
      input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
      //      gamePlayer->handleJumpingAndFalling(position, maxyx, coordRulesCurrentContextBuffer);
    }

  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      input = handleRightCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      input = handleLeftCollision(position, maxyx.y);
    }

  handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
    (input, position, maxyx, backgroundLength);
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength)
{
  const yx peekPos {gamePlayer->peekAtPos(newDir)};
  if(gamePlayer->inLevelY(peekPos.y, maxyx.y) &&
     gamePlayer->inLevelX(peekPos.x + position, backgroundLength))
    {
      // We won't be outside of the level if we move.
      handleFinalPlayerMovementAndWindowAndMarginInteractions
	(newDir, position, maxyx, backgroundLength, peekPos);
    }
  else
    {
      gamePlayer->updateDirection(sprite::DIR_NONE);
    }
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractions
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
    /* Make any final movement, check for window margin contact and take
     appropriate action if such contact is made. */
  if(((newDir == sprite::DIR_LEFT || newDir == sprite::DIR_RIGHT) &&
      gamePlayer->notInWindowInnerMarginX(peekPos.x,
					  PLAYER_MOVEMENT_INNER_MARGIN.x)) ||
     newDir == sprite::DIR_NONE)	// Only need to check for DIR_NONE here.
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else if((newDir == sprite::DIR_DOWN || newDir == sprite::DIR_UP)
	  && gamePlayer->notInWindowInnerMarginY(peekPos.y,
						 PLAYER_MOVEMENT_INNER_MARGIN.y))
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else
    {
      movePlayerWhenInteractingWithInnerMargin(newDir, position, maxyx,
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
      gamePlayer->updatePosRel(input);
      break;
    case sprite::DIR_RIGHT:
      movePlayerRightWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    case sprite::DIR_DOWN:
      gamePlayer->updatePosRel(input);
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
  if(gamePlayer->leftOfWindowInnerRightMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x, maxyx))
    { // We are to the left of the inner right margin.
      gamePlayer->updatePosRel(input);
    }
  else if(size_t(position + maxyx.x) < backgroundLength)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position++;	// Move background.
    }
  else if(size_t(position + maxyx.x) == backgroundLength)
    { /* No background left, so move the player to the right
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


void rules::movePlayerLeftWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->rightOfWindowInnerLeftMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x))
    { // We are to the righ of the inner left margin.
      gamePlayer->updatePosRel(input);
    }
  else if(position > 0)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position--;	// Move background.
    }
  else if(position == 0)
    { /* No background left, so move the player to the left
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


// int rules::getClosestBoarderCharAbove(const int position,
// 				      const bool directContact)
// {
//   int minBoarderCharDistanceRet {0};
//   int yPos {};
    
//   std::vector<std::string> spriteTopCoords
//     {gamePlayer->getTopXAbsRangeAsStrsForOneOffContact(position)};

//   // All coordinates from spriteTopCoords and above.
//   std::vector<std::string> spriteTopCoordsColumn {};

//   for(std::string strCoord: spriteTopCoords)
//     {
//       std::stringstream coord {strCoord};
//       int spCoordY, spCoordX;
//       char eat;
//       coord>>spCoordY;
//       coord>>eat;
//       coord>>spCoordX;

//       yPos = spCoordY;

//       for(int y {spCoordY}; y >= 0; y--)
// 	{
// 	  std::stringstream newCoord {};
// 	  newCoord<<y;
// 	  newCoord<<',';
// 	  newCoord<<spCoordX;
// 	  spriteTopCoordsColumn.push_back(newCoord.str());
// 	}
//     }


//   bool foundBoarderChar {false};
//   for(std::string coord: spriteTopCoordsColumn)
//     {
//       if(coordChars.find(coord) != coordChars.end())
// 	{
// 	  foundBoarderChar = true;
// 	  std::stringstream boarderCoord {coord};
// 	  int boarderY {};
// 	  boarderCoord>>boarderY;
	  
// 	  minBoarderCharDistanceRet =
// 	    boarderY > minBoarderCharDistanceRet ? (yPos - boarderY):
// 	    (yPos - minBoarderCharDistanceRet);
// 	}
//       mvprintw(0, 0, "%d", minBoarderCharDistanceRet);
//     }

//   // for(std::string strCoord: spriteTopCoordsColumn)
//   //   {
//   //     std::stringstream coord {strCoord};
//   //     int y, x;
//   //     char eat;

//   //     coord>>y;
//   //     coord>>eat;
//   //     coord>>x;

//   //     if((x - position) < 250)
//   // 	{
//   // 	  mvprintw(y, (x - position), "@");
//   // 	  refresh();
//   // 	}
//   //     coord.clear();
//   //   }

//   return foundBoarderChar ? minBoarderCharDistanceRet: -1;
// }


/* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
sprite::directions rules::handleGroundCollision(const int position,
						const int backgroundHeight)
{
  sprite::directions retDir {sprite::DIR_DOWN};
  // for(yx coord:
  // 	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact(position))
  //   {
  //     char rule {};
  //     if(getCoordRule(coord, coordRules, backgroundHeight, rule) &&
  // 	 (rule == boarderRuleChars::BOARDER_CHAR ||
  // 	  rule == boarderRuleChars::PLATFORM_CHAR))
  // 	{
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
sprite::directions rules::handleRightCollision(const int position,
					       const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_RIGHT};
  // const std::vector<yx> playerCoords
  //   {gamePlayer->getRightYAbsRangeAsStrsForOneOffContact(position)};
  // const yx bottomRightPlayerCoord
  //   {playerCoords[playerCoords.size() -1]};
  // bool stoppingContact {false};

  // char rule {};
  // for(yx playerCoord: playerCoords)
  //   {
  //     // If there is near contact and it's not with the bottom right coord.
  //     if(playerCoord != bottomRightPlayerCoord &&
  // 	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
  // 	 rule == BOARDER_CHAR)
  // 	{
  // 	  stoppingContact = true;
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }
  
  // if(!stoppingContact &&
  //    getCoordRule(bottomRightPlayerCoord, coordRules, backgroundHeight, rule) &&
  //    (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
  //   {
  //     if(gamePlayer->getPos().y > 0)
  // 	{
  // 	  /* If we've hit a "step" (as in the things that constitute staircases)
  // 	     and we are not at the minimum (top of window) y position, then
  // 	     "step up" :). */
  // 	  gamePlayer->updatePosRel(sprite::DIR_UP);
  // 	}
  //     else
  // 	{
  // 	  // We've hit the top of the window.
  // 	  retDir = sprite::DIR_NONE;
  // 	}
  //   }
  
  return retDir;
}


sprite::directions rules::handleLeftCollision(const int position,
					      const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_LEFT};
  // const std::vector<yx> playerCoords
  //   {gamePlayer->getLeftYAbsRangeAsStrsForOneOffContact(position)};
  // const yx bottomLeftPlayerCoord
  //   {playerCoords[playerCoords.size() -1]};
  // bool stoppingContact {false};

  // char rule {};
  // for(yx playerCoord: playerCoords)
  //   {
  //     // If there is near contact and it's not with the bottom right coord.
  //     if(playerCoord != bottomLeftPlayerCoord &&
  // 	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
  // 	 rule == BOARDER_CHAR)
  // 	{
  // 	  stoppingContact = true;
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }

  // if(!stoppingContact &&
  //    getCoordRule(bottomLeftPlayerCoord, coordRules, backgroundHeight, rule) &&
  //    (rule == BOARDER_CHAR ||
  //     rule == PLATFORM_CHAR))
  //   {
  //     if(gamePlayer->getPos().y > 0)
  // 	{
  // 	  /* If we've hit a "step" and we are not at the minimum (top of window)
  // 	     y position, then "step up" :) */
  // 	  gamePlayer->updatePosRel(sprite::DIR_UP);
  // 	}
  //     else
  // 	{
  // 	  // We've hit the top of the window.
  // 	  retDir = sprite::DIR_NONE;
  // 	}
  //   }

  return retDir;
}


#ifdef DEBUG
#include <sstream>

void rules::printRuleChars(const int position, const int maxY, const int maxX)
{
  for(int y {}; y < maxY; ++y)
    {
      for(int x {}; x < (coordRules.size() / maxY); ++x)
	{
	  char coordRule;
	  getCoordRule(y, x, coordRules, maxY, coordRule);
	  if(coordRule != ' ' && (x - position) < maxX)
	    {
	      mvprintw(y, (x - position), (std::string {coordRule}).c_str());
	    }
	}
    }
  refresh();
}
#endif


void rules::physics
(const player::directionChars input, int & position, const yx maxyx,
 const size_t backgroundLength,
 std::__1::chrono::steady_clock::time_point & secStartTime)
{
#ifdef DEBUG
  printRuleChars(position, maxyx.y, maxyx.x);
#endif
  
  movePlayer(player::convertDirectionCharsToDirections(input), position, maxyx,
	     backgroundLength);
  sleep(engineSleepTime);
  resetOldTime(secStartTime);
}
