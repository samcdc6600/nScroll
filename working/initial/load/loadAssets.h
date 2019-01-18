#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "../../physics/physics.h" // For rules.


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
void handleRulesFile(const char rulesFileName []);


#endif
