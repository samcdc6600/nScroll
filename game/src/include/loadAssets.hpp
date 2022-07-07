#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
//#include <fstream>
#include "levelRules.hpp" // For rules.


// /* Coordinate character's. I have decided to add an uppercase alternative for
//    each character. This is so that rules.lev file's will be more forgiving of
//    "mistakes." */
// constexpr char BOARDER_CHAR {'b'}; // Player character's cannot pass through coordinate's marked as this.
// constexpr char BOARDER_CHAR_UPPER {'B'};
// /* Player character's are killed uppon intersection with coordinate's marked as
//    this. */
// constexpr char KILL_CHAR {'k'};
// constexpr char KILL_CHAR_UPPER {'K'};
// constexpr char DEGRADE_HEALTH_CHAR {'d'}; // Player character's loose health uppon intersection with coordinate's
// constexpr char DEGRADE_HEALTH_CHAR_UPPER {'D'}; // marked as this.
// constexpr char DEGRADE_HEALTH_BOARDER_CHAR {'i'}; // Combination of DEGRADE_HEALTH_CHAR and BOARDER_CHAR.
// constexpr char DEGRADE_HEALTH_BOARDER_CHAR_UPPER {'I'};
// /* Player character's "win" the level uppon intersection with coordinates
//    marked. */
// constexpr char END_LEV_CHAR {'e'};
// constexpr char END_LEV_CHAR_UPPER {'E'}; // as this.
// // Player character's gain health point's when comming into contact
// constexpr char LIFE_POWER_UP_CHAR {'l'};
// constexpr char LIFE_POWER_UP_CHAR_UPPER {'L'};


/* This function should initialise the argument background that is passed to it.
   This variable should then contain the "grphical data" of the level. It should
   initialise the levelRules argument that is passed to it, This variable should
   then contain the "rules and non player sprites" as well as the player sprite
   of the level. The bgFileName and rulesFileName are the names of the files
   that should be read in and parsed to initialise the background and levelRules
   arguments. */
void loadAssets
(const yx maxyx, const char bgFileName [], std::vector<int> & background,
 const char rulesFileName [], rules & levelRules);
void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				std::vector<int> & background);
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
                const std::string &rawRules,
                std::string::const_iterator &buffPos);
/* Attempts to read the start of the header in a rules.lev file. */
void readStartOfHeader(const std::string &buff,
                       std::string::const_iterator &buffPos,
                       const std::string &eMsg);
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
   should say something about the context in which readSingleCoordSection() was
   called. Returns the coordinate read. */
void readSingleCoordSection(const std::string & buff,
                          std::string::const_iterator & buffPos,
                          const std::string & eMsg, void * coord);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum(const std::string &buff, std::string::const_iterator &buffPos,
                  const std::string &eMsg);
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
      headerKeywordAction(const std::string & kword,
			  void (* a)(const std::string & buff,
				     std::string::const_iterator & buffPos,
				     const std::string & eMsg, void * retObj))
	: keyword(kword), action(a)
      { }

      const std::string keyword;
      bool found {false};			// Found this keyword already.
      /* If action needs to return anything the address of a variable of the
	 appropriate type should be passed to the function via retObj. If the
	 function doesn't encounter any error (in which case it should print a
	 message and exit) *retObj should be populated with what is essential the
	 return value of the function. */
      void (*action)(const std::string &buff,
		     std::string::const_iterator &buffPos,
		     const std::string &eMsg, void *retObj);
    };
  };
  // Each new section should start with a header char followed by this char.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  // Header section header chars.
  constexpr char PLAYER_HEADER_SECTION_SPECIFIER {'p'};
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
  /* Used to map keywords to unique int values so appropriate action can be
     taken for those keywords when parsing player section. KEY ORDER SHOULD
     MATCH playerSectionHasDefaultValue! */
  const std::map<std::string, int> playerSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER, 0},
    {SPRITE_INIT_COORD_SECTION_HEADER, 1},
    {SPRITE_INIT_DIR_SECTION_HEADER, 2},
    {SPRITE_HEALTH_SECTION_HEADER, 3},
    {SPRITE_GRAV_CONST_SECTION_HEADER, 4},
    {SPRITE_MAX_VERT_V_SECTION_HEADER, 5},
    {SPRITE_MAX_FALL_JMP_SECTION_HEADER, 6},
    {SPRITE_MAX_JMP_NUM_SECTION_HEADER, 7}
  };
  namespace defaultValues
  {
    // KEY ORDER SHOULD MATCH playerSectionKeywordToId!
    const std::map<std::string, bool> playerSectionHasDefaultValue
      {
	{SPRITE_FILE_SECTION_HEADER, false},
	{SPRITE_INIT_COORD_SECTION_HEADER, false},
	{SPRITE_INIT_DIR_SECTION_HEADER, true},
	{SPRITE_HEALTH_SECTION_HEADER, true},
	{SPRITE_GRAV_CONST_SECTION_HEADER, true},
	{SPRITE_MAX_VERT_V_SECTION_HEADER, true},
	{SPRITE_MAX_FALL_JMP_SECTION_HEADER, true},
	{SPRITE_MAX_JMP_NUM_SECTION_HEADER, true}
      };

    // Default values for the player.
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
void checkForDefaultPlayerValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 playerHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::playerInitialData & playerInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName []);

#endif
