#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"
#include "backgroundSprite.hpp"
#include "background.hpp"
#include "loadAssets.hpp"


class rules
{
public:
  /* This type is used for storing the the rules that correspond to a background
     chunk from the background class. */
  typedef std::vector<char> coordRulesChunk;
  
  // The player cannot pass widthin this many character's of the window boarder's (y, x).
  const yx PLAYER_MOVEMENT_INNER_MARGIN {0, 52};
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
   const char bgFileName [], const backgroundData & background) const;
  void initialiseCoordRules
  (const yx expectedChunkSize, const char coordRulesFileName [],
   const char bgFileName [], rules::coordRulesType & coordRules,
   const std::string & coordRulesData, const backgroundData & background) const;
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
  (const char coordRulesFileName [], const char bgFileName [],
   const coordRulesType & coordRules, const backgroundData & background) const;
  // ===== Headers Related To Loading COORD_RULES_FILE_EXTENSION Files END =====
  // ===========================================================================
  
  // ==== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ===
  // ===========================================================================
  // Where rawRules holds the contents of a RULES_CONFIG_FILE_EXTENSION file.
  // void parseRulesHeader(const yx maxyx, const char rulesFileName[],
  // 			  rules & levelRules, const size_t bgSize,
  // 		      const std::string & rawRules,
  // 		      std::string::const_iterator & buffPos);
  // void initPlayer(const yx maxyx, const char rulesFileName[], rules &levelRules,
  // 		const size_t bgSize, const std::string &rawRules,
  //                 std::string::const_iterator &buffPos);
  /* This function should be called for each background sprite section that's
     encountered. */
  // void initBgSprites(const yx maxyx, const char rulesFileName[], rules & levelRules,
  // 		   const size_t bgSize, const std::string & rawRules,
  // 		   std::string::const_iterator & buffPos);
  /* Attempts to read the start of the header in a RULES_CONFIG_FILE_EXTENSION
     file. */
  // void readStartOfHeader(const std::string &buff,
  //                        std::string::const_iterator &buffPos,
  //                        const std::string &eMsg);
  // /* Attempts to read the bracket at the start of a section. Calls exit with eMsg 
  //    and section if there is an error. */
  void readSectionOpeningBracket
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, const std::string & section,
   const bool skipSpace = true);
  /* Attempts to read the bracket at the end of a section. Calls exit with eMsg 
     and section if there is an error. */
  void readSectionEndingBracket
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, const std::string & section);
  /* Attempts to read the strings from a string section in a
     RULES_CONFIG_FILE_EXTENSION file. Should be called when a series of strings
     is expected. Buff is the buffer where the strings should be located and
     buffPos is the position to start reading from. EMsg will be embedded in any
     error message/s the function spits out and should say something about the
     context in which readStringsSection() was called. Returns the strings that
     were read. */
  void readStringsSection
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, void * retObj);
  /* Attempts to read one coordinate from a section in buff starting at buffPos.
     Emsg will be embedded in any error message/s the function spits out and
     should say something about the context in which the function was
     called. Returns the coordinate read. Only reads natural numbers (inclusive
     of 0.)*/
  void readSingleCoordSectionInNNumbers
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, void * coord);
  /* This function is the same as readSingleCoordSectionInNNumbers() with the one
     exception that it does not skip any spaces i.e. the first character of the
     coordinate (the "(" terminal) should be at *buffPos. Otherwise the program will
     be halted with an error message. */
  void readSingleCoordSectionInNNumbersNoSkpSp
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, void * coord);
  /* Same as readSingleCoordSectionInNNumbers() with the exception that it can
     read integers. */
  void readSingleCoordSectionInZNumbers
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, void * coord);
  /* This function should be called through readSingleCoordSectionInNNumbers() or
     readSingleCoordSectionInZNumbers(). SkipSpace suppresses the skipping of
     spaces before the opening terminal "(". */
  void readSingleCoordSection
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, const bool useIntegers,
   void * coord, const std::string typeOfNumber, const bool skipSpace = true);
  /* Attempts to read a boolean for a section in buff starting at buffPos. Emsg
     will be embedded in any error message/s the function spits out and should
     START say something about the context in which the function was called. Returns
     the bool read. Can read a boolean in a string or integer format i.e. "true" /
     "false" or 1 / 0. */
  void readBoolSection
  (const std::string & buff, std::string::const_iterator & buffPos,
   const std::string & eMsg, void * boolean);
  // /* Attempts to read a number starting at buffPos (will skip any space before the
  //    number.) */
  // int readSingleNum
  // (const std::string & buff, std::string::const_iterator & buffPos,
  //  const std::string & eMsg, const bool useIntegers);
  // Verifies that the header of a RULES_CONFIG_FILE_EXTENSION file is present.
  // void readEndOfHeader
  // (const std::string & buff, std::string::const_iterator & buffPos,
  //  const std::string & eMsg);
  // void parseRulesMain(const yx maxyx, const char bgFileName[],
  //                     const char rulesFileName[], rules &levelRules,
  //                     const size_t bgSize, const std::string &rawRules,
  //                     std::string::const_iterator &buffPos);
  // /* Checks if argument is an element of boarderRuleChars::CHARS (see utils.cpp).
  //    Or if the argument is a space character. If either of these is true then
  //    returns. Otherwise calls exit() */
  // void checkRuleChar(const char potentialRule, const std::string eMsg);

  /* Should be called after checking for all header sections related to the
     player. Checks if all sections were found. If a section is encountered that
     wasn't found, then we check if it has a default value. If so we apply the
     default value. If it doesn't have a default value then call exit()*/
  // void checkForDefaultPlayerValues(
  // 				 std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
  // 				 playerHeaderKeywordActions,
  // 				 levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
  // 				 levelFileKeywords::playerInitialData &playerInitData,
  // 				 std::string::const_iterator &buffPos, const char rulesFileName[]);
  // void checkForDefaultBgSpriteValues
  // (std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
  //  bgSpriteHeaderKeywordActions,
  //  levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
  //  levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, 
      //  std::string::const_iterator & buffPos,
      //  const char rulesFileName []);
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
  void movePlayer(sprite::directions input,
		  int & position, const yx maxyx, const size_t backgroundLength);
  /* No functions that change the position of the player should be called after
     this one for a given frame. */
  /* Calls handleFinalPlayerMovementAndWindowAndMarginInteractions after
     checking that the player won't go outside of the level after it's position
     is updated to peekPos. */
  void handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength);
  void handleFinalPlayerMovementAndWindowAndMarginInteractions
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* Moves the player when interacting with the inner margin (NOTE THAT THIS
     FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
     THE INNER MARGIN AND NOT OUTSIDE THE BOUNDS OF THE LEVEL AS DEFINED BY THE
     BACKGROUND.) */
  void movePlayerWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerRightWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerLeftWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  // /* Returns number of characters untill the player bits a boarder character
  //    (if moving up). Will return -1 if there are no boarder characters above */
  // int getClosestBoarderCharAbove(const int position, const bool directContact);
  /* Handles collision with boarder characters when the player is moving down
     Input should be the player input direction and the current absolute
     position of the player sprite in the x dimension. Returns updated
     direction. */
  sprite::directions handleGroundCollision(const int position,
					   const int backgroundHeight);
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character unless the player has a y position of 0, in which case the
     player will be stopped. The player sprite will also be stopped if there is
     more then one character directly to the right of it. */
  sprite::directions handleRightCollision(const int position,
					  const int backgroundHeight);
  // // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision(const int position,
					 const int backgroundHeight);

#ifdef DEBUG
  // Where maxX should be the maximum size of the window and not the level.
  void printRuleChars(const int position, const int maxY, const int maxX);
#endif

public:
  /* background is required because for every chunk in background there should
     be a corresponding chunk in rules::coordRules. */
  rules
  (const yx maxyx, const backgroundData & background, const char bgFileName [],
   const char coordRulesFileName [], const char rulesFileName []) :
    coordRules(loadAndInitialiseCoordRules
	       (maxyx, coordRulesFileName, bgFileName, background)),
    coordRulesCurrentContextBufferSize(maxyx.y * maxyx.x * 9),
    coordRulesCurrentContextBuffer(new char [maxyx.y * maxyx.x * 9])
  {
  }
  void physics(const player::directionChars input, int & position, const yx maxyx,
	       const size_t backgroundLength,
	       std::__1::chrono::steady_clock::time_point & secStartTime);
  ~rules()
  {
    delete gamePlayer;
    delete [] coordRulesCurrentContextBuffer;
  }
};


#endif
