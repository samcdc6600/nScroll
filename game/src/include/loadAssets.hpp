#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "levelRules.hpp" // For rules.


constexpr char HEADER_START [] = {"(p("}; // The header should always start with me.
constexpr char FIELD_START_DENOTATION {'('}; // Marks the start of a new field or section of the current file.
constexpr char FIELD_END_DENOTATION {')'}; // Marks the end of the current field or section of the current file.
constexpr char HEADER_END_DENOTATION {'#'}; // Marks the end of the header sectino of the file.
constexpr char STRING_DENOTATION {'"'}; // This character denotes the start of a sting
constexpr int SHOW_COUNT {45}; // How many characters to print in printN function.
/* This character denotes the presence of another string when used after a
   string. */
constexpr char STRINGS_SEPERATION {','};
/* The character used for escape sequences (within a string) in .rules.lev
   files. */
constexpr char ESCAPE_CHAR {'\\'};
constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
constexpr char NULL_BYTE {'\0'};
/* Coordinate character's. I have decided to add an uppercase alternative for
   each character. This is so that rules.lev file's will be more forgiving of
   "mistakes." */
constexpr char BOARDER_CHAR {'b'}; // Player character's cannot pass through coordinate's marked as this.
constexpr char BOARDER_CHAR_UPPER {'B'};
/* Player character's are killed uppon intersection with coordinate's marked as
   this. */
constexpr char KILL_CHAR {'k'};
constexpr char KILL_CHAR_UPPER {'K'};
constexpr char DEGRADE_HEALTH_CHAR {'d'}; // Player character's loose health uppon intersection with coordinate's
constexpr char DEGRADE_HEALTH_CHAR_UPPER {'D'}; // marked as this.
constexpr char DEGRADE_HEALTH_BOARDER_CHAR {'i'}; // Combination of DEGRADE_HEALTH_CHAR and BOARDER_CHAR.
constexpr char DEGRADE_HEALTH_BOARDER_CHAR_UPPER {'I'};
/* Player character's "win" the level uppon intersection with coordinates
   marked. */
constexpr char END_LEV_CHAR {'e'};
constexpr char END_LEV_CHAR_UPPER {'E'}; // as this.
// Player character's gain health point's when comming into contact
constexpr char LIFE_POWER_UP_CHAR {'l'};
constexpr char LIFE_POWER_UP_CHAR_UPPER {'L'};


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
/* Opens file "name" and copies it's content's into buff. If there is an error
   opening the file returns false. Return's true otherwise. */
bool loadASCIIFile(const char name [], std::string & buff);
/* Load rules file, parse header, extract sprite coordinates and sprite name/s
   and process sprites. extract sprite rule after processing each sprite. Header
   should contain file location and starting position of player sprite and the
   player should be initialised using this info. The header may also contain
   other information. When a sprite coordinate, sprite and rule triple has been
   extracted add sprite and rule using sprite coordinate as key to a vector in
   the spriteCoords map in the form of a spriteInfo struct */
void loadParseAndInitialiseRules
(const yx maxyx, const char rulesFileName [], rules & levelRules,
 const size_t bgSize);
/* Extract and parse header info in buff. */
void parse(const yx maxyx, std::string & buff, const char rulesFileName [],
	   rules & levelRules, const size_t bgSize);
/* Increment's (advances) i by (n -1), if i equals iEnd before being incremented
   n times we throw an exception. */
template <typename T_A, typename T_B> auto
getAdvancedIter(T_A i, T_B iEnd, const size_t n) -> T_A
{
  for(size_t iter {}; iter < size_t(n -1); ++iter)
    {
      i++;
      if(*i == *iEnd)
	throw std::out_of_range ("range error");
    }
  return i;
}
/* Branch to differnt parsing function's depending on the values of current and
   peek. InHeader tells whether we are in the header (parsing decisions may be
   different depending on it's state.) */
void switchOnCurrent
(const yx maxyx, std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator max,
 rules & levelRules, const size_t bgSize);
/* Dispatches functions based on the value of *peek. Should only be called when
 *current == FIELD_START_DENOTATION */
void handleCurrentIsFieldStartDenotation
(const yx maxyx, std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator max,
 rules & levelRules, const size_t bgSize);
/* Read in the character *current (which should already have been checked for
   validity), then read in the coordinate pair, finally the function should
   check to see that this coordinate pair is unique in the object
   levelRules.charCoords and if it is use the coordinates as a key to store the
   character (which is to be interprited as a Coordinate character */
void handleCoordinateCharacter
(std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator & max,
 rules & levelRules, const size_t bgSize);
/* Read in the character *current (which should already have been checked for
   it's validity), then read in the coordinate pair, finally the function should
   check to see that this coordinate pair is unique in the object
   levelRules.charCoords and if it is use the coordinates as a key to store the
   character (which is to be interprited as a Coordinate character */
void handleCoordinateCharacter
(std::string::const_iterator & current, std::string::const_iterator & peek,
 std::string::const_iterator & max, const rules & levelRules);
/* Calls handleStringDenotationAfterFieldDenotation to read in string's. Then
   calls getCoords to read in coordinates. Then initialises player */
void initPlayerSprite
(const yx maxyx, std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator & max,
 rules & levelRules, const size_t bgSize);
/* Check's for coordinate of the form "(Y,X)" and return's "Y,X" (discarding
   "(" and ")") if Y and X are valid integer number's that do not falloutside of
   the X and Y values in maxyx and are greater or equal to zero.  */
std::string getCoords(std::string::const_iterator & current,
		      std::string::const_iterator & peek, const size_t bgSize);
/* Read's in a number of string's separated by the STRING_SEPERATION character.
   Stop when current reaches the character where a STRING_SEPERATION character
   is expected. Returns each string read in a vector. */
std::vector<std::string>
handleStringDenotationAfterFieldDenotation
(std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator max);
/* Called when we encounter the STRING_DENOTATION character. Extract's and
   returns string (dealing with escape character's.) Call's exit if there is an
   error */
std::string getString
(std::string::const_iterator & current, std::string::const_iterator & peek,
 const std::string::const_iterator max);
/* Increment's peek untill it point's to a non white space character. At which
   point proceeds to delete character's [current, peek) if (++current != peek).
   Return's false if peek becomes >= max while incrementing. It is assumed that
   current and peek point to consecutive elment's */
bool rubOutSpace
(std::string & buff, std::string::const_iterator & current,
 std::string::const_iterator & peek, std::string::const_iterator & max);
void printCurrent
(std::string::const_iterator & current, const int SHOW_COUNT,
 std::stringstream & e);


#endif
