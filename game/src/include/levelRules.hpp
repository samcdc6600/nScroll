#ifndef LEVELRULES_HPP_
#define LEVELRULES_HPP_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"
#include "backgroundSprite.hpp"
#include "background.hpp"
#include "chronological.hpp"


class rules: public chunk<char>
{
public:
  /* Used to provide a nicer error message if the members of gameTiming aren't
     initialised properly. */
  const std::string gameTimingErrorInfo {"rules::startTimers()"};
  // Used to handle all timing for the game.
  timers gameTiming {};
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
  
private:
  /* The character rules first stage buffer should be larger than 5x5 so that
     non player sprites can move a bit off of the screen without the worry that
     they will move into a rules chunk that is invalid. This way the non player
     sprites can move out of the view port and not mess up the game logic. */
  static const int FIRST_STAGE_BUFFER_DIMENSIONS_SIZE {7};
  /* The player cannot pass widthin this many character's of the window
     boarder's (y, x). This variable shouldn't be changed once it is set. */
  yx PLAYER_MOVEMENT_AREA_PADDING {};
  /* This variable shouldn't be changed once it is set.  */
  yx INITIAL_VIEW_PORT_COORDINATES {};
  // Contains position based rules for current view port position.
  chunkElementBaseType * secondStageRulesBuffer;
  
  // ==== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files START ====
  // ===========================================================================
  void loadAndInitialiseCoordRules(const yx expectedChunkSize,
				   const char coordRulesFileName [],
				   const backgroundData & background);
  void initialiseCoordRules
  (const yx expectedChunkSize, const char coordRulesFileName [],
   chunkMapType & coordRules, const std::string & coordRulesData,
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
  (const std::string & chunkIn, chunkType & rawChunk,
   const yx expectedChunkSize, const ssize_t chunksReadIn,
   const char coordRulesFileName[]) const;
  /* Checks if argument is an element of boarderRuleChars::CHARS (see utils.cpp).
     Or if the argument is a space character. If either of these is true then
     returns. Otherwise calls exit() */
  void checkRuleChar(const char potentialRule, const std::string eMsg) const;
  /* Make sure that for each key in coordRules there is a corresponding key in
     background and that coordRules and background have the same cardinality. */
  void verifyTotalOneToOneOntoMappingOfCoordToBgKeys
  (const chunkMapType & coordRules, const backgroundData & background) const;
  // ===== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files END =====
  // ===========================================================================
  
  // ==== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ===
  // ===========================================================================
  // Where rulesBuffer holds the contents of a RULES_CONFIG_FILE_EXTENSION file.
  void parseRulesConfigFileAndInitialiseVariables
  (const char rulesFileName [], const std::string & rulesBuffer,
   const backgroundData & background);
  /* Checks the validity of the initial values for the relative player position
     and padding. Exits with an error message if either of the values aren't
     valid. */
  void checkInitPlayerPosAndPadding();
  // ===== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files END ====
  // ===========================================================================

  
  // Updates rule characters buffers.
  void updateBuffers()
  {
    updateFirstStageBuffer();
    updateSecondStageBuffer(secondStageRulesBuffer);
  }

  
  // check level against sprite
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
  // /* No functions that change the position of the player should be called after
  //    this one for a given frame. */
  // /* Calls handleFinalPlayerMovementAndWindowAndPaddingInteractions after
  //    checking that the player won't go outside of the level after it's position
  //    is updated to peekPos. */
  // void handleFinalPlayerMovementAndWindowAndPaddingInteractionsSafe
  // (const sprite::directions newDir);
  // void handleFinalPlayerMovement(const sprite::directions newDir);
  /* Moves the player when interacting with the inner padding (NOTE THAT THIS
     FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
     THE INNER PADDING AND NOT OUTSIDE THE BOUNDS OF THE LEVEL AS DEFINED BY THE
     BACKGROUND.) */
  // void movePlayerWhenInteractingWithInnerPadding
  // (const sprite::directions input, const yx peekPos);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER PADDING */
  // void movePlayerRightWhenInteractingWithInnerPadding
  // (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
  //  const size_t backgroundLength, const yx peekPos,
  //  const int REACHED_INNER_PADDING_X);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER PADDING */
  // void movePlayerLeftWhenInteractingWithInnerPadding
  // (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
  //  const size_t backgroundLength, const yx peekPos,
  //  const int REACHED_INNER_PADDING_X);
  // /* Returns number of characters untill the player bits a boarder character
  //    (if moving up). Will return -1 if there are no boarder characters above */
  // int getClosestBoarderCharAbove(const int position, const bool directContact);
  /* Handles collision with boarder characters when the player is moving down.
     Returns updated direction. */
  sprite::directions handleGroundCollision();
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character. The player sprite will be stopped if there is more then one
     character directly to the right of it. */
  sprite::directions handleRightCollision();
  // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision();

#ifdef DEBUG
  void printRuleChars();
#endif

public:
  rules
  (const yx viewPortSize, const char coordRulesFileName [],
   const chunk::chunkElementBaseType missingChunkFiller,
   const char rulesFileName [], const backgroundData & background):
    chunk(viewPortSize, FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
	  missingChunkFiller, coordRulesFileName),
    secondStageRulesBuffer
    (new chunkElementBaseType [viewPortSize.y * viewPortSize.x])
  {
    std::string rulesBuffer {};
    loadAndInitialiseCoordRules(viewPortSize, coordRulesFileName, background);
    loadFileIntoString
      (rulesFileName, rulesBuffer,
       concat("trying to read ", RULES_CONFIG_FILE_EXTENSION, " file"));
    parseRulesConfigFileAndInitialiseVariables
      (rulesFileName, rulesBuffer, background);
    checkInitPlayerPosAndPadding();


    // endwin();
    // std::cout<<"\nINITIAL_REL_VIEW_PORT_COORDINATES = "<<INITIAL_REL_VIEW_PORT_COORDINATES
    //   <<"\nPLAYER_MOVEMENT_AREA_PADDING = "<<PLAYER_MOVEMENT_AREA_PADDING
    // 	     <<"\ngamePlayer->getPos() = "<<gamePlayer->getPos()<<'\n';
    // exit(-1);

  
    // Initialise game timers.
    /* NOTE THAT WE HAVE HARD CODED THESE VALUES HERE FOR NOW, BUT WE INTEND TO
       HAVE AT LEAST SOME OF THEM LOADED FROM RULES.LEV FILES. */
    //gameTiming.movePlayer = chronological{22.2, gameTimingErrorInfo};
    // gameTiming.movePlayer = chronological{27.3, gameTimingErrorInfo};
    gameTiming.movePlayer = chronological{25, gameTimingErrorInfo};
    // 8.3333 ~ 120 FPS
    gameTiming.drawTime = chronological{8.3333, gameTimingErrorInfo};
  }

  
  ~rules()
  {
    delete [] secondStageRulesBuffer;
    delete gamePlayer;
  }


  yx getPlayerMovementAreaPadding() {return PLAYER_MOVEMENT_AREA_PADDING;}
  yx getInitialViewPortPosition() {return INITIAL_VIEW_PORT_COORDINATES;}
  // Initialises all timers used for game timing.
  void startTimers();
  void physics
  (backgroundData & background, const sprite::directions input);
};


#endif
