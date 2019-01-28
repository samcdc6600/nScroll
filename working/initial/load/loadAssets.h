#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "../../physics/physics.h" // For rules.


constexpr char HEADER_START [] = {"(p("}; // The header should always start with me.
constexpr char FIELD_START_DENOTATION {'('};	 // Marks the start of a new field or section of the current file.
constexpr char FIELD_END_DENOTATION {')'};	 // Marks the end of the current field or section of the current file.
constexpr char HEADER_END_DENOTATION {'#'}; // Marks the end of the header sectino of the file.
constexpr char STRING_DENOTATION {'"'}; // This character denotes the start of a sting
 // This character denotes the presence of another string when used after a string.
constexpr char STRINGS_SEPERATION {','};
constexpr char ESCAPE_CHAR {'\\'}; // The character used for escape sequences (within a string) in .rules.lev file's.
constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.


/* This function should initialise the argument background that is passed to it. This variable should then contain
   the "grphical data" of the level. It should initialise the levelRules argument that is passed to it, This variable
   should then contain the "rules and non player sprites" as well as the player sprite of the level. The bgFileName
   and rulesFileName are the names of the files that should be read in and parsed to initialise the background and
   levelRules arguments. */
void loadAssets(const yx maxyx, const char bgFileName [], std::vector<int> & background,
		const char rulesFileName [], rules & levelRules);
/* Opens file "name" and copies it's content's into buff. If there is an error opening the file returns false.
   Return's true otherwise. */
bool loadASCIIFile(const char name [], std::string & buff);
/* Load rules file, parse header, extract sprite coordinates and sprite name/s and process sprites. extract sprite
   rule after processing each sprite.
   Header should contain file location and starting position of player sprite and the player should be initialised
   using this info. The header may also contain other information. 
   When a sprite coordinate, sprite and rule triple has been extracted add sprite and rule using sprite coordinate as
   key to a vector in the spriteCoords map in the form of a spriteInfo struct */
void parseAndInitialiseRules(const yx maxyx, const char rulesFileName [], rules & levelRules);
/* Extract and parse header info in buff. */
void parse(const yx maxyx, std::string & buff, const char rulesFileName [], rules & levelRules);
/* Increment's (advances) i by (n -1), if i equals iEnd before being incremented n times we throw an exception. */
template <typename T_A, typename T_B> auto getAdvancedIter(T_A i, T_B iEnd, const size_t n) -> T_A
{
  for(int iter {}; iter < (n -1); ++iter)
    {
      i++;
      if(*i == *iEnd)
	throw std::out_of_range ("range error");
    }
  return i;
}
/* Branch to differnt parsing function's depending on the values of current and peek. string may be modified to hold
   the value of a string if the STRING_DENOTATION character is encountered and we are not already in a sring.
   InHeader tells whether we are in the header (parsing decisions may be different depending on it's state.) */
int switchOnCurrent(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		    std::string::const_iterator & peek, std::string::const_iterator max, bool inHeader,
		    rules & levelRules);
/* This function should be called from switchOnCurrent when *current == FIELD_DENOTATION. 
   It despatches a function based on the value of *peek */
void handleCurrentIsFieldDenotation(const yx maxyx, std::string & buff, std::string::const_iterator & current,
				    std::string::const_iterator & peek, std::string::const_iterator max,
				    const bool inHeader, rules & levelRules);
/* Calls handleStringDenotationAfterFieldDenotation to read in string's. Then calls getCoords to read in coordinates.
Then initialises player */
void initPlayerSprite(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator & max, rules & levelRules);
/* Check's for coordinate of the form "(Y,X)" and return's "Y,X" (discarding "(" and ")") if Y and X are valid
   integer number's that do not falloutside of the X and Y values in maxyx and are greater or equal to zero.  */
std::string getCoords(const std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, const std::string::const_iterator & max);
/* Read's in a number of string's separated by the STRING_SEPERATION character. Stop when current reaches the
  character where a STRING_SEPERATION character is expected. Returns each string read in a vector. */
std::vector<std::string> handleStringDenotationAfterFieldDenotation(std::string & buff,
							       std::string::const_iterator & current,
							       std::string::const_iterator & peek,
							       std::string::const_iterator max);
/* Called when we encounter the STRING_DENOTATION character. Extract's and returns string (dealing with escape
   character's.) Call's exit if there is an error */
std::string getString(std::string::const_iterator & current, std::string::const_iterator & peek,
		      const std::string::const_iterator max);
/* Increment's peek untill it point's to a non white space character. At which point proceeds to delete
   character's [current, peek) if (++current != peek). Return's false if peek becomes >= max while incrementing.
   It is assumed that current and peek point to consecutive elment's */
bool rubOutSpace(std::string & buff, std::string::const_iterator & current, std::string::const_iterator & peek,
	       std::string::const_iterator & max);


#endif
