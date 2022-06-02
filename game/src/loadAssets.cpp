#include <iostream>
#include <sstream>
#include <cstring>
#include <ncurses.h>
#include "include/utils.hpp"
#include "include/loadAssets.hpp"
#include "include/collapse.hpp"


void loadAssets
(const yx maxyx, const char bgFileName [], std::vector<int> & background,
 const char rulesFileName [], rules & levelRules)
{
  // Initialise background array.
  loadAndParseBackgroundFile(maxyx, bgFileName, background);
  /* Initialise player and non-player sprites (rules file) and initialise
     levelRules array. */
  loadAndParseLevelRulesFile(maxyx, rulesFileName, levelRules,
			     background.size());
}


void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				std::vector<int> & background)
{
  std::string levelBackGround {};
  if(!loadFileIntoString(bgFileName, levelBackGround))
    {
      std::stringstream err {};
      err<<"Error: unable to open \""<<bgFileName<<"\".";
      exit(err.str(), ERROR_OPENING_FILE);
    }
  collapse(levelBackGround, background); //collapse nonColor escape
  //sequences.
  if((background.size() % (maxyx.x * maxyx.y)) != 0)
    {
      std::stringstream err {};
      err<<"Error: wrong number of characters in background after collapsing "
	"escaped characters. (background size (after processing) % (window "
	"length * window height)) != 0, where background size = "
	 <<background.size()<<", window length = "<<maxyx.x<<" and window "
	"height = "<<maxyx.y<<".";
      exit(err.str(), ERROR_BACKGROUND);
    }
}


void loadAndParseLevelRulesFile
(const yx maxyx, const char rulesFileName [], rules & levelRules,
 const size_t bgSize)
{
  std::string rawRules {};
  if(!loadFileIntoString(rulesFileName, rawRules))
    {
      std::stringstream e {};
      e<<"Error: unable to open \""<<rulesFileName<<"\".";
      exit(e.str(), ERROR_OPENING_FILE);
    }
  parseRulesHeader(maxyx, rulesFileName, levelRules, bgSize, rawRules);
}


void parseRulesHeader(const yx maxyx, const char rulesFileName[],
			  rules & levelRules, const size_t bgSize,
			  const std::string & rawRules)
{
  using namespace levelFileStrings;
  // if(strncmp(rawRules.c_str(), RULES_HEADER_START_DENOTATION,
  // 	     sizeof(RULES_HEADER_START_DENOTATION) -1) != 0)
  //   {
  //     std::stringstream e {};
  //     e<<"Error: header of the .rules.lev file \""<<rulesFileName
  //      <<"\" is malformed!";
  //     exit(e.str(), ERROR_RULES_LEV_HEADER);
  //   }

  std::string::const_iterator buffPos {rawRules.begin()};
  initPlayer(maxyx, rulesFileName, levelRules, rawRules, buffPos);

  // std::string::const_iterator buffPos
  //   {getAdvancedIter(rawRules.begin(), rawRules.end(),
  // 		     sizeof(RULES_HEADER_START_DENOTATION),
  // 		     std::string {"Error: header of the .rules.lev file is "
  // 		       "malformed. input ended unexpectedly."})};

  // initPlayerSprite(readPlayerSpriteFileNames);
  // //initOtherSprites(readOtherSpriteFileNames)
  // std::vector<std::string> targets {RULES_HEADER_START_DENOTATION};
  // std::string targetFound {};


  // endwin();
  // targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  // std::cout<<"targetFound = "<<targetFound<<'\n';
  // if(buffPos == rawRules.end())
  //   {
  //     std::cout<<"buffPos = end()\n";
  //   }
  // std::cout<<"*buffPos = "<<*buffPos<<'\n';
  // exit(-1);

  
	// current {peek++}; *peek != NULL_BYTE; ++peek, ++current)
	// {
	  // std::stringstream e {};
	  // e<<"*current = "<<*current<<", *peek = "<<*peek<<'\n';
	  // exit(e.str().c_str(), 0);
	  // if(*current == STRING_DENOTATION && *peek == 
	  // // switchOnCurrent(maxyx, rawRules, current, peek, rawRules.end(), levelRules,
	  // // 		  bgSize);
	// }
}


void initPlayer(const yx maxyx, const char rulesFileName[], rules & levelRules,
		const std::string & rawRules,
		std::string::const_iterator & buffPos)
{
  using namespace levelFileStrings;

  std::vector<std::string> targets {RULES_HEADER_START_DENOTATION};
  std::string targetFound {};
  
  targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  targets.clear();

  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_START_DENOTATION<<"\" first when "
	"reading rules.lev file \""<<rulesFileName<<"\".";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  std::vector<std::string> spriteFileDirectories
    {readStringsSection(rawRules, buffPos, "reading sprite file directory strings")};

  endwin();
  std::cout<<"spriteFileDirectories = \n";
  for(auto dir: spriteFileDirectories)
    {
      std::cout<<dir<<'\n';
    }
  exit(-1);
  

  /* Read in all valid strings then read in coords then pass to new player
     object. Player object should check for correct number of strings. (MAKE
     SURE THIS IS WHAT'S ACTUALLY
     HAPPENING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!) */
  
  
  // levelRules.gamePlayer =
  // 	(new player(sprites, maxyx, initPos, sprite::DIR_NONE, 25, -0.38, 1.9,
  // 		    1, 3))
}


std::vector<std::string> readStringsSection(const std::string & buff,
				     std::string::const_iterator & buffPos,
				     const std::string & eMsg)
{
  using namespace levelFileStrings;

  std::vector<std::string> strings {};
  std::vector<std::string> targets {};
  std::string targetFound {};

  endwin();

  while(true)
    { 
      targets.push_back(std::string {STRING_START_DENOTATION});
      // Skip to start of string.
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      targets.clear();
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \""<<STRING_START_DENOTATION<<"\" to start a "
	    "string when "<<eMsg<<".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
      

      if(*buffPos != STRING_END_DENOTATION)
	{
	  // WE DON'T HAVE AN EMPTY STRING, SO READ IN STRING.
	  std::string newString {};		
	  do
	    {
	      std::string::const_iterator peekBuffPos {buffPos};
	      peekBuffPos++;
	      
	      newString.push_back(*buffPos);
	      
	      if(*buffPos != STRING_ESC &&
		 *peekBuffPos == STRING_END_DENOTATION)
		{
		  // We've reached the end of the string.
		  break;
		}
	      else if(peekBuffPos == buff.end())
		{
		  std::stringstream e {};
		  e<<"Error: file ended unexpectedly when "<<eMsg<<".\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      buffPos = peekBuffPos;
	    }
	  while(true);
	  strings.push_back(newString);
	  newString.clear();
	}
      // ++ once to catch up with peekBuffPos and once again to surpass it.
      ++buffPos;		
      ++buffPos;

      targets.push_back(std::string {STRING_SEPARATION});
      targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
      // Skip to string separation.
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      targets.clear();
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \""<<STRING_SEPARATION<<"\" or \""
	   <<RULES_HEADER_SECTION_END_DENOTATION<<"\" to separate strings or end "
	    "the string section when "<<eMsg<<".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
      else if(targetFound == std::string {RULES_HEADER_SECTION_END_DENOTATION})
	{
	  // We've read all the strings in the string section.
	  break;
	}
    }

  return strings;


  //   endwin();
  // std::cout<<"targetFound = "<<targetFound<<", *buffPos = "<<*buffPos<<"\n";
  // exit(-1);
}
// void switchOnCurrent
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max,
//  rules & levelRules, const size_t bgSize, bool & inHeader)
// {
//   if(inHeader)
//     {
//       switch(*current)
// 	{
// 	case FIELD_START_DENOTATION:    // Handle start of new field or section.
// 	  handleCurrentIsFieldStartDenotation
// 	    (maxyx, buff, current, peek, max, levelRules, bgSize);
// 	  break;
// 	default:
// 	  std::stringstream e {};
// 	  e<<"Error: current character not expected at current position in the "
// 	    "rules.lev file. The following is the "<<SHOW_COUNT<<" character's "
// 	    "after and including the current character (note output may be less "
// 	    "then "<<SHOW_COUNT<<" character's if the EOF character was "
// 	    "encountered):\n";
// 	  printCurrent(current, SHOW_COUNT, e);
// 	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 	}
//     }
//   else
//     {
//       // Read in the player sprite info.
//       initPlayerSprite(maxyx, buff, current, peek, max, levelRules, bgSize);
//       /* Make sure there's no space between end of player field and
// 	 HEADER_END_DENOTATION character. */
//       rubOutSpace(buff, current, peek, max); 
//       ++current, ++peek; // Move peek past HEADER_END_DENOTATION character.
//       if(*current != HEADER_END_DENOTATION)
// 	{
// 	  std::stringstream e {};
// 	  e<<"Error: expected but did not see \""<<HEADER_END_DENOTATION<<"\" "
// 	    "character to denote the end of the header.";
// 	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 	}
//       /* Make sure there's no space between HEADER_END_DENOTATION and next
// 	 field. */
//       //      rubOutSpace(buff, current, peek, max);
//       inHeader = false; // We are leaving the header.
//     }
// }


// // FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -START- -----------------------
// void handleCurrentIsFieldStartDenotation
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max,
//  rules & levelRules, const size_t bgSize)
// {			// We have a sprite
//   switch(*peek)
//     {
//     case STRING_DENOTATION:
//       endwin();
//       std::cout<<"String denotation character encountered after start "
// 	"denotation (not in header.)\n";
//       sleep(10000);
//       break;
//     default:
//       if(*peek == boarderRuleChars::boarderChar ||
// 	 *peek == boarderRuleChars::platformChar)
// 	{ // Call function to handle coordinate character's character
// 	  ++current, ++peek;
// 	  handleCoordinateCharacter(buff, current, peek, max, levelRules,
// 				    bgSize);
// 	  /* Make sure there's no space between end of coordinate character
// 	     field and next field (if any.) */
// 	  rubOutSpace(buff, current, peek, max);
// 	}
//       else
// 	{
// 	  std::stringstream e {};
// 	  e<<"Error: expected but did not encounter coordinate character. "
// 	    "The following is the "<<SHOW_COUNT<<" character's after and "
// 	    "including the current character (note output may be less then"
// 	   <<SHOW_COUNT<<" character's if the EOF character was "
// 	    "encountered):\n";
// 	  printCurrent(current, SHOW_COUNT, e);
// 	  exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE_CHAR_FIELD);
// 	}
//       break;
//     }
// }
// // FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -END- -------------------------


// /* Read in the character *current (which should already have been checked for
//    validity), then read in the coordinate pair, finally the function should
//    check to see that this coordinate pair is unique in the object
//    levelRules.charCoords and if it is use the coordinates as a key to store the
//    character (which is to be interprited as a Coordinate character */
// void handleCoordinateCharacter
// (std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator & max,
//  rules & levelRules, const size_t bgSize)
// { // Get coordinate char (should be pre checked for validity.)
//   const char c {*current};
//   /* Make sure there is no space between coordinate char and
//      FIELD_START_DENOTATION for the coordinate field. */
//   rubOutSpace(buff, current, peek, max);
//   if(*peek != FIELD_START_DENOTATION)
//     {
//       std::stringstream e {};
//       e<<"Error: encountered coordinate character but did not encounter "
//        <<FIELD_START_DENOTATION<<" character.";
//       exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
//     }
//   // Set current to FIELD_START_DENOTATION character position.
//   ++current, ++peek;
//   std::string key {getCoords(current, peek, bgSize)};
//   /* Make sure there's no space between the end of the coordinate field and the
//      end of the coordinate character field. */
//   rubOutSpace(buff, current, peek, max);
//   if(levelRules.coordChars.find(key) == levelRules.coordChars.end())
//     { // Key not found
//       // Add our beautiful key, coordinate character pair to the coordChars map.
//       levelRules.coordChars[key] = c;
//     }
//   else
//     { // Error duplicate key's in rules.lev file.
//       std::stringstream e {};
//       e<<"Error: duplicate character coordinate's ("<<key<<") encountered in "
// 	"rules.lev file.";
//       exit(e.str().c_str(), ERROR_DUPLICATE_COORDINATE);
//     }
//   /* Advance peek past end of coordinate character field and thus past
//      FIELD_END_DENOTATION character. */
//   ++current, ++peek;
// }


// void initPlayerSprite
// (const yx maxyx, std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator & max,
//  rules & levelRules, const size_t bgSize)
// {
//   std::vector<std::string> sprites
//     {handleStringDenotationAfterFieldDenotation(buff, current, peek, max)};
//   // Make sure there's no space between string field and coordinate field.
//   rubOutSpace(buff, current, peek, max);
//   current++, peek++;
  
//   if(*current == FIELD_START_DENOTATION)
//     {
//       std::stringstream coordsSS {getCoords(current, peek, bgSize)};
//       yx initPos {};		// Initial sprite position.
//       coordsSS>>initPos.y;
//       { // We are only using c to skip the COORD_SEPERATION character!
// 	char c {};
// 	coordsSS>>c;
//       }
//       coordsSS>>initPos.x;
//       levelRules.gamePlayer =
// 	(new player(sprites, maxyx, initPos, sprite::DIR_NONE, 25, -0.38, 1.9,
// 		    1, 3));
//       ++current, ++peek;
//     }
//   else
//     {
//       std::stringstream e {};
//       e<<"Error: in rules.lev file header, encountered end of string field ("
// 	"denoted by \""<<FIELD_END_DENOTATION<<"\") after string. But did not "
// 	"then encounter sprite coordinates (denoted by \""
//        <<FIELD_START_DENOTATION<<"\".)";
//       exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
//     }
// }


// std::string getCoords(std::string::const_iterator & current,
// 		      std::string::const_iterator & peek, const size_t bgSize)
// {
//   std::string ret;  
//   ++current, ++peek;		// Advance current to first digit.

//   if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
//     {				// Error!
//       std::stringstream e {};
//       e<<"Error: parsing coordinate, Y coordinate malformed! Coordinats must be "
// 	"of the form \"Y"<<COORD_SEPERATION<<"X\", where Y and X are integer "
// 	"numbers.";
//       exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
//     }
  
//   while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
//     {				// Get Y coordinate.
//       ret.push_back(*current);
//       ++current, ++peek;	// Next character.
//     }
  
//   if(*current == COORD_SEPERATION)
//     {
//       ret.push_back(*current);	// COORD_SEPERATION is part of our coords.
//       ++current, ++peek;
      
//       if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
// 	{				// Error!
// 	  std::stringstream e {};
// 	  e<<"Error: parsing coordinate, X coordinate malformed! Coordinats must"
// 	    "be of the form \"Y"<<COORD_SEPERATION<<"X\", where Y and X are "
// 	    "integer numbers.";
// 	  exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
// 	}
      
//       while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
// 	{			// Get X coordinate.
// 	  ret.push_back(*current);
// 	  ++current, ++peek;
// 	}
//     }
//   else
//     {				// Error!
//       std::stringstream e {};
//       e<<"Error: parsing coordinate, missing \""<<COORD_SEPERATION<<"\" "
// 	"character or non integer character before \""<<COORD_SEPERATION<<"\" "
// 	"character.";
//       exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
//     }

//   if(*current != FIELD_END_DENOTATION)
//     {	  
//       std::stringstream e {};
//       e<<"Error: in rules.lev file. Encountered character other then \""
//        <<FIELD_END_DENOTATION<<"\" or integer character while parsing X "
// 	"coordinate.";
//       exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
//     }
//   else
//     {
//       int y, x;
//       char charNull;
//       std::stringstream coord {ret};
//       coord>>y;
//       coord>>charNull; // Skip the current character (should be ",").
//       coord>>x;

//       if(y >= yHeight)
// 	{
// 	  std::stringstream e {};
// 	  e<<"Error: in rules.lev file. Encountered y coordinated component "\
// 	    "that is out of range for coordinate ("<<y<<", "<<x<<").\n";
// 	  exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
// 	}
//       else if (x >= (bgSize / yHeight))
// 	{
// 	  std::stringstream e {};
// 	  e<<"Error: in rules.lev file. Encountered x coordinated component "\
// 	    "that is out of range for coordinate ("<<y<<", "<<x<<").\n";
// 	  exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
// 	}
//     }
  
//   return ret;
// }

// std::vector<std::string>
// handleStringDenotationAfterFieldDenotation
// (std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator max)
// {
//   std::vector<std::string> ret {};
//   do
//     { // Get the sprite path.
//       ret.push_back(std::string {getString(++current, ++peek, max)});
	  
//       if(std::isspace(*current)) // Check for spaces after string
// 	{
// 	  --current, --peek;
// 	  // Remove leading space before STRINGS_SEPERATION.
// 	  rubOutSpace(buff, current, peek, max);
// 	  ++current, ++peek; // Set current to STRINGS_SEPERATION.
// 	}
//       // Remove trailing space after STRINGS_SEPERATION.
//       rubOutSpace(buff, current, peek, max);

//       if(*current != STRINGS_SEPERATION)
// 	{
// 	  if(*current == FIELD_END_DENOTATION)
// 	    { // We've read all the little stringy wingy's in :).
// 	      break;
// 	    }
// 	  else
// 	    { // Didn't encounter STRING_SEPERATION or FIELD_END_DENOTATION.
// 	      std::stringstream e {}; // We encountered was a syntax error.
// 	      e<<"Error: encountered a character other then '"<<STRING_DENOTATION
// 	       <<"' or '"<<FIELD_END_DENOTATION<<"' when reading string's and "
// 		"between string's";
// 	      exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
// 	    }
// 	}
//       else
// 	{
// 	  if(*peek == STRING_DENOTATION)
// 	    continue;
// 	  else
// 	    { // Encounterd STRING_SEPERATION but no STRING_DENOTATION.
// 	      std::stringstream e {};
// 	      e<<"Error: in field containig string/s found '"<<STRINGS_SEPERATION
// 	       <<"' that was not followed by '"<<STRING_DENOTATION<<'\'';
// 	      exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
// 	    }
// 	}
//       // Initalise sprite and add info to sprite map.
//     }
//   while(*current == STRINGS_SEPERATION); // There can be more then one sprite

//   return ret;
// }


// std::string getString
// (std::string::const_iterator & current, std::string::const_iterator & peek,
//  const std::string::const_iterator max)
// {
//   std::string ret {};
//   ++current, ++peek;		// Skip inital STRING_DENOTATION character.
//   while(true)
//     {
//       if(peek == max)
// 	{
// 	  std::stringstream e {};
// 	  e<<"Error: parsing string, we have reached the end of the file but "
// 	    "have not encountred the appropriate character's. A rules.lev file "
// 	    "must end with the character's \""<<STRING_DENOTATION
// 	   <<FIELD_END_DENOTATION<<"\", if the last piece of information in the"
// 	    "last field in the file is a string.";
// 	  exit(e.str().c_str(), ERROR_MALFORMED_STRING);
// 	}
//       switch(*current)
// 	{
// 	case ESCAPE_CHAR:
// 	  if(*peek == ESCAPE_CHAR)
// 	    { // The ESCAPE_CHAR character has been escaped.
// 	      ret.push_back(ESCAPE_CHAR);
// 	      ++current, ++peek;
// 	    }
// 	  else
// 	    {   
// 	      if(*peek == STRING_DENOTATION)
// 		{ // The STRING_DENOTATION character has been escaped.
// 		  ret.push_back(STRING_DENOTATION);
// 		  ++current, ++peek;
// 		}
// 	      else
// 		{
// 		  std::stringstream e {};
// 		  e<<"Error: parsing string, encountered '"<<ESCAPE_CHAR<<"' but"
// 		    "did not encounter '"<<ESCAPE_CHAR<<"' or '"
// 		   <<STRING_DENOTATION<<"' following it.";
// 		  exit(e.str().c_str(), ERROR_MALFORMED_STRING);
// 		}
// 	    }
// 	  break;

// 	  // We've reached the end of the string :^).
// 	case STRING_DENOTATION:
// 	  // Current should be after the STRING_DENOTATION character.
// 	  ++current, ++peek;
// 	  return ret;
	
// 	default:
// 	  ret.push_back(*current);
// 	}
//       ++current, ++peek;
//     }
// }


// bool rubOutSpace
// (std::string & buff, std::string::const_iterator & current,
//  std::string::const_iterator & peek, std::string::const_iterator & max)
// {
//   bool foundSp {false};
//   while(std::isspace(*peek))
//     {
//       ++peek;
//       foundSp = true;
//       if(peek >= max)
// 	return false;
//     }
//   if(foundSp)			// Only erase if we found space.
//     { // Why ++current? Erases the sequnece in the range [first, last).
//       current = buff.erase(++current, peek);
//       --current; /* Set current to element it was pointing to uppon entering
// 		    function. */
//       peek = current, ++peek;	// Reset peak.
//       max = buff.end();		// Reset max.
//     }
  
//   return true;
// }


// void printCurrent
// (std::string::const_iterator & current, const int SHOW_COUNT,
//  std::stringstream & e)
// {
//   for(int iter {SHOW_COUNT} ; *current != levelFileStrings::NULL_BYTE &&
// 	iter > 0; --iter, ++current)
//     {
//       e<<*current;
//     }
// }
