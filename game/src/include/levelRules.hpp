#ifndef LEVELRULES_HPP_
#define LEVELRULES_HPP_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"
#include "backgroundSprite.hpp"
#include "background.hpp"


class rules
{
public:
  /* This type is used for storing the the rules that correspond to a background
     chunk from the background class. */
  typedef std::vector<char> coordRulesChunk;
  
  // The player cannot pass widthin this many character's of the window boarder's (y, x).
  const yx PLAYER_MOVEMENT_AREA_PADDING {11, 52};
  static constexpr size_t second {1};
  // Contains position based rules.
  
  //  	std::vector<char> coordRules {}; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //For sprites (holds sprite data (slices) and the rule for the sprite.)
  struct spriteInfo
  { // Sprite data to go here (when I do it.)
    sprite * sp {}; // This has to be a pointer because sprite is an ABC.
    std::string rule {};
  };
  // /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  // std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer; /* Info about user controlled sprite (AKA player.) */
  std::vector<bgSprite *> bgSprites;
  /* The player cannot pass widthin this many character's of the left and right
     window boarders (y, x). */
  const yx playerMovementInnerLRBoarder {0, 44};
  
private:
  typedef std::map<std::string, coordRulesChunk> coordRulesType;
  const coordRulesType coordRules;
  const ssize_t coordRulesCurrentContextBufferSize;
  /* This is equivalent to the first stage draw buffer in
     backgroundData. Coord rule chunks are copied into this buffer based on the
     players position (according to exactly the same rules as those followed in
     backgroundData.) This array can then be indexed into (after a simple
     calculation) to get the character rule (if any) for a given position on the
     screen. */ 
  char * coordRulesCurrentContextBuffer;
  // const size_t millisecondsInSec {1000*second};
  // The duration of time we call sleep() for (in milliseconds.)
  const size_t engineSleepTime {32};
  // const double sleepTimeAsAPercentageOfASecond {double(engineSleepTime / millisecondsInSec)};

  // ==== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files START ====
  // ===========================================================================
  coordRulesType loadAndInitialiseCoordRules
  (const yx expectedChunkSize, const char coordRulesFileName [],
   const backgroundData & background) const;
  void initialiseCoordRules
  (const yx expectedChunkSize, const char coordRulesFileName [],
   rules::coordRulesType & coordRules, const std::string & coordRulesData,
   const backgroundData & background) const;
  /* Attempts to decompress chunk in chunkIn. If successful returns
     decompressed chunk via rawChunk. ChunkIn is assumed to be compressed using
     the run length encoding technique.
     LevelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR signifies the start of
     a "run". The character after that is repeated a number of times in
     rawChunk equal to the number after that character. The number should be in
     base 10 in ASCII. The end of the number is non-ambiguous because numbers
     are not valid rule characters in a RULES_CONFIG_FILE_EXTENSION file. If
     there is an  error an error message is printed and the program is
     terminated. */
  void decompressChunk
  (const std::string & chunkIn, coordRulesChunk & rawChunk,
   const yx expectedChunkSize, const ssize_t chunksReadIn,
   const char coordRulesFileName[]) const;
  /* Checks if argument is an element of boarderRuleChars::CHARS (see utils.cpp).
     Or if the argument is a space character. If either of these is true then
     returns. Otherwise calls exit() */
  void checkRuleChar(const char potentialRule, const std::string eMsg) const;
  /* Make sure that for each key in coordRules there is a corresponding key in
     background and that coordRules and background have the same cardinality. */
  void verifyTotalOneToOneOntoMappingOfCoordToBgKeys
  (const char coordRulesFileName [], const coordRulesType & coordRules,
   const backgroundData & background) const;
  // ===== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files END =====
  // ===========================================================================
  
  // ==== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ===
  // ===========================================================================
  // Where rulesBuffer holds the contents of a RULES_CONFIG_FILE_EXTENSION file.
  void parseRulesConfigFileAndInitialiseVariables
  (const yx viewPortSize, const char rulesFileName [],
   const std::string & rulesBuffer, const backgroundData & background);
  // ===== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files END ====
  // ===========================================================================
  
  /* Set's oldTime to the current time if
     (oldTime - (the current time) >= second). */
  void resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime);
  // check level agains sprite
  char intersection(const std::string & boundsInteraction,
		    const std::vector<int> spChoords);
  // check player against sprite
  char intersection(const std::vector<int> playerSpChoords,
		    const std::vector<int> spChoords);
  // check level against sprite
  char nearPass(const std::string & boundsInteraction,
		const std::vector<int> spChoords);
  // check player against sprite
  char nearPass(const std::vector<int> playerSpChoords,
		const std::vector<int> spChoords);
  // Moves the player 
  void movePlayer
  (backgroundData & background, sprite::directions input);
  /* No functions that change the position of the player should be called after
     this one for a given frame. */
  /* Calls handleFinalPlayerMovementAndWindowAndMarginInteractions after
     checking that the player won't go outside of the level after it's position
     is updated to peekPos. */
  void handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
  (const sprite::directions newDir);
  void handleFinalPlayerMovementAndWindowAndMarginInteractions
  (const sprite::directions newDir, yx & viewPortPosition, const yx viewPortSize,
   const size_t backgroundLength, const yx peekPos);
  /* Moves the player when interacting with the inner margin (NOTE THAT THIS
     FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
     THE INNER MARGIN AND NOT OUTSIDE THE BOUNDS OF THE LEVEL AS DEFINED BY THE
     BACKGROUND.) */
  void movePlayerWhenInteractingWithInnerMargin
  (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
   const size_t backgroundLength, const yx peekPos);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerRightWhenInteractingWithInnerMargin
  (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerLeftWhenInteractingWithInnerMargin
  (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  // /* Returns number of characters untill the player bits a boarder character
  //    (if moving up). Will return -1 if there are no boarder characters above */
  // int getClosestBoarderCharAbove(const int position, const bool directContact);
  /* Handles collision with boarder characters when the player is moving down
     Input should be the player input direction and the current absolute
     position of the player sprite in the x dimension. Returns updated
     direction. */
  sprite::directions handleGroundCollision(const yx viewPortPosition,
					   const int backgroundHeight);
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character unless the player has a y position of 0, in which case the
     player will be stopped. The player sprite will also be stopped if there is
     more then one character directly to the right of it. */
  sprite::directions handleRightCollision(const yx viewPortPosition,
					  const int backgroundHeight);
  // // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision(const yx viewPortPosition,
					 const int backgroundHeight);

#ifdef DEBUG
  // Where maxX should be the maximum size of the window and not the level.
  void printRuleChars(const int position, const int maxY, const int maxX);
#endif

public:
  /* background is required because for every chunk in background there should
     be a corresponding chunk in rules::coordRules. */
  rules
  (const yx viewPortSize, const char coordRulesFileName [],
   const char rulesFileName [], const backgroundData & background) :
    coordRules
    (loadAndInitialiseCoordRules(viewPortSize, coordRulesFileName, background)),
    coordRulesCurrentContextBufferSize(viewPortSize.y * viewPortSize.x * 9),
    coordRulesCurrentContextBuffer(new char [viewPortSize.y * viewPortSize.x * 9])
  {
    std::string rulesBuffer {};
    loadFileIntoString
      (rulesFileName, rulesBuffer,
       concat("trying to read ", RULES_CONFIG_FILE_EXTENSION, " file"));
    parseRulesConfigFileAndInitialiseVariables
      (viewPortSize, rulesFileName, rulesBuffer, background);
  }
  void physics
  (backgroundData & background, const sprite::directions input,
   std::__1::chrono::steady_clock::time_point & secStartTime);
  ~rules()
  {
    delete gamePlayer;
    delete [] coordRulesCurrentContextBuffer;
  }
};


#endif
