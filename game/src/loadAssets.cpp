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
		     (maxyx, rulesFileName, levelRules, rawRules, buffPos);
		   break;
		 case 1:
		   /* We don't set found here because this keyword should have
		      headerKeywordAction.foundMultipleOptional set to true. */
		   headerKeywordActions[foundIter].headerAction
		     (maxyx, rulesFileName, levelRules, rawRules, buffPos);
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
		const std::string & rawRules,
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
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSection},
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
    new player(playerInitData.spritePaths, maxyx, playerInitData.coordinate,
	       playerInitData.direction, playerInitData.health,
	       playerInitData.gravitationalConstant,
	       playerInitData.maxVerticalVelocity,
	       playerInitData.maxFallingJumpNumber,
	       playerInitData.maxJumpNumber);
}


void initBgSprites(const yx maxyx, const char rulesFileName[], rules & levelRules,
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
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSection},
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
    (new bgSprite(bgSpriteInitData.spritePaths, maxyx,
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


void readSingleCoordSection(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, void * coord)
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
	"denote start of single coordinate section when "<<eMsg<<". Encountered "
       <<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  ((yx*)coord)->y = readSingleNum(buff, buffPos, eMsg);
      
  targets = {std::string {COORD_SEPARATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<COORD_SEPARATION<<"\" before second coordinate "
	"component in single coordinate section when "<<eMsg<<". Encountered "
       <<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  ((yx*)coord)->x = readSingleNum(buff, buffPos, eMsg);

  targets = {std::string {RULES_HEADER_SECTION_END_DENOTATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<"\" to "
	"denote end of single coordinate section when "<<eMsg<<". Encountered "
       <<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


int readSingleNum(const std::string & buff,
			  std::string::const_iterator & buffPos,
		 const std::string & eMsg)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
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
	       std::string {"\""}))};
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
//   for(int iter {SHOW_COUNT} ; *current != levelFileKeywords::NULL_BYTE &&
// 	iter > 0; --iter, ++current)
//     {
//       e<<*current;
//     }
// }


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
