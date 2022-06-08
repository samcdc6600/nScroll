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
void loadAndParseLevelRulesFile(const yx maxyx, const char rulesFileName[],
                                rules & levelRules, const size_t bgSize);
// Where rawRules holds the contents of a rules.lev file.
void parseRulesHeader(const yx maxyx, const char rulesFileName[],
                      rules & levelRules, const size_t bgSize,
                      const std::string & rawRules);
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
void readEndOfHeader(const std::string & buff,
		     std::string::const_iterator & buffPos,
		     const std::string & eMsg);


/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places then this. */
constexpr int MAX_COORD_LEN{10};
namespace levelFileKeywords
{
  namespace keywordAction
  {
    class defaultValABS
    {
      virtual void dummy() = 0;
    };

    // Used to get default values of different types dynamically.
    template <typename T> class defaultVal : public defaultValABS
    {
      void dummy() {};
      const T val;
      
    public:      
      defaultVal(T v) : val(v) {}
    };
  
    struct headerKeywordAction
    {
      headerKeywordAction(const std::string & kword, const std::string rType,
			  const defaultValABS * dVal,
			  void (* a)(const std::string & buff,
				     std::string::const_iterator & buffPos,
				     const std::string & eMsg, void * retObj))
	: keyword(kword), returnType(rType), defaultVal(dVal), action(a)
      { }      
      const std::string keyword;
      bool found {false};			// Found this keyword already.
      /* Tells what to cast default and retObj to (if an empty string then assume
	 there is no default value.) */
      const std::string returnType;
      // Default value for this keyword.
      const defaultValABS * defaultVal;
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
  const std::string SPRITE_FILE_SECTION_HEADER{"sprites"};
  const keywordAction::defaultVal<std::string>  playerSpriteFileDefaultVal {""};
  const std::string SPRITE_INIT_COORD_SECTION_HEADER	{"initialCoordinate"};
  const keywordAction::defaultVal<yx>	playerCoordDefaultVal {yx {0, 1}};
  // Sub section headers for player sprite.
  const std::string SPRITE_INIT_DIR_SECTION_HEADER	{"initialDirection"};
  const std::string SPRITE_HEALTH_SECTION_HEADER	{"initialHealth"};
  const std::string SPRITE_GRAV_CONST_SECTION_HEADER	{"gravConst"};
  const std::string SPRITE_MAX_VERT_V_SECTION_HEADER	{"maxVelocity"};
  const std::string SPRITE_MAX_FALL_JMP_SECTION_HEADER	{"maxJumpsAfterFall"};
  const std::string SPRITE_MAX_JMP_NUM_SECTION_HEADER	{"maxJumps"};
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
  constexpr int SHOW_COUNT {45}; // How many characters to print in printN
  // function.
}


#endif
