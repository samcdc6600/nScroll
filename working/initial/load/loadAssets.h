#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "../../physics/physics.h" // For rules.


/* This function should initialise the background variable (it's second argument), which should then contain the 
   "grphical data" of the level. It should initialise and return the levelRules variable (it's third argument),
   which should then contain the "rules and non player sprites" as well as the player sprite of the level. */
void loadAssets(const yx maxyx, std::vector<int> & background, rules & levelRules);
/* Opens file "name" and copies it's content's into buff. If there is an error opening the file returns false.
   Return's true otherwise. */
bool loadASCIIFile(const std::string & name, std::string & buff);


#endif
