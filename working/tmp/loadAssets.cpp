#include <iostream>
#include <sstream>
#include <cstring>
#include "loadAssets.h"
#include "../collapse/collapse.h"


/* Increment's (advances) i by n, if i equals iEnd before being incremented n times we throw an exception. */
template <typename T_A, typename T_B> auto getAdvancedIter(T_A i, T_B iEnd, const size_t n) -> T_A
{
  for(int iter {}; iter < n; ++iter)
    {
      i++;
      if(*i == *iEnd)
	throw std::out_of_range ("range error");
    }
  return i;
}


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


void parse(const std::string & buff, const char rulesFileName [], rules & levelRules)
{
  constexpr char headerStart [] = "(p("; // The header shuld always start with me.
  if(strncmp(buff.c_str(), headerStart, sizeof(headerStart) -1) != 0)
    {
      std::stringstream e {};
      e<<"Error header of the .rules.lev file \""<<rulesFileName<<"\" is malformed!";
      exit(e.str(), ERROR_RULES_LEV_HEADER);
    }


  try
    {
      for(std::string::const_iterator peek {getAdvancedIter(buff.begin(), buff.end(), sizeof(headerStart))},
	    current {peek++}; *peek != '\0'; ++peek, ++current)
	{	  
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

void switchOnChars(const std::string::const_iterator & current, const std::string::const_iterator & peek,
		   bool inHeader, std::string & string)
{
  switch(*current)
    {
    case '(':		// Handle start of new field or section.
      break;
    case '\"':		// Handle string.
      string = getString(current, peek);
      break;
    case '\\':			// Start of escape (if in string.)
      break;
    default:
      if(*peek == '#')// We've reached the end of the header (or file is malformed.) We've already checked for != '\\'
	{}	
    }
}


std::string getString(const std::string::const_iterator & current, const std::string::const_iterator & peek)
{
  return std::string {};	// just to compile.
}


bool checkForDoubleEscape(const std::string::const_iterator current, const std::string::const_iterator peek)
{
  if(*current == ESCAPE_CHAR)
    if(checkPeekForESCAPE_CHAR(peek))
      return true;
  return false;
}


bool checkPeekForESCAPE_CHAR(const std::string::const_iterator peek)
{
  if(*peek == ESCAPE_CHAR)
    return true;
  return false;
}
