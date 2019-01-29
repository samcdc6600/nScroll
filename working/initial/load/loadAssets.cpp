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
  /*  yx spritePos = {10, 10}; // Set inital position for player.
      levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player */
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
      bool inHeader {true};
      for(std::string::const_iterator peek {getAdvancedIter(buff.begin(), buff.end(), sizeof(HEADER_START) -1)},
	    current {peek++}; *peek != NULL_BYTE; ++peek, ++current)
	{
	  // endwin();
	  //	  std::cout<<"*current = "<<*current<<std::endl;
	  //	  std::cout<<"*peek = "<<*peek<<std::endl;
	  //	  std::cout<<"inHeader = "<<inHeader<<std::endl<<std::endl;
	  //	  char ch {};
	  //	  std::cin>>ch;
	  //	  sleep(1000);
	  
	  switchOnCurrent(maxyx, buff, current, peek, buff.end(), inHeader, levelRules);
	}
    }
  catch (std::out_of_range & e)
    {				/* We are using a specific error code here because we are only expecting to recive an
				   exception from the getAdvancedIter function. We had decided to remove all excption
				   handling from this program. However we are going to use it in this one case to kee
				   the getAdvancedIter function more general. */
      exit(e.what(), ERROR_RULES_LEV_HEADER);
    }
}


void switchOnCurrent(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		    std::string::const_iterator & peek, std::string::const_iterator max, bool & inHeader,
		    rules & levelRules)
{
  switch(*current)
    {
    case FIELD_START_DENOTATION:    // Handle start of new field or section.
      handleCurrentIsFieldStartDenotation(maxyx, buff, current, peek, max, inHeader, levelRules);
      break;
    case HEADER_END_DENOTATION:
      rubOutSpace(buff, current, peek, max); // Remove any space after HEADER_END_DENOTATION.
      inHeader = false;
      break;
    default:
      constexpr int SHOW_COUNT {45};
      std::stringstream e {};
      e<<"Error current character not expected at current position rules.lev. The following is the "<<SHOW_COUNT
       <<" character's after and including the current character (note output may be less then 15 character's if"
	" the EOF character was encountered):\n";
      for(int iter {SHOW_COUNT} ; *current != NULL_BYTE && iter > 0; --iter, ++current)
	{e<<*current;}
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  //endwin();
  //sleep(10000);
  //    exit("\n\nwe are free!\n", -1);
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -START- ------------------------------- -----------------------------


void handleCurrentIsFieldStartDenotation(const yx maxyx, std::string & buff, std::string::const_iterator & current,
				    std::string::const_iterator & peek, std::string::const_iterator max,
				    const bool inHeader, rules & levelRules)
{			// We have a sprite
  if(!inHeader)
    {
      switch(*peek)
	{
	case STRING_DENOTATION:
	  std::cout<<"\" character encountered."<<std::endl;
	  sleep(20000);
	  break;
	default:
	  if(*peek == BOARDER_CHAR			||	*peek == BOARDER_CHAR_UPPER
	     || *peek == KILL_CHAR			||	*peek == KILL_CHAR_UPPER
	     || *peek == DEGRADE_HEALTH_CHAR		||	*peek == DEGRADE_HEALTH_CHAR_UPPER
	     || *peek == DEGRADE_HEALTH_BOARDER_CHAR	||	*peek == DEGRADE_HEALTH_BOARDER_CHAR_UPPER
	     || *peek == END_LEV_CHAR			||	*peek == END_LEV_CHAR_UPPER
	     || *peek == LIFE_POWER_UP_CHAR		||	*peek == LIFE_POWER_UP_CHAR_UPPER)
	    {	      // Call function to handle coordinate character's character
	      //handleBoarderCharacter();
	    }
	  break;
	}
    }
  else			// Read in the player sprite info.
    initPlayerSprite(maxyx, buff, current, peek, max, levelRules);
  
  rubOutSpace(buff, current, peek, max);
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -END- ------------------------------- -----------------------------


void initPlayerSprite(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator & max, rules & levelRules)
{
  std::vector<std::string> sprites {handleStringDenotationAfterFieldDenotation(buff, current, peek, max)};
  rubOutSpace(buff, current, peek, max);
  current++, peek++;
  
  if(*current == FIELD_START_DENOTATION)
    {
      std::stringstream coordsSS {getCoords(buff, current, peek, max)};
      yx initPos {};		// Initial sprite position.
      coordsSS>>initPos.y;
      {				// We are only using c to skip the COORD_SEPERATION character!
	char c {};
	coordsSS>>c;
      }
      coordsSS>>initPos.x;
      levelRules.gamePlayer = (new player(sprites, maxyx, initPos, 25, DIR_RIGHT));
      ++current, ++peek;
    }
  else
    {
      std::stringstream e {};
      e<<"Error in rules.lev file header, encountered end of string field (denoted by \""<<FIELD_END_DENOTATION
       <<"\") after string. But did not then encounter sprite coordinates (denoted by \""<<FIELD_START_DENOTATION
       <<".\"";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


std::string getCoords(const std::string & buff, std::string::const_iterator & current,
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
