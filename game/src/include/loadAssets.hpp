#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "levelRules.hpp" // For rules.


/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places then this. */
constexpr int MAX_COORD_LEN {10};
namespace levelFileTokens
{
  /* Here p stands for player and this string denotes the player sprite
     parameters section of the rules.lev file. */
  constexpr char PLAYER_HEADER_SECTION_SPECIFIER {'p'};
  // Each new main section in the header should start with this character.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  constexpr char STRING_DENOTATION	{'\"'};
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
void readStartOfHeader(const std::string & buff,
		       std::string::const_iterator & buffPos,
		       const std::string & eMsg);
/* Attempts to read the strings from a string section in a rules.lev file.
   Should be called when a series of strings is expected. Buff is the buffer
   where the strings should be located and buffPos is the position to start
   reading from. EMsg will be embedded in any error message/s the function spits
   out and should say something about the context in which readStringsSection()
   was called. Returns the strings that were read. */
std::vector<std::string>
readStringsSection(const std::string & buff,
                   std::string::const_iterator & buffPos,
                   const std::string & eMsg);
/* Attempts to read one coordinate from a section in buff starting at buffPos.
   Emsg will be embedded in any error message/s the function spits out and
   should say something about the context in which readSingleCoordSection() was
   called. Returns the coordinate read. */
yx readSingleCoordSection(const std::string &buff,
                          std::string::const_iterator &buffPos,
                          const std::string &eMsg);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum(const std::string & buff,
		 std::string::const_iterator & buffPos,
		 const std::string & eMsg);
// /* Extract and parse header info in buff. */
// void parseLevelRules(const yx maxyx, std::string & buff, const char rulesFileName [],
// 	   rules & levelRules, cons tsize_t bgSize);
// /* Branch to differnt parsing function's depending on the values of current and
//    peek. InHeader tells whether we are in the header (parsing decisions may be
//    different depending on it's state.) */
// void switchOnCurrent
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max,
//  rules & levelRules, const size_t bgSize, bool & inHeader);
// /* Dispatches functions based on the value of *peek. Should only be called when
//  *current == SECTION_START_DENOTATION */
// void handleCurrentIsFieldStartDenotation
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max,
//  rules & levelRules, const size_t bgSize);
// /* Read in the character *current (which should already have been checked for
//    validity), then read in the coordinate pair, finally the function should
//    check to see that this coordinate pair is unique in the object
//    levelRules.charCoords and if it is use the coordinates as a key to store the
//    character (which is to be interprited as a Coordinate character */
// void handleCoordinateCharacter
// (std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator & max,
//  rules & levelRules, const size_t bgSize);
// /* Read in the character *current (which should already have been checked for
//    it's validity), then read in the coordinate pair, finally the function should
//    check to see that this coordinate pair is unique in the object
//    levelRules.charCoords and if it is use the coordinates as a key to store the
//    character (which is to be interprited as a Coordinate character */
// void handleCoordinateCharacter
// (std::string::const_iterator & current, std::string::const_iterator & peek,
//  std::string::const_iterator & max, const rules & levelRules);
/* Calls handleStringDenotationAfterFieldDenotation to read in string's. Then
   calls getCoords to read in coordinates. Then initialises player */
// void initPlayerSprite
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator & max,
//  rules & levelRules, const size_t bgSize);
// /* Check's for coordinate of the form "(Y,X)" and return's "Y,X" (discarding
//    "(" and ")") if Y and X are valid integer number's that do not falloutside of
//    the X and Y values in maxyx and are greater or equal to zero.  */
// std::string getCoords(std::string::const_iterator & current,
// 		      std::string::const_iterator & peek, const size_t bgSize);
// /* Read's in a number of string's separated by the STRING_SEPERATION character.
//    Stop when current reaches the character where a STRING_SEPERATION character
//    is expected. Returns each string read in a vector. */
// std::vector<std::string>
// handleStringDenotationAfterFieldDenotation
// (std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max);
// /* Called when we encounter the STRING_DENOTATION character. Extract's and
//    returns string (dealing with escape character's.) Call's exit if there is an
//    error */
// std::string getString
// (std::string::const_iterator & current, std::string::const_iterator & peek,
//  const std::string::const_iterator max);
// /* Increment's peek untill it point's to a non white space character. At which
//    point proceeds to delete character's [current, peek) if (++current != peek).
//    Return's false if peek becomes >= max while incrementing. It is assumed that
//    current and peek point to consecutive elment's */
// void printCurrent
// (std::string::const_iterator & current, const int SHOW_COUNT,
//  std::stringstream & e);


#endif
