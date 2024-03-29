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
  // Used to handle timings for the main loop of the game.
  timers gameTiming {};
  //For sprites (holds sprite data (slices) and the rule for the sprite.)
  struct spriteInfo
  { // Sprite data to go here (when I do it.)
    sprite * sp {}; // This has to be a pointer because sprite is an ABC.
    std::string rule {};
  };
  // /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  // std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer; /* Info about user controlled sprite (AKA
			  player.) */
  /* Key is the chunk position of the sprite (i.e a sprite with
     position 33, 169 would have a key  "(0,0)"). Since each key must
     be unique we store sprites that are in the same chunk in a
     vector. */
  std::map<std::string, std::vector<bgSprite *>> bgSprites;
  /* This var is used in the initialisation of the player and other sprites in
     the rules.lev file parsing code in levelRules.cpp. It is also used to
     calculate how much memory to allocate for the second stage rules
     buffer. */
  const yx firstStageBufferSizeInChars;
  
private:
  /* THE PHYSICS ENGINE SHOULD BE RUN THIS OFTEN (IN REAL SYSTEM TIME.) The
     sprites position should be updated by 1 character in any direction at
     most this often. This is in seconds. */
  const long double physicsTickInterval {1 / 120.0};
  /* ANYTHING TIMING DONE WITHIN THE PHYSICS ENGINE SHOULD USE THIS TIME (THIS
     IS BECAUSE THE PHYSICS ENGINE MAY BE UPDATED AT A SIGNIFICANTLY SLOWER RATE
     THAN MASTERPHYSICSTIME IF THE COMPUTER SYSTEM IS NOT FAST ENOUGH, BECAUSE
     OF THIS WE MUST USE A FIXED VALUE SO THE PHYSICS ENGINE'S CALCULATIONS
     AREN'T EFFECTED. LOOK INTO FIXED TIME STEP FOR MORE INFO. */
  const long double fixedTimeStep {physicsTickInterval};
  /* The character rules first stage buffer should be larger than 5x5 so that
     non player sprites can move a bit off of the screen without the worry that
     they will move into a rules chunk that is invalid. This way the non player
     sprites can move out of the view port and not mess up the game logic. */
  static const int FIRST_STAGE_BUFFER_DIMENSIONS_SIZE {7};
  const char cRRunLengthSequenceSignifier {'R'};
  /* The player cannot pass widthin this many character's of the window
     boarder's (y, x). This variable shouldn't be changed once it is set. */
  yx PLAYER_MOVEMENT_AREA_PADDING {};
  /* This variable shouldn't be changed once it is set.  */
  yx INITIAL_VIEW_PORT_COORDINATES {};
  // Contains position based rules for current view port position.
  chunkElementBaseType * secondStageRulesBuffer;
  
  // ==== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files START ====
  // ===========================================================================
  void loadAndInitialiseCoordRules(const backgroundData & background);
  void initialiseCoordRules
  (std::fstream & cRFile, chunkMapType & coordRules,
   const backgroundData & background) const;
  /* NOTE THAT IF A THE LAST CHUNK IN THE FILE IS READ FILE.FAIL() WILL BE TRUE
     AND WILL NEED TO BE RESET IF MORE OPERATIONS (AT LEAST FOR SOME POSSIBLE
     OPERATIONS) NEED TO BE DONE ON FSTREAM. */
  bool getCRChunk
  (std::fstream & file, std::vector<chunkElementBaseType> & chunk,
   const yx chunkSize, yx & chunkCoord, const std::string & fileName,
   const bool multiChunkFile = false) const;
  /* Make sure that for each key in coordRules there is a corresponding key in
     background and that coordRules and background have the same cardinality. */
  void verifyTotalOneToOneOntoMappingOfCoordToBgKeys
  (const chunkMapType & coordRules, const backgroundData & background) const;
  // ===== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files END =====
  // ===========================================================================
  
  // ==== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ===
  // ===========================================================================
  /* Removes comments from rulesBuffer and calls
     parseRulesConfigFileAndInitialiseVariablesStageTwo(). Where rulesBuffer
     holds the contents of a RULES_CONFIG_FILE_EXTENSION file. */
  void parseRulesConfigFileAndInitialiseVariablesStageOne
  (const char rulesFileName [], std::string & rulesBuffer,
   const backgroundData & background);
  /* Where rulesBuffer holds the contents of a RULES_CONFIG_FILE_EXTENSION file
     that has no comments. */
  void parseRulesConfigFileAndInitialiseVariablesStageTwo
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

  
  // // check level against sprite
  // char intersection(const std::string & boundsInteraction,
  // 		    const std::vector<int> spChoords);
  // // check player against sprite
  // char intersection(const std::vector<int> playerSpChoords,
  // 		    const std::vector<int> spChoords);
  // // check level against sprite
  // char nearPass(const std::string & boundsInteraction,
  // 		const std::vector<int> spChoords);
  // // check player against sprite
  // char nearPass(const std::vector<int> playerSpChoords,
  // 		const std::vector<int> spChoords);
  // // Moves the player 
  // void movePlayer
  // (backgroundData & background, sprite::directions input,
  //  const double timeElapsed);
  
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
  // /* Handles collision with boarder characters when the player is moving down.
  //    Returns updated direction. */
  // sprite::directions handleGroundCollision();
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character. The player sprite will be stopped if there is more then one
     character directly to the right of it. */
  sprite::directions handleRightCollision();
  // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision();

public:
#ifdef DEBUG
  /* Will print rules chars for printInterval and then won't print them for
     printInterval. Then the cycle is repeated. This way both rules chars and bg
     chars / sprite chars can be seen when debugging. The unit for printInterval
     is ms.  */
  void printRuleChars(const long double printInterval);
#endif
  rules
  (const yx viewPortSize, const char coordRulesFileName [],
   const chunk::chunkElementBaseType missingChunkFiller,
   const char rulesFileName [], const backgroundData & background):
    chunk(viewPortSize, FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
	  missingChunkFiller, coordRulesFileName, "calling chunk constructor "
	  "before rules constructor body"),
    firstStageBufferSizeInChars
    (chunk::secondStageBufferSizeInChunks * viewPortSize.y,
     chunk::secondStageBufferSizeInChunks * viewPortSize.x),
    secondStageRulesBuffer
    (new chunkElementBaseType
     [firstStageBufferSizeInChars.y * firstStageBufferSizeInChars.x])
  {
    std::string rulesBuffer {};

    loadAndInitialiseCoordRules(background);
    loadFileIntoString
      (rulesFileName, rulesBuffer,
       concat("trying to read ", RULES_CONFIG_FILE_EXTENSION, " file"));
    parseRulesConfigFileAndInitialiseVariablesStageOne
      (rulesFileName, rulesBuffer, background);
    checkInitPlayerPosAndPadding();

    gameTiming.physics =
      chronological{physicsTickInterval, gameTimingErrorInfo};
  }

  
  ~rules()
  {
    delete [] secondStageRulesBuffer;
    delete gamePlayer;
    for (const auto & bgSpritesPair: bgSprites)
      {
	const std::vector<bgSprite *> & bgSprites =
	  bgSpritesPair.second;
	for(auto bgSprite: bgSprites)
	  {
	    delete bgSprite;
	  }
      }
  }


  void initBuffers(const yx initialViewPortPos)
  {
    chunk::initBuffers(initialViewPortPos, secondStageRulesBuffer);
  }


  yx getPlayerMovementAreaPadding() {return PLAYER_MOVEMENT_AREA_PADDING;}
  yx getInitialViewPortPosition() {return INITIAL_VIEW_PORT_COORDINATES;}
  // Initialises all timers used for game timing.
  void startTimers();
  void physics
  (backgroundData & background, const sprite::directions input);
};


#endif
