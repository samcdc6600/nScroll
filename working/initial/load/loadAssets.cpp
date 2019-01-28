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
parse(maxyx, buff, rulesFileName, levelRules);
    // Initialise player.
  yx spritePos = {10, 10}; // Set inital position for player.
  levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
                                      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
                                      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player 
}


void parse(const yx maxyx, std::string & buff, const char rulesFileName [], rules & levelRules)
{
  if(strncmp(buff.c_str(), HEADER_START, sizeof(HEADER_START) -1) != 0)
    {
      std::stringstream e {};
      e<<"Error header of the .rules.lev file \""<<rulesFileName<<"\" is malformed!";
      exit(e.str(), ERROR_RULES_LEV_HEADER);
    }


  try
    {
      for(std::string::const_iterator peek {getAdvancedIter(buff.begin(), buff.end(), sizeof(HEADER_START) -1)},
	    current {peek++}; *peek != '\0'; ++peek, ++current)
	{
	 bool inHeader {true};
	 switch(switchOnCurrent(maxyx, buff, current, peek, buff.end(), inHeader))
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


int switchOnCurrent(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator max, bool inHeader)
{	    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  endwin();			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TEST CODE
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 
  switch(*current)
    {
    case FIELD_START_DENOTATION:    // Handle start of new field or section.
      handleCurrentIsFieldDenotation(maxyx, buff, current, peek, max, inHeader);
      break;
      
    default:
      if(*peek == HEADER_END_DENOTATION) // We've reached the end of the header (or file is malformed.)
	{}
    }

    sleep(10000);
    exit("\n\nwe are free!\n", -1);
  return 0;
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -START- ------------------------------- -----------------------------


void handleCurrentIsFieldDenotation(const yx maxyx, std::string & buff, std::string::const_iterator & current,
				    std::string::const_iterator & peek, std::string::const_iterator max,
				    const bool inHeader)
{
  if(*peek != STRING_DENOTATION)
    {			// We have a coordinate character
      // Char c = validateCoordCharacter()
      // Call function to get the coordinate's.
      // Add info to coordinate rules map.
    }
  else
    {			// We have a sprite
      if(!inHeader)
	{
	  // Call function to get the the rule.
	  // Call function to get the coordinate's.
	}
      else			// Read in the player sprite info.
	initPlayerSprite(maxyx, buff, current, peek, max);
    }
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -END- ------------------------------- -----------------------------


void initPlayerSprite(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator & max)
{
  std::vector<std::string> sprites {handleStringDenotationAfterFieldDenotation(buff, current, peek, max)};
  rubOutSpace(buff, current, peek, max);
  current++, peek++;
  
  if(*current == FIELD_START_DENOTATION)
    {				/* Currently maxyx hold's coordinates that can allow the player sprite to be
				   somewhat offscreen. This should be fixed at some point :3. */
      std::stringstream coordsSS {getCoords(maxyx, buff, current, peek, max)};

      // ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!
      // ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!
      // ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!
      char c {};
      yx coords {};
      coordsSS>>c;
      coords.y = (c - ASCII_NUMBER_OFFSET);
      while((coordsSS>>c) != COORD_SEPERATION)
	{
	  coords.y *= 10;
	  coords.y = (c - ASCII_NUMBER_OFFSET);
	}
      ++current, ++peek;
      coordsSS>>c;
      coords.x = (c - ASCII_NUMBER_OFFSET);
      while((coordsSS>>c) != COORD_SEPERATION)
	{
	  coords.x *= 10;
	  coords.x = (c - ASCII_NUMBER_OFFSET);
	}
	// ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!
	// ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!
	// ATTENTION!!! MOVE MOST OF THIS CODE INTO A GENERAL FUNCTION IN COMMON.CPP!!!!!!!!!!!!!!!!!!!!!!!!!


	
      // We are ready to initialise the payer :).
    }
  else
    {
      std::stringstream e {};
      e<<"Error in rules.lev file header, encountered end of string field (denoted by \""<<FIELD_END_DENOTATION
       <<"\") after string. But did not then encounter sprite coordinates (denoted by \""<<FIELD_START_DENOTATION
       <<".\"";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
     // Init player with coord from next fun.

	  /*
  yx spritePos = {10, 10}; // Set inital position for player.
  levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
                                      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
                                      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player  */
}


/* Check's for coordinate of the form "(Y,X)" and return's "Y,X" (discarding "(" and ")") if Y and X are valid
   integer number's that do not falloutside of the X and Y values in maxYX and are greater or equal to zero.  */
std::string getCoords(const yx maxyx, const std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, const std::string::const_iterator & max)
{
  std::string ret;  
  ++current, ++peek;		// Advance current to first digit.

  if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
    {				// Error!
      std::stringstream e {};
      e<<"Error parsing coordinate, Y coordinate malformed! Coordinats must be of the form \"Y"<<COORD_SEPERATION
       <<"X\", where Y and X are integer numbers.";
	exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
    }
  
  while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
    {				// Get Y coordinate.
      ret.push_back(*current);
      ++current, ++peek;	// Next character.
    }
  
  if(*current == COORD_SEPERATION)
    {
      ret.push_back(*current);	// COORD_SEPERATION is part of our coords.
      ++current, ++peek;
      
      if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
	{				// Error!
	  std::stringstream e {};
	  e<<"Error parsing coordinate, X coordinate malformed! Coordinats must be of the form \"Y"<<COORD_SEPERATION
	   <<"X\", where Y and X are integer numbers.";
	  	exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
	}
      
      while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
	{			// Get X coordinate.
	  ret.push_back(*current);
	  ++current, ++peek;
	}
    }
  else
    {				// Error!
      std::stringstream e {};
      e<<"Error parsing coordinate, missing \""<<COORD_SEPERATION<<"\" character or non integer character before"
	" \""<<COORD_SEPERATION<<"\" character.";
      exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
    }

  if(*current != FIELD_END_DENOTATION)
    {	  
      std::stringstream e {};
      e<<"Error in rules.lev file. Encountered character other then \""<<FIELD_END_DENOTATION
       <<"\" or integer character while parsing X coordinate.";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  std::cout<<ret<<std::endl;
  return ret;
}

std::vector<std::string> handleStringDenotationAfterFieldDenotation(std::string & buff,
							       std::string::const_iterator & current,
							       std::string::const_iterator & peek,
							       std::string::const_iterator max)
{
  std::vector<std::string> ret {};
        do
	{	  
	  ret.push_back(std::string {getString(++current, ++peek, max)}); // Get the sprite path.
	  
	  if(std::isspace(*current)) // Check for spaces after string
	    {
	      --current, --peek;
	      rubOutSpace(buff, current, peek, max); // Remove leading space before STRINGS_SEPERATION.
	      ++current, ++peek; // Set current to STRINGS_SEPERATION.
	    }
	  rubOutSpace(buff, current, peek, max); // Remove trailing space after STRINGS_SEPERATION.

	  
	  if(*current != STRINGS_SEPERATION)
	    {
	      if(*current == FIELD_END_DENOTATION)
		break;		// We've read all the little stringy wingy's in :).
	      else
		{			  // Didn't encounter STRING_SEPERATION or FIELD_END_DENOTATION.
		  std::stringstream e {}; // We encountered was a syntax error.
		  e<<"Error encountered a character other then '"<<STRING_DENOTATION<<"' or '"<<FIELD_END_DENOTATION
		   <<"' when reading string's and between string's";
		  exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
		}
	    }
	  else
	    {
	      if(*peek == STRING_DENOTATION)
		continue;
	      else
		{			// Encounterd STRING_SEPERATION but no STRING_DENOTATION.
		  std::stringstream e {};
		  e<<"Error in field containig string/s found '"<<STRINGS_SEPERATION
		   <<"' that was not followed by '"<<STRING_DENOTATION<<'\'';
		  exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
		}
	    }
	  // Initalise sprite and add info to sprite map.
	}
      while(*current == STRINGS_SEPERATION); // There can be more then one sprite

  return ret;
}


std::string getString(std::string::const_iterator & current, std::string::const_iterator & peek,
		      const std::string::const_iterator max)
{
  std::string ret {};
  ++current, ++peek;		// Skip inital STRING_DENOTATION character.
  while(true)
    {
      if(peek == max)
	{
	  std::stringstream e {};
	  e<<"Error parsing string, we have reached the end of the file but have not encountred the appropriate"
	    "character's. A rules.lev file must end with the character's \""<<STRING_DENOTATION<<FIELD_END_DENOTATION
	   <<"\", if the last peice of information in the last field in the file is a string.";
	  exit(e.str().c_str(), ERROR_MALFORMED_STRING);
	}
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
	      {
		std::stringstream e {};
		e<<"Error parsing string, encountered '"<<ESCAPE_CHAR<<"' but did not encounter '"<<ESCAPE_CHAR<<
		  "' or '"<<STRING_DENOTATION<<"' following it.";
		exit(e.str().c_str(), ERROR_MALFORMED_STRING);
	      }
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


bool rubOutSpace(std::string & buff, std::string::const_iterator & current, std::string::const_iterator & peek,
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
       current = buff.erase(++current, peek); // Why ++current? Erases the sequnece in the range [first, last).
       --current; // Set current to element it was pointing to uppon entering function.
       peek = current, ++peek;	// Reset peak.
       max = buff.end();		// Reset max.
     }
  
  return true;
}
