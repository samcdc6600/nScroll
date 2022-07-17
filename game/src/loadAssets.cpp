#include <iostream>
#include <sstream>
#include <cstring>
#include <ncurses.h>
#include <string>
#include "include/utils.hpp"
#include "include/loadAssets.hpp"
#include "include/collapse.hpp"
//#include "include/backgroundSprite.hpp"


void loadAssets
(const yx maxyx, const char bgFileName [], std::vector<int> & background,
 const char rulesFileName [], rules & levelRules)
{
  // Initialise background array.
  loadAndParseBackgroundFile(maxyx, bgFileName, background);
  /* Initialise player and non-player sprites (rules file) and initialise
     levelRules array. */
  loadAndParseLevelRulesFile(maxyx, bgFileName, rulesFileName, levelRules,
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
  if(background.size() < (maxyx.x * maxyx.y) || (background.size() % maxyx.y) != 0)
    {
      std::stringstream err {};
      err<<"Error: wrong number of characters in background after collapsing "
	"escaped characters. "
	"background size (after processing) < (window length * window height) "
	"or (background size (after processing) % window height) != 0."
	" Where background size = "<<background.size()<<", window length = "
	 <<maxyx.x<<" and window height = "<<maxyx.y<<".";
      exit(err.str(), ERROR_BACKGROUND);
    }
}


void loadAndParseLevelRulesFile(const yx maxyx, const char bgFileName [],
				const char rulesFileName[],
                                rules &levelRules, const size_t bgSize)
{
  std::string rawRules {};
  if(!loadFileIntoString(rulesFileName, rawRules))
    {
      std::stringstream e {};
      e<<"Error: unable to open \""<<rulesFileName<<"\".";
      exit(e.str(), ERROR_OPENING_FILE);
    }

  std::string::const_iterator buffPos {rawRules.begin()};
  parseRulesHeader(maxyx, rulesFileName, levelRules, bgSize, rawRules, buffPos);
  parseRulesMain(maxyx, bgFileName, rulesFileName, levelRules, bgSize, rawRules, buffPos);
}


void parseRulesHeader(const yx maxyx, const char rulesFileName[],
			  rules & levelRules, const size_t bgSize,
		      const std::string & rawRules,
		      std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;

  readStartOfHeader
    (rawRules, buffPos,
     concat(std::string("reading start of rules.lev header file \""),
	    rulesFileName, "\""));

  /* Setup keyword actions associations for header section. If there is a
     section that is needed but we don't want to be specifiable in rules.lev
     files then it's entry here should have a default value and the action
     function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> headerKeywordActions
    {keywordAction::headerKeywordAction
     {PLAYER_HEADER_SECTION_SPECIFIER, nullptr, initPlayer, false},
     keywordAction::headerKeywordAction
     {BG_SPRITE_HEADER_SECTION_SPECIFIER, nullptr, initBgSprites, true}};

  std::vector<std::string> targets {};
  std::string targetFound {};
  for(auto keywordAction: headerKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  // Parse player sub sections.
  while(true)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
      
      if(targetFound == "")
	{
	  break;
	}
      
      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)headerKeywordActions.size();
	  foundIter++)
	{
	   if(targetFound == headerKeywordActions[foundIter].keyword)
	     {
	       /* Note here that sections that should only appear once will be
		  marked as found if encountered, however sections that can
		  appear  multiple times will not be marked as found if
		  encountered. */
	       if(headerKeywordActions[foundIter].found)
		 {
		   std::stringstream e {};
		   e<<"Error: reading rules.lev header file \""<<rulesFileName
		    <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		     "reading header section, however this keyword is only "
		     "expected once in this section.\n";
		   exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		 }
	       switch(headerSectionKeywordToId.at(targetFound).order)
		 {
		 case 0:
		   headerKeywordActions[foundIter].found = true;
		   headerKeywordActions[foundIter].headerAction
		     (maxyx, rulesFileName, levelRules, bgSize, rawRules,
		      buffPos);
		   break;
		 case 1:
		   /* We don't set found here because this keyword should have
		      headerKeywordAction.foundMultipleOptional set to true. */
		   headerKeywordActions[foundIter].headerAction
		     (maxyx, rulesFileName, levelRules, bgSize, rawRules,
		      buffPos);
		   break;
		 }

	       /*
		 NOTE THAT WE LEAVE THIS CODE HERE IN CASE IT IS EVER DESIRED TO
		 HAVE A KEYWORD THAT IS FORBIDDEN BUT ASSOCIATED WITH A DEFAULT
		 VALUE IN THIS SECTION. 
		 if(false)
		 {
		 ENCOUNTERED_FORBIDDEN_KEYWORD:
		   std::stringstream e {};
		   e<<"Error: reading rules.lev header file \""<<rulesFileName
		    <<"\". Encountered keyword \""<<targetFound<<"\" when "
		     "reading header section, however this keyword is "
		     "forbidden.\n";
		   exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		   }*/
	     }
	}
    }

  // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: headerKeywordActions)
    {
      if(!(keywordAction.found || keywordAction.foundMultipleOptional))
	{
	  // See if there is a default value for not found.
          /*
	    NOTE THAT HERE SIMILAR TO THE ABOVE WE LEAVE THIS CODE IN CASE IT IS
	    EVER DESIRED TO HAVE A  KEYWORD THAT HAS A DEFAULT VALUE FOR THIS
	    SECTION.  
	    checkForDefaultHeaderValues
	    (headerKeywordActions, keywordAction, playerInitData,
	    buffPos, rulesFileName);*/
	  std::stringstream e {};
	  e<<"Error: expected section\\s \"";
	  for(auto keywordAction: headerKeywordActions)
	    {
	      
	      if(!(keywordAction.found || keywordAction.foundMultipleOptional) &&
		 keywordAction.headerAction != nullptr)
		{
		  e<<keywordAction.keyword<<", ";
		}
	    }
	  e<<"\" in header section. Encountered character \""<<*(--buffPos)
	   <<*(++buffPos)<<*(++buffPos)<<"\", when reading \""<<rulesFileName
	   <<"\" file\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
    }
  
  readEndOfHeader
    (rawRules, buffPos,
     concat(std::string("reading end of rules.lev header \""),
	    rulesFileName, "\""));
}


void initPlayer(const yx maxyx, const char rulesFileName[], rules & levelRules,
		const size_t bgSize, const std::string & rawRules,
		std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;

  /* Setup keyword actions associations for player section. This should contain
     an entry for each thing that the player constructor takes. If there is a
     variable that the player constructor needs but we don't want to be
     specifiable in rules.lev files then it's entry here should have a default
     value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> playerHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {SPRITE_FILE_SECTION_HEADER, readStringsSection},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInNNumbers},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_HEALTH_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_GRAV_CONST_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_VERT_V_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_FALL_JMP_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_JMP_NUM_SECTION_HEADER, nullptr}};

  std::vector<std::string> targets {};
  std::string targetFound {};

  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  targets.clear();
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<" to "
	"denote the  start of a player section when reading header sub section "
	"player. Encountered \""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  
  for(auto keywordAction: playerHeaderKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  playerInitialData playerInitData {};

  // Parse player sub sections.
  for(int targetIter {}; targetIter < (int)playerHeaderKeywordActions.size();
      targetIter++)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
      
      if(targetFound == "")
	{
	  break;
	}      
      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)playerHeaderKeywordActions.size();
	  foundIter++)
	{
	  if(targetFound == playerHeaderKeywordActions[foundIter].keyword)
	    {
	      if(playerHeaderKeywordActions[foundIter].found)
		{
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section player section, however this "
		    "keyword is only expected once in this section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(playerSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat(std::string("reading sprite dir strings from "
					"rules.lev header file \""),
			    rulesFileName, "\""), &playerInitData.spritePaths);
		 
		  break;
		case 1:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat(std::string("reading single coord section from "
					"rules.lev header file  \""),
			    rulesFileName, "\""),
		     &playerInitData.coordinate);
		  break;
		case 2:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 3:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 4:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 5:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
                case 6:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 7:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section player section, however this "
		    "keyword is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

  targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
  targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  targets.clear();
  if(targetFound != std::string {RULES_HEADER_SECTION_END_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<" to "
	"denote the end of a player section when reading header sub section "
	"player. Encountered \""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: playerHeaderKeywordActions)
    {
      if(!keywordAction.found)
	{
	  // See if there is a default value for not found.
	  checkForDefaultPlayerValues
	    (playerHeaderKeywordActions, keywordAction, playerInitData,
	     buffPos, rulesFileName);
	}
    }

  levelRules.gamePlayer =
    new player(playerInitData.spritePaths, maxyx, bgSize,
	       playerInitData.coordinate, playerInitData.direction,
	       playerInitData.health, playerInitData.gravitationalConstant,
	       playerInitData.maxVerticalVelocity,
	       playerInitData.maxFallingJumpNumber,
	       playerInitData.maxJumpNumber);
}


void initBgSprites(const yx maxyx, const char rulesFileName[],
		   rules & levelRules, const size_t bgSize,
		   const std::string & rawRules,
		   std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;
  /* Setup keyword actions associations for background sprite section. This
     should contain an entry for each thing that the bgSprite constructor takes.
     If there is a variable that the bgSprite constructor needs but we don't
     want to be specifiable in rules.lev files then it's entry here should have
     a default value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> bgSpriteHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {SPRITE_FILE_SECTION_HEADER, readStringsSection},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInZNumbers},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_DIR_SECTION_HEADER, nullptr}};

  std::vector<std::string> targets {};
  std::string targetFound {};

  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  targets.clear();
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<" to "
	"denote the start of a background sprite section when reading header "
	"sub section background sprite. Encountered \""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  for(auto keywordAction: bgSpriteHeaderKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  bgSpriteInitialData bgSpriteInitData {};

  // Parse background sprite sub sections.
  for(int targetIter {}; targetIter < (int)bgSpriteHeaderKeywordActions.size();
      targetIter++)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);

      if(targetFound == "")
	{
	  break;
	}
      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)bgSpriteHeaderKeywordActions.size();
	  foundIter++)
	{
	  if(targetFound == bgSpriteHeaderKeywordActions[foundIter].keyword)
	    {
	      if(bgSpriteHeaderKeywordActions[foundIter].found)
		{
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section background sprite section, "
		    "however this keyword is only expected once in this "
		    "section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(bgSpriteSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat(std::string("reading background sprite dir strings "
					"from rules.lev header file \""),
			    rulesFileName, "\""), &bgSpriteInitData.spritePaths);
		  break;
		case 1:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat(std::string("reading single coord section from "
					"rules.lev header file  \""),
			    rulesFileName, "\""),
		     &bgSpriteInitData.coordinate);
		  break;
		case 2:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section background sprite section, "
		    "however this keyword is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

  targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
  targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
  targets.clear();
  if(targetFound != std::string {RULES_HEADER_SECTION_END_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<" to "
	"denote the end of a background header section when reading header sub "
	"section background sprite. Encountered \""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

    // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: bgSpriteHeaderKeywordActions)
    {
      if(!keywordAction.found)
	{
	  // See if there is a default value for not found.
	  checkForDefaultBgSpriteValues
	    (bgSpriteHeaderKeywordActions, keywordAction, bgSpriteInitData,
	     buffPos, rulesFileName);
	}
    }

  levelRules.bgSprites.push_back
    (new bgSprite(bgSpriteInitData.spritePaths, maxyx, bgSize,
		  bgSpriteInitData.coordinate, bgSpriteInitData.direction));
}


void readStartOfHeader(const std::string & buff,
		       std::string::const_iterator & buffPos,
		       const std::string & eMsg)
{
  using namespace levelFileKeywords;
    
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<"\" to "
	"denote the start of the header when "<<eMsg
       <<". Encountered\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  // targets = {std::string {PLAYER_HEADER_SECTION_SPECIFIER}};
  // targetFound = skipSpaceUpTo(buff, buffPos, targets);
  // if(targetFound != std::string {PLAYER_HEADER_SECTION_SPECIFIER})
  //   {
  //     std::stringstream e {};
  //     e<<"Error: expected \""<<PLAYER_HEADER_SECTION_SPECIFIER
  //      <<RULES_HEADER_SECTION_START_DENOTATION<<"\" to denote the start of "
  // 	"the player section when "<<eMsg<<". Encountered\""<<*buffPos
  //      <<"\"\n";
  //     exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
  //   }
  
  // targets = {std::string {RULES_HEADER_SECTION_START_DENOTATION}};
  // targetFound = skipSpaceUpTo(buff, buffPos, targets);
  // if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
  //   {
  //     std::stringstream e {};
  //     e<<"Error: expected \""<<PLAYER_HEADER_SECTION_SPECIFIER
  //      <<RULES_HEADER_SECTION_START_DENOTATION<<"\" to denote the start"
  // 	"of the player section when "<<eMsg<<". Encountered\""
  //      <<*buffPos<<"\"\n";
  //     exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
  //   }
}


void readStringsSection(const std::string & buff,
			std::string::const_iterator & buffPos,
			const std::string & eMsg, void * strings)
{
  using namespace levelFileKeywords;
  
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  targets.clear();
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<" to "
	"denote start of string section when "<<eMsg
       <<". Encountered\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
      // We're in a new section.
      while(true)
	{
	  std::string newString {};
	  
	  targets.push_back(std::string {STRING_DENOTATION});
	  // Skip to start of string.
	  targetFound = skipSpaceUpTo(buff, buffPos, targets);
	  targets.clear();
	  if(targetFound == "")
	    {
	      std::stringstream e {};
	      e<<"Error: expected \""<<STRING_DENOTATION<<"\" to start a "
		"string when "<<eMsg<<". Encountered \""<<*buffPos<<"\".\n";
	      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	    }

	  std::string::const_iterator peekBuffPos {buffPos};
	  peekBuffPos++;

	  if(*buffPos != STRING_DENOTATION)
	    {
	      // We don't have an empty string
	      do
		{
		  if(peekBuffPos == buff.end())
		    {
		      std::stringstream e {};
		      e<<"Error: file ended unexpectedly when "<<eMsg<<".\n";
		      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		    }
		  else if(*buffPos == STRING_ESC)
		    {
		      if(*peekBuffPos != STRING_DENOTATION &&
			 *peekBuffPos != STRING_ESC)
			{
			  std::stringstream e {};
			  e<<"Error: encountered character (\""<<*peekBuffPos
			   <<"\") other then \""<<STRING_ESC<<"\" or \""
			   <<STRING_DENOTATION<<"\" after escape character \""
			   <<STRING_ESC<<"\" when "<<eMsg<<".\n";
			  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
			}
		      else
			{
			  // Skip adding char at buffPos!
			  newString.push_back(*peekBuffPos);
			  peekBuffPos++;
			  if(peekBuffPos == buff.end())
			    {
			      std::stringstream e {};
			      e<<"Error: file ended unexpectedly when "<<eMsg<<".\n";
			      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
			    }  
			}
		    }
		  else
		    {
		      if(*buffPos == STRING_DENOTATION)
			{
			  // We've finished reading the string.
			  break;
			}
		      // BuffPos isn't pointing at a STRING_ESC char.
		      newString.push_back(*buffPos);
		    }
		  
		  
		  buffPos = peekBuffPos;
		  peekBuffPos++;
		} while(true);
	    }
	  ((std::vector<std::string>*)strings)->push_back(newString);
	  newString.clear();


      buffPos++;

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
	    "the string section when "<<eMsg
	   <<". Encountered \""<<*buffPos<<"\".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
      else if(targetFound == std::string {RULES_HEADER_SECTION_END_DENOTATION})
	{
	  // We've read all the strings in the string section.
	  break;
	}
    }
}


void readSingleCoordSectionInNNumbers(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers");
}


void readSingleCoordSectionInZNumbers(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, true, coord, "integers");
}


void readSingleCoordSection(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, const bool useIntegers,
			    void * coord, const std::string typeOfNumber)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets {};
  std::string targetFound {};
  
  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<"\" to "
	"denote start of single coordinate section (with "<<typeOfNumber
       <<") when "<<eMsg<<". Encountered "<<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  ((yx*)coord)->y = readSingleNum(buff, buffPos, eMsg, useIntegers);
      
  targets = {std::string {COORD_SEPARATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<COORD_SEPARATION<<"\" before second coordinate "
	"component in single coordinate section (with "<<typeOfNumber
       <<") when "<<eMsg<<". Encountered "<<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  ((yx*)coord)->x = readSingleNum(buff, buffPos, eMsg, useIntegers);

  targets = {std::string {RULES_HEADER_SECTION_END_DENOTATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<"\" to "
	"denote end of single coordinate section (with "<<typeOfNumber
       <<") when "<<eMsg<<". Encountered "<<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


int readSingleNum(const std::string & buff,
			  std::string::const_iterator & buffPos,
		  const std::string & eMsg, const bool useIntegers)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  if(useIntegers)
    {
      targets.push_back("-");
    }
  std::string targetFound {};
  std::stringstream number {};

  // Skip space up until the start of the number.
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected non-negative number when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
    }

  int decPlacesIter {};
  // Read in number.
  while(isNum(*buffPos))
    {
      number<<*buffPos;
      decPlacesIter++;
      buffPos++;
    }
  
  // Check if number was too long.
  if(decPlacesIter > MAX_COORD_LEN)
    {
      std::stringstream e {};
      e<<"Error: number \""<<number.str()<<"\" too long (longer than \""
       <<MAX_COORD_LEN<<"\") when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  int numberRet;
  number>>numberRet;
  return numberRet;
}


void readEndOfHeader(const std::string & buff,
		     std::string::const_iterator & buffPos,
		     const std::string & eMsg)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets {};
  std::string targetFound {};

  constexpr int nestingLevel {1};

  for(int iter {}; iter < nestingLevel; ++iter)
    {
      targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \"";
	  for(int iter {}; iter < nestingLevel; ++iter)
	    {
	      e<<RULES_HEADER_SECTION_END_DENOTATION;
	    }
	  e<<"\" when "
	   <<eMsg<<". Encountered \""<<*(--buffPos)<<*(++buffPos)
	   <<*(++buffPos)<<"\".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
    }

  targets = {std::string {RULES_HEADER_END_DENOTATION[1]}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(*(buffPos -= 2) != RULES_HEADER_END_DENOTATION[0] || targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_END_DENOTATION<<"\" when "
       <<eMsg<<". Encountered \""<<*buffPos<<*(++buffPos)<<*(++buffPos)<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  buffPos += 2;
}


void parseRulesMain(const yx maxyx, const char bgFileName [],
		    const char rulesFileName[], rules & levelRules,
		    const size_t bgSize,
		    const std::string & rawRules,
		    std::string::const_iterator & buffPos)
{
  // Main body of rules file should start on a new line (account for if.)
  buffPos++;			
  const size_t expectedLineLength {bgSize / maxyx.y};
  int lineNumber {};
  int lineLength {};
  
  while(buffPos < rawRules.end())
    {
      if(*buffPos == levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR)
	{
	  // We've encountered a run-length encoding character.
	  const char ruleChar {*(++buffPos)};
	  ++buffPos;

	  checkRuleChar(ruleChar,
			concat(std::string {"parsing main section of \""},
			       rulesFileName, std::string {"\""},
			       std::string {" (on line "}, lineNumber,
			       std::string {" of main section)"}));

	  const int runLength {readSingleNum
	    (rawRules, buffPos,
	     concat(std::string {"reading run-length encoding length as a "
				 "result of encountering run-length encoding "
				 "character \""},
	       levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR,
	       std::string {"\" while parsing main section of \""},
	       rulesFileName,
	       std::string {"\""}), false)};
	  --buffPos;

	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
	    {
	      levelRules.coordRules.push_back(ruleChar);
	    }
	  lineLength += runLength;
	}
      else
	{
	  checkRuleChar(*buffPos,
			concat(std::string {"parsing main section of \""},
			       rulesFileName, std::string {"\""},
			       std::string {" (on line "}, lineNumber,
			       std::string {" of main section)"}));
	  levelRules.coordRules.push_back(*buffPos);
	  lineLength++;
	}
      
      buffPos++;
      if(*buffPos == '\n')
	{
	  if(lineLength != expectedLineLength)
	    {
	      std::stringstream e {};
	      e<<"Error: reading rules.lev header file \""<<rulesFileName
	       <<"\". Encountered line of length ("<<lineLength<<") (on line "
	       <<lineNumber<<" of main section) when reading main section of "
		"file. Expected a line of length ("<<expectedLineLength<<").";

	      exit(e.str().c_str(), ERROR_BACKGROUND);
	    }
	  lineLength = 0;
	  lineNumber++;
	  buffPos++;
	}
    }

  if(lineLength != expectedLineLength)
    {
      std::stringstream e {};
      e<<"Error: reading rules.lev header file \""<<rulesFileName
       <<"\". Encountered line of length ("<<lineLength<<") (on line "
       <<lineNumber<<") when reading main section of file. Expected a line "
	"of length ("<<expectedLineLength<<").";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }

  if(levelRules.coordRules.size() != bgSize)
    {
      std::stringstream e {};
      e << "Error: reading rules.lev header file \"" << rulesFileName
	<< "\". Size ("<<levelRules.coordRules.size()<<") of main section of "
	"file not equal to size ("<<bgSize<<") of background file \""
	<<bgFileName<<"\".";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }
}


void checkRuleChar(const char potentialRule, const std::string eMsg)
{
  for(char rule: boarderRuleChars::CHARS)
    {
      if(potentialRule == rule || potentialRule == ' ')
	{
	  return;
	}
    }
  std::stringstream e {};
  e<<"Error: found that character \""<<potentialRule<<"\" is not a space, new "
    "line, \""<<levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR<<"\" or in "
    "the set of rule characters (";
  for(char rule: boarderRuleChars::CHARS)
    {
      e<<rule<<", ";
    }
  e<<") when "<<eMsg<<'.';
  exit(e.str().c_str(), ERROR_CHARACTER_RANGE);
}


void checkForDefaultPlayerValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 playerHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::playerInitialData & playerInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName [])
{
  using namespace levelFileKeywords;

  if(!playerSectionKeywordToId.at(keywordAction.keyword).defaultVal)
    {
      goto DEFAULT;
    }
  
  switch(playerSectionKeywordToId.at(keywordAction.keyword).order)
    {
    case 0:
      playerInitData.spritePaths = defaultValues::player::spritePaths;
      break;
    case 1:
      playerInitData.coordinate = defaultValues::player::coordinate;
      break;
    case 2:
      playerInitData.direction = defaultValues::player::direction;
      break;
    case 3:
      playerInitData.health = defaultValues::player::health;
      break;
    case 4:
      playerInitData.gravitationalConstant =
	defaultValues::player::gravitationalConstant;
      break;
    case 5:
      playerInitData.maxVerticalVelocity =
	defaultValues::player::maxVerticalVelocity;
      break;
    case 6:
      playerInitData.maxFallingJumpNumber =
	defaultValues::player::maxFallingJumpNumber;
      break;
    case 7:
      playerInitData.maxJumpNumber = defaultValues::player::maxJumpNumber;
      break;
    default:
    DEFAULT:
      std::stringstream e {};
      e<<"Error: expected section\\s \"";
      for(auto keywordAction: playerHeaderKeywordActions)
	{
	  if(!keywordAction.found && keywordAction.action != nullptr)
	    {
	      e<<keywordAction.keyword<<", ";
	    }
	}
      e<<"\" in header sub section player. Encountered character \""
       <<*buffPos<<"\", when reading \""<<rulesFileName<<"\" file\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


void checkForDefaultBgSpriteValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 bgSpriteHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
 levelFileKeywords::bgSpriteInitialData &bgSpriteInitData,
 std::string::const_iterator &buffPos, const char rulesFileName[])
{
  using namespace levelFileKeywords;


  if(!bgSpriteSectionKeywordToId.at(keywordAction.keyword).defaultVal)
    {
      goto DEFAULT;
    }

  switch(bgSpriteSectionKeywordToId.at(keywordAction.keyword).order)
    {
    case 0:
      bgSpriteInitData.spritePaths = defaultValues::bgSprites::spritePaths;
      break;
    case 1:
      bgSpriteInitData.coordinate = defaultValues::bgSprites::coordinate;
      break;
    case 2:
      bgSpriteInitData.direction = defaultValues::bgSprites::direction;
      break;
    default:
    DEFAULT:
      std::stringstream e {};
      e<<"Error: expected section\\s \"";
      for(auto keywordAction: bgSpriteHeaderKeywordActions)
	{
	  if(!keywordAction.found && keywordAction.action != nullptr)
	    {
	      e<<keywordAction.keyword<<", ";
	    }
	}
      e<<"\" in header sub section backgroundSprite. Encountered character \""<<*buffPos
       <<"\", when reading \""<<rulesFileName<<"\" file\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}
