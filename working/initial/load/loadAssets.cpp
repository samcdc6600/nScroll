#include <iostream>
#include <sstream>
#include <cstring>
//#include <locale>
#include <ncurses.h>// test code
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
      std::stringstream a {};
      a<<"Error unable to open: \""<<bgFileName<<"\"";
      exit(a.str(), ERROR_OPENING_FILE);
    }
  collapse(levelBackGround, background); //collapse nonColor escape sequences.
  // Initialise player and non-player sprites (rules file).
  parseAndInitialiseRules(maxyx, rulesFileName, levelRules);
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


void parseAndInitialiseRules(const yx maxyx, const char rulesFileName [], rules & levelRules)
{
  std::string buff {};
  if(!loadASCIIFile(rulesFileName, buff)) // Load in the rules file.
    {
      std::stringstream e {};
      e<<"Error unable to open: \""<<rulesFileName<<"\"";
      exit(e.str(), ERROR_OPENING_FILE);
    }
  parse(buff, rulesFileName, levelRules);
    // Initialise player.
  // THESE SHOULD BE SPECIFIED IN THE HEADER OF THE RULES FILE!!!!!!!!!!!!!!!1 
  yx spritePos = {10, 10}; // Set inital position for player.
  levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
                                      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
                                      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player 
}


void parse(std::string & buff, const char rulesFileName [], rules & levelRules)
{
  if(strncmp(buff.c_str(), HEADER_START, sizeof(HEADER_START) -1) != 0)
    {
      std::stringstream e {};
      e<<"Error header of the .rules.lev file \""<<rulesFileName<<"\" is malformed!";
      exit(e.str(), ERROR_RULES_LEV_HEADER);
    }


  try
    {
      for(std::string::iterator peek {getAdvancedIter(buff.begin(), buff.end(), sizeof(HEADER_START) -1)},
	    current {peek++}; *peek != '\0'; ++peek, ++current)
	{
	  bool inHeader {true};	  
	  switch(switchOnChars(buff, current, peek, buff.end(), inHeader))
	    {
	    default:
	      break;
	    }
	}
    }
  catch (std::out_of_range & e)
    {				/* We are using a specific error code here because we are only expecting to recive an
				   exception from the getAdvancedIter function. We had decided to remove all excption
				   handling from this program. However we are going to use it in this one case to kee
				   the getAdvancedIter function more general. */
      exit(e.what(), ERROR_RULES_LEV_HEADER);
    }
  // now we need to write a function to check for the two occurences of "("
  // or maybe think about whether we really want to deal with that beacuse it might be harder then I had though.
  // I'll have to think about!
}


int switchOnChars(std::string & buff, std::string::iterator & current, std::string::iterator & peek,
		  std::string::const_iterator max, bool inHeader)
{
  std::string str {};
  switch(*current)
    {
    case FIELD_DENOTATION:    // Handle start of new field or section.
      if(*peek != STRING_DENOTATION) // Likely path!
	{			// We have a coordinate character
	  // Char c = validateCoordCharacter()
	  // Call function to get the coordinate's.
	  // Add info to coordinate rules map.
	}
      else
      	{			// We have a sprite	  
	  // Call function to the the rule.
	  // Call function to get the coordinate's.
	  do
	    {
	      if(*peek != STRING_DENOTATION)
		exit("Error encountered the character ',' after a string but did not encounter another sting!",
		     ERROR_MALFORMED_STRING);
	      str = getString(++current, ++peek, max); // Get the sprite path.
	      //std::cout<<"*current = "<<*current<<", peek = "<<(int)*peek<<std::endl;
	      //std::cout<<"str = "<<str<<std::endl;
	      //std::cout<<"*current = "<<*current<<", peek = "<<(int)*peek<<std::endl;
	      rubOutSpace(buff, current, peek, max); // There can be spaces inbetween the ',' characters.
	      //std::cout<<"*current = "<<*current<<", peek = "<<(int)*peek<<std::endl;
	      if(*current != STRINGS_SEPERATION)
		break;
	      else
		if(*peek == STRING_DENOTATION)
		  continue;
		else
		  {
		    //std::cout<<"*current = "<<*current<<", peek = "<<(int)*peek<<std::endl;
		    rubOutSpace(buff, ++current, ++peek, max);
		    //std::cout<<"*current = "<<*current<<", peek = "<<(int)*peek<<std::endl;
		    --current, --peek; // Peek should point to STRING_DENOTATION character.
		  }
	      // Initalise sprite and add info to sprite map.
	    }
	  while(*current == STRINGS_SEPERATION); // There can be more then one sprite
	}
      break;
      
    default:
      if(*peek == HEADER_END_DENOTATION) // We've reached the end of the header (or file is malformed.)
	{}
    }
  return 0;
}


std::string getString(std::string::iterator & current, std::string::iterator & peek,
		      const std::string::const_iterator max)
{
  std::string ret {};
  ++current, ++peek;		// Skip inital STRING_DENOTATION character.
  while(true)
    {
      if(peek == max)
	exit("Error parsing string, (peek equals max.) A rules.lev file cannot end with \"X\"\" or \"XY\", where X"
	     " and Y are arbitrary characters and where XY is not the sequence \"\")\". If the last peice of"
	     " information in the file is a string it must end \"\")\"",
	     ERROR_MALFORMED_STRING);
      switch(*current)
	{
	case ESCAPE_CHAR:
	  if(*peek == ESCAPE_CHAR)
	    {			// The ESCAPE_CHAR character has been escaped.
	      ret.push_back(ESCAPE_CHAR);
	      ++current, ++peek;
	    }
	  else
	    if(*peek == STRING_DENOTATION)
	      {			// The STRING_DENOTATION character has been escaped.
		ret.push_back(STRING_DENOTATION);
		++current, ++peek;
	      }
	    else
	      exit("Error parsing string, encountered \\ but did not encounter \\ or \" following it.",
		   ERROR_MALFORMED_STRING);
	  break;
	  
	case STRING_DENOTATION:	// We've reached the end of the string :^).
		++current, ++peek;		// Current should be after the STRING_DENOTATION character.
	  return ret;
	
	default:
	  ret.push_back(*current);	  
	}
      ++current, ++peek;
    }
}


bool rubOutSpace(std::string & buff, std::string::iterator & current, std::string::iterator & peek,
	       std::string::const_iterator & max)
{
  bool foundSp {false};
   while(std::isspace(*peek))
    {
      ++peek;
      foundSp = true;
      if(peek >= max)
	return false;
    }
    
  if(foundSp)			// Only erase if we found space.
    {
      std::cout<<"before"<<std::endl;
      current = buff.erase(++current, peek); // Why ++current? Erases the sequnece in the range [first, last).
      std::cout<<"After"<<std::endl<<std::endl;
    }
  peek = current, ++peek;
  max = buff.end();		// Reset max.
  
  return true;
}
