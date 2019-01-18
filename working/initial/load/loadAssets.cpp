#include <iostream>
#include "loadAssets.h"
#include "../collapse/collapse.h"


void loadAssets(const yx maxyx, const char bgFileName [], std::vector<int> & background,
		const char rulesFileName [], rules & levelRules)
{
  // Initialise background.
  //constexpr char backgroundFileName [] = "assets/level1/level1.backgound.lev";
  std::string levelBackGround {};
  if(!loadASCIIFile(bgFileName, levelBackGround))
    {
      exit("Error unable to open: \"./assets/level1/level1.background.lev\"", ERROR_OPENING_FILE);
    }
  collapse(levelBackGround, background); //collapse nonColor escape sequences.
  // Initialise non-player sprites.
  
  // Initialise player.
  // THESE SHOULD BE SPECIFIED IN THE HEADER OF THE RULES FILE!!!!!!!!!!!!!!!1 
  yx spritePos = {10, 10}; // Set inital position for player.
  levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
                                      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
                                      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player 
}



bool loadASCIIFile(const char name [], std::string & buff)
{
  std::ifstream file;
  file.open(name);
  if(!file.is_open())
    {				// Couldn't open file.
      file.close();
      return false;
    }
  char c;
  while(file.get(c))		// Copy file content's to buff.
    buff.push_back(c);
  file.close();
  
  return true;
}

/*void handleRulesFile(cosnt )
{
}*/
