#include <iostream>
#include "loadAssets.h"
#include "../collapse/collapse.h"


void loadAssets(const yx maxyx, std::vector<int> & background, rules & levelRules)
{
  try
    {
      {                         // Initialise background.
        std::string backgroundFileName {"assets/level1/level1.backgound.lev"};
        std::string levelBackGround {};
        if(!loadASCIIFile(backgroundFileName, levelBackGround))
          {
            throw std::logic_error("Error unable to open: \"./assets/level1/level1.background.lev\"");
          }
        collapse(levelBackGround, background); //collapse nonColor escape sequences.
      }
      // THESE SHOULD BE SPECIFIED IN THE HEADER OF THE RULES FILE!!!!!!!!!!!!!!!1 
      yx spritePos = {10, 10}; // Set inital position for player.
      levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
                                      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
                                      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player 
    }
  catch(std::logic_error errorMsg)
    {
      exit(errorMsg.what(), ERROR_OPENING_FILE);
    }
}



bool loadASCIIFile(const std::string & name, std::string & buff)
{
  std::ifstream file;
  file.open(name.c_str());
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
