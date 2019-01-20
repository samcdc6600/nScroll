#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "../../physics/physics.h" // For rules.


constexpr char FIELD_DENOTATION {'('};	 // Marks the start of a new field or section of the file.
constexpr char HEADER_END_DENOTATION {'#'}; // Marks the end of the header sectino of the file.
constexpr char STRING_DENOTATION {'"'}; // This character denotes the start of a sting
 // This character denotes the presence of another string when used after a string.
constexpr char STRINGS_SEPERATION {','};
constexpr char ESCAPE_CHAR {'\\'}; // The character used for escape sequences (within a string) in .rules.lev file's.


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
void parse(const std::string & buff, const char rulesFileName [], rules & levelRules);
/* Branch to differnt parsing function's depending on the values of current and peek. string may be modified to hold
   the value of a string if the STRING_DENOTATION character is encountered and we are not already in a sring.
   InHeader tells whether we are in the header (parsing decisions may be different depending on it's state.) */
int switchOnChars(std::string::const_iterator & current, std::string::const_iterator & peek,
		   const std::string::const_iterator & max, bool inHeader);
/* Called when we encounter the STRING_DENOTATION character. Extract's and returns string (dealing with escape
   character's.) Call's exit if there is an error */
std::string getString(std::string::const_iterator & current, std::string::const_iterator & peek,
		      const std::string::const_iterator max);
/* Return true if we encounter the sequence "ESCAPE_CHAR ESCAPE_CHAR", where ESCAPE_CHAR is the escape sequence
   character and there is no space */
//bool checkForDoubleEscape(const std::string::const_iterator current, const std::string::const_iterator peek);
/* Return true if peek points to the character ESCAPE_CHAR */
//bool checkPeekForESCAPE_CHAR(const std::string::const_iterator peek);
/* Increment's current untill it encounter's a non white space character (also increment's peak.) If current is
   equal to max at any point return false. */
bool skipSpace(std::string::const_iterator & current, std::string::const_iterator & peek,
	       const std::string::const_iterator max);


#endif
