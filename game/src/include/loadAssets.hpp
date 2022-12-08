#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
//#include <fstream>
#include "levelRules.hpp" // For rules.
#include "background.hpp"


// constexpr char BOARDER_CHAR {'b'}; // Player character's cannot pass through coordinate's marked as this.
// /* Player character's are killed uppon intersection with coordinate's marked as
//    this. */
// constexpr char KILL_CHAR {'k'};
// constexpr char DEGRADE_HEALTH_CHAR {'d'}; // Player character's loose health uppon intersection with coordinate's
// constexpr char DEGRADE_HEALTH_BOARDER_CHAR {'i'}; // Combination of DEGRADE_HEALTH_CHAR and BOARDER_CHAR.
// /* Player character's "win" the level uppon intersection with coordinates
//    marked. */
// constexpr char END_LEV_CHAR {'e'};
// // Player character's gain health point's when comming into contact
// constexpr char LIFE_POWER_UP_CHAR {'l'};

/* This function should initialise the argument background that is passed to it.
   This variable should then contain the "grphical data" of the level. It should
   initialise the levelRules argument that is passed to it, This variable should
   then contain the "rules and non player sprites" as well as the player sprite
   of the level. The bgFileName and rulesFileName are the names of the files
   that should be read in and parsed to initialise the background and levelRules
   arguments. */
void loadAssets
(const yx maxyx, const char bgFileName [], // std::vector<int> & background,
 backgroundData & background,
 const char rulesFileName [], rules & levelRules);
void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				backgroundData & background);
				// std::vector<int> & background);
/* Load rules file, parse header, extract sprite coordinates and sprite name/s
   and process sprites. extract sprite rule after processing each sprite. Header
   should contain file location and starting position of player sprite and the
   player should be initialised using this info. The header may also contain
   other information. When a sprite coordinate, sprite and rule triple has been
   extracted add sprite and rule using sprite coordinate as key to a vector in
   the spriteCoords map in the form of a spriteInfo struct */
void loadAndParseLevelRulesFile(const yx maxyx, const char bgFileName [],
				const char rulesFileName[],
                                rules &levelRules, const size_t bgSize);
// Where rawRules holds the contents of a rules.lev file.
void parseRulesHeader(const yx maxyx, const char rulesFileName[],
			  rules & levelRules, const size_t bgSize,
		      const std::string & rawRules,
		      std::string::const_iterator & buffPos);
void initPlayer(const yx maxyx, const char rulesFileName[], rules &levelRules,
		const size_t bgSize, const std::string &rawRules,
                std::string::const_iterator &buffPos);
/* This function should be called for each background sprite section that's
   encountered. */
void initBgSprites(const yx maxyx, const char rulesFileName[], rules & levelRules,
		   const size_t bgSize, const std::string & rawRules,
		   std::string::const_iterator & buffPos);
/* Attempts to read the start of the header in a rules.lev file. */
void readStartOfHeader(const std::string &buff,
                       std::string::const_iterator &buffPos,
                       const std::string &eMsg);
/* Attempts to read the bracket at the start of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionOpeningBracket(const std::string &buff,
                               std::string::const_iterator &buffPos,
                               const std::string &eMsg,
                               const std::string &section);
/* Attempts to read the bracket at the end of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionEndingBracket(const std::string & buff,
			      std::string::const_iterator & buffPos,
			      const std::string & eMsg,
			      const std::string & section);
/* Attempts to read the strings from a string section in a rules.lev file.
   Should be called when a series of strings is expected. Buff is the buffer
   where the strings should be located and buffPos is the position to start
   reading from. EMsg will be embedded in any error message/s the function spits
   out and should say something about the context in which readStringsSection()
   was called. Returns the strings that were read. */
void readStringsSection(const std::string & buff,
                   std::string::const_iterator & buffPos,
                   const std::string & eMsg, void * retObj);
/* Attempts to read one coordinate from a section in buff starting at buffPos.
   Emsg will be embedded in any error message/s the function spits out and
   should say something about the context in which the function was
   called. Returns the coordinate read. Only reads natural numbers (inclusive
   of 0.)*/
void readSingleCoordSectionInNNumbers(const std::string & buff,
                                      std::string::const_iterator & buffPos,
                                      const std::string & eMsg, void * coord);
/* Same as readSingleCoordSectionInNNumbers() with the exception that it can
   read integers. */
void readSingleCoordSectionInZNumbers(const std::string & buff,
                                      std::string::const_iterator & buffPos,
                                      const std::string & eMsg, void * coord);
/* This function should be called through readSingleCoordSectionInNNumbers() or
   readSingleCoordSectionInZNumbers() */
void readSingleCoordSection(const std::string & buff,
                            std::string::const_iterator &buffPos,
                            const std::string & eMsg, const bool useIntegers,
                            void *coord, const std::string typeOfNumber);
/* Attempts to read a boolean for a section in buff starting at buffPos. Emsg
   will be embedded in any error message/s the function spits out and should
   say something about the context in which the function was called. Returns
   the bool read. Can read a boolean in a string or integer format i.e. "true" /
   "false" or 1 / 0. */
void readBoolSection(const std::string & buff,
		    std::string::const_iterator & buffPos,
		    const std::string & eMsg, void * boolean);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum(const std::string &buff, std::string::const_iterator &buffPos,
                  const std::string &eMsg, const bool useIntegers);
// Verifies that the header of a .rules.lev file is present.
void readEndOfHeader(const std::string &buff,
                     std::string::const_iterator &buffPos,
                     const std::string &eMsg);
void parseRulesMain(const yx maxyx, const char bgFileName[],
                    const char rulesFileName[], rules &levelRules,
                    const size_t bgSize, const std::string &rawRules,
                    std::string::const_iterator &buffPos);
/* Checks if argument is an element of boarderRuleChars::CHARS (see utils.cpp).
   Or if the argument is a space character. If either of these is true then
   returns. Otherwise calls exit() */
void checkRuleChar(const char potentialRule, const std::string eMsg);


/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places then this. */
constexpr int MAX_COORD_LEN{10};
namespace levelFileKeywords
{
  namespace keywordAction
  {
    struct headerKeywordAction
    {
      headerKeywordAction
      (const std::string & kword, void (* a)
       (const std::string & buff, std::string::const_iterator & buffPos,
	const std::string & eMsg, void * retObj),
       void (*hA)(const yx maxyx, const char rulesFileName[],
		  rules &levelRules, const size_t bgSize,
		  const std::string &rawRules,
		  std::string::const_iterator &buffPos) = nullptr,
       const bool fMO = false)
	: keyword(kword), foundMultipleOptional(fMO), action(a), headerAction(hA)
      { }

      const std::string keyword;
      // Found this keyword already (for keywords that can only appear once.).
      bool found{false};
      /* Set to true in the constructor if keyword is optional and can appear
	 multiple times. */
      const bool foundMultipleOptional;
      /* If action needs to return anything the address of a variable of the
	 appropriate type should be passed to the function via retObj. If the
	 function doesn't encounter any error (in which case it should print a
	 message and exit) *retObj should be populated with what is essential the
	 return value of the function. */
      void (*action)(const std::string &buff,
		     std::string::const_iterator &buffPos,
		     const std::string &eMsg, void *retObj);
      /* This function should be used as the action instead of the above if we
	 are at the top level of the header (in terms of sections). */
      void (*headerAction)(const yx maxyx, const char rulesFileName[],
			   rules &levelRules, const size_t bgSize,
			   const std::string &rawRules,
			   std::string::const_iterator &buffPos);
    };
  };
  // Each new section should start with a header char followed by this char.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  // Header section header.
  const std::string PLAYER_HEADER_SECTION_SPECIFIER {"player"};
  const std::string BG_SPRITE_HEADER_SECTION_SPECIFIER {"backgroundSprite"};
  // Header sub section headers.
  // Sub section headers for general player and sprites.
  const std::string SPRITE_FILE_SECTION_HEADER		{"sprites"};
  const std::string SPRITE_INIT_COORD_SECTION_HEADER	{"initialCoordinate"};
  // Sub section headers for player sprite.
  const std::string SPRITE_INIT_DIR_SECTION_HEADER	{"initialDirection"};
  const std::string SPRITE_HEALTH_SECTION_HEADER	{"initialHealth"};
  const std::string SPRITE_GRAV_CONST_SECTION_HEADER	{"gravConst"};
  const std::string SPRITE_MAX_VERT_V_SECTION_HEADER	{"maxVelocity"};
  const std::string SPRITE_MAX_FALL_JMP_SECTION_HEADER	{"maxJumpsAfterFall"};
  const std::string SPRITE_MAX_JMP_NUM_SECTION_HEADER	{"maxJumps"};
  // Sub section headers for background sprite.
  const std::string SPRITE_DISPLAY_IN_FORGROUND         {"displayInForground"};
  // Used to store the order of a keyword and whether it has a default value.
  struct orderAndDefaultVal
  {
    int order;
    bool defaultVal;
  };
  /* Used to map keywords to unique int values so appropriate action can be
     taken for those keywords when parsing specific section section. KEY ORDER
     (first value of orderAndDefaultVal) OF THESE OBJECTS SHOULD MATCH switches
     that use these objects.! */
  const std::map<std::string, orderAndDefaultVal> headerSectionKeywordToId {
    {PLAYER_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {0, false}},
    {BG_SPRITE_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {1, false}}
  };
  const std::map<std::string, orderAndDefaultVal> playerSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
    {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
    {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
    {SPRITE_HEALTH_SECTION_HEADER,	orderAndDefaultVal {3, true}},
    {SPRITE_GRAV_CONST_SECTION_HEADER,	orderAndDefaultVal {4, true}},
    {SPRITE_MAX_VERT_V_SECTION_HEADER,	orderAndDefaultVal {5, true}},
    {SPRITE_MAX_FALL_JMP_SECTION_HEADER, orderAndDefaultVal {6, true}},
    {SPRITE_MAX_JMP_NUM_SECTION_HEADER,	orderAndDefaultVal {7, true}}
  };
  const std::map<std::string, orderAndDefaultVal> bgSpriteSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
    {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
    {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
    {SPRITE_DISPLAY_IN_FORGROUND,       orderAndDefaultVal {3, true}}
  };
  namespace defaultValues
  {
    /* These default values can only be used if defaultVal is set to true in
       orderAndDefaultVal for the values corresponding section in it's
       associated XXXSectionKeywordToId map (see above). */
    namespace player
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx coordinate {0, 1};
      const sprite::directions direction {sprite::DIR_NONE};
      const int health {16};
      const double gravitationalConstant {-0.38};
      const double maxVerticalVelocity {1.9};
      const unsigned maxFallingJumpNumber {1};
      const unsigned maxJumpNumber {3};
    }
    namespace bgSprites
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx coordinate {20, 62};
      const sprite::directions direction {sprite::DIR_NONE};
      const bool displayInForground {false};
    }
  }
  /* This struct should be populated with the values that the player will
     eventually be initialised with. */
  struct playerInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
    sprite::directions direction {};
    int health {};
    double gravitationalConstant {};
    double maxVerticalVelocity {};
    unsigned maxFallingJumpNumber {};
    unsigned maxJumpNumber {};
  };
  /* This struct should be populated with the values read in for use with the
     constructor of the next background sprite to be created. */
  struct bgSpriteInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
    sprite::directions direction {};
    bool displayInForground {};
  };
  // Misc.
  constexpr char STRING_DENOTATION		{'\"'};
  constexpr char STRING_SEPARATION		{','};
  constexpr char STRING_ESC			{'\\'};
  constexpr char COORD_SEPARATION		{','};
  constexpr char DIR_START_ABS			{'/'};
  constexpr char DIR_START_REL			{'.'};
  /* The character used for escape sequences (within a string) in .rules.lev
     files. */
  constexpr char ESCAPE_CHAR {'\\'};
  constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
  constexpr char NULL_BYTE {'\0'};
  /* If this character is encountered in the main section of a rules.lev file
     the character 2 places after it should be an ASCII number. After this
     number there can be a string of contiguous ASCII numbers (up to some
     maximum) that together represent some larger number. This number is the
     number of times the ASCII character before the number should be repeated. */
  constexpr char RULES_MAIN_RUNLENGTH_BEGIN_CHAR {'R'};
}


/* Should be called after checking for all header sections related to the
   player. Checks if all sections were found. If a section is encountered that
   wasn't found, then we check if it has a default value. If so we apply the
   default value. If it doesn't have a default value then call exit()*/
void checkForDefaultPlayerValues(
				 std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
				 playerHeaderKeywordActions,
				 levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
				 levelFileKeywords::playerInitialData &playerInitData,
				 std::string::const_iterator &buffPos, const char rulesFileName[]);
void checkForDefaultBgSpriteValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 bgSpriteHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName []);

#endif
