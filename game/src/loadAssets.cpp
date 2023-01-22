// #include <iostream>
// #include <sstream>
// #include <cstring>
// #include <ncurses.h>
// #include <string>
// #include "include/utils.hpp"
// #include "include/loadAssets.hpp"
//#include "include/backgroundSprite.hpp"


// void loadAssets
// (const yx maxyx, const char bgFileName [],
//  backgroundData & background,
//  const char rulesFileName [], rules & levelRules)
// {
//   // Initialise background array.
//   //  loadAndParseBackgroundFile(maxyx, bgFileName, background);
//   /* Initialise player and non-player sprites (rules file) and initialise
//      levelRules array. */
//   /*  loadAndParseLevelRulesFile(maxyx, bgFileName, rulesFileName, levelRules,
//       background.size());*/
// }


/*void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				backgroundData & background)
				// std::vector<int> & background)
{
  std::string levelBackground {};
  if(!loadFileIntoString(bgFileName, levelBackground))
    {
      std::stringstream err {};
      err<<"Error: unable to open \""<<bgFileName<<"\".";
      exit(err.str(), ERROR_OPENING_FILE);
    }
  
  background.initialiseBackgroundData(false, bgFileName, levelBackground);
  // collapse(levelBackground, background); //collapse nonColor escape
  //sequences.
  // if(background.size() < (size_t(maxyx.x) * maxyx.y) ||
  //    (background.size() % maxyx.y) != 0)
  //   {
  //     std::stringstream err {};
  //     err<<"Error: wrong number of characters in background after collapsing "
  // 	"escaped characters. "
  // 	"background size (after processing) < (window length * window height) "
  // 	"or (background size (after processing) % window height) != 0."
  // 	" Where background size = "<<background.size()<<", window length = "
  // 	 <<maxyx.x<<" and window height = "<<maxyx.y<<".";
  //     exit(err.str(), ERROR_BACKGROUND);
  //   }
  }*/


// void loadAndParseLevelRulesFile(const yx maxyx, const char bgFileName [],
// 				const char rulesFileName[],
//                                 rules &levelRules, const size_t bgSize)
// {
//   std::string rawRules {};
//   if(!loadFileIntoString(rulesFileName, rawRules))
//     {
//       std::stringstream e {};
//       e<<"Error: unable to open \""<<rulesFileName<<"\".";
//       exit(e.str(), ERROR_OPENING_FILE);
//     }

//   std::string::const_iterator buffPos {rawRules.begin()};
//   parseRulesHeader(maxyx, rulesFileName, levelRules, bgSize, rawRules, buffPos);
//   parseRulesMain(maxyx, bgFileName, rulesFileName, levelRules, bgSize, rawRules, buffPos);
// }


// void initPlayer(const yx maxyx, const char rulesFileName[], rules & levelRules,
// 		const size_t bgSize, const std::string & rawRules,
// 		std::string::const_iterator & buffPos)
// {
//   using namespace levelFileKeywords;

//   /* Setup keyword actions associations for player section. This should contain
//      an entry for each thing that the player constructor takes. If there is a
//      variable that the player constructor needs but we don't want to be
//      specifiable in RULES_CONFIG_FILE_EXTENSION files then it's entry here should have a default
//      value and the action function should be set nullptr. */
//   std::vector<keywordAction::headerKeywordAction> playerHeaderKeywordActions
//     {keywordAction::headerKeywordAction
//      {SPRITE_FILE_SECTION_HEADER, readStringsSection},
//      keywordAction::headerKeywordAction
//      {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInNNumbers},
//      keywordAction::headerKeywordAction
//      {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_HEALTH_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_GRAV_CONST_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_MAX_VERT_V_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_MAX_FALL_JMP_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_MAX_JMP_NUM_SECTION_HEADER, nullptr}};

//   std::vector<std::string> targets {};
//   std::string targetFound {};

//   readSectionOpeningBracket
//     (rawRules, buffPos,
//      concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
// 	    " from \"", rulesFileName, "\""),
//      concat("a ", PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
//   for(auto keywordAction: playerHeaderKeywordActions)
//     {
//       targets.push_back(keywordAction.keyword);
//     }

//   playerInitialData playerInitData {};

//   // Parse player sub sections.
//   for(int targetIter {}; targetIter < (int)playerHeaderKeywordActions.size();
//       targetIter++)
//     {
//       targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
      
//       if(targetFound == "")
// 	{
// 	  break;
// 	}      
//       /* Target found, now check which object it's associated with and perform
// 	 targets associated action. */
//       for(int foundIter {}; foundIter < (int)playerHeaderKeywordActions.size();
// 	  foundIter++)
// 	{
// 	  if(targetFound == playerHeaderKeywordActions[foundIter].keyword)
// 	    {
// 	      if(playerHeaderKeywordActions[foundIter].found)
// 		{
// 		  std::stringstream e {};
// 		  e<<"Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \""<<rulesFileName
// 		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
// 		    "reading header sub section player section, however this "
// 		    "keyword is only expected once in this section.\n";
// 		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 		}
// 	      switch(playerSectionKeywordToId.at(targetFound).order)
// 		{
// 		case 0:
// 		  playerHeaderKeywordActions[foundIter].found = true;
// 		  playerHeaderKeywordActions[foundIter].action
// 		    (rawRules, buffPos,
// 		     concat("reading sprite dir strings from ",
// 	 RULES_CONFIG_FILE_EXTENSION, " header "
// 			    "file \"", rulesFileName, "\""),
// 		     &playerInitData.spritePaths);
		 
// 		  break;
// 		case 1:
// 		  playerHeaderKeywordActions[foundIter].found = true;
// 		  playerHeaderKeywordActions[foundIter].action
// 		    (rawRules, buffPos,
// 		     concat("reading single coord section from ",
// 		RULES_CONFIG_FILE_EXTENSION, " header"
// 			    " file  \"", rulesFileName, "\""),
// 		     &playerInitData.coordinate);
// 		  break;
// 		case 2:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		case 3:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		case 4:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		case 5:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
//                 case 6:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		case 7:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		}

// 	      if(false)
// 		{
// 		ENCOUNTERED_FORBIDDEN_KEYWORD:
// 		  std::stringstream e {};
// 		  e<<"Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \""<<rulesFileName
// 		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
// 		    "reading header sub section player section, however this "
// 		    "keyword is forbidden.\n";
// 		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 		}
// 	    }
// 	}
//     }

//   readSectionEndingBracket
//     (rawRules, buffPos,
//      concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
// 	    " from \"", rulesFileName, "\""),
//      concat(PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
//   // Check that we've encountered all keywords that were searched for.
//   for(auto keywordAction: playerHeaderKeywordActions)
//     {
//       if(!keywordAction.found)
// 	{
// 	  // See if there is a default value for not found.
// 	  checkForDefaultPlayerValues
// 	    (playerHeaderKeywordActions, keywordAction, playerInitData,
// 	     buffPos, rulesFileName);
// 	}
//     }

//   levelRules.gamePlayer =
//     new player(playerInitData.spritePaths, maxyx, bgSize,
// 	       playerInitData.coordinate, playerInitData.direction,
// 	       playerInitData.health, playerInitData.gravitationalConstant,
// 	       playerInitData.maxVerticalVelocity,
// 	       playerInitData.maxFallingJumpNumber,
// 	       playerInitData.maxJumpNumber);
// }


// void initBgSprites(const yx maxyx, const char rulesFileName[],
// 		   rules & levelRules, const size_t bgSize,
// 		   const std::string & rawRules,
// 		   std::string::const_iterator & buffPos)
// {
//   using namespace levelFileKeywords;
//   /* Setup keyword actions associations for background sprite section. This
//      should contain an entry for each thing that the bgSprite constructor takes.
//      If there is a variable that the bgSprite constructor needs but we don't
//      want to be specifiable in RULES_CONFIG_FILE_EXTENSION files then it's entry here should have
//      a default value and the action function should be set nullptr. */
//   std::vector<keywordAction::headerKeywordAction> bgSpriteHeaderKeywordActions
//     {keywordAction::headerKeywordAction
//      {SPRITE_FILE_SECTION_HEADER, readStringsSection},
//      keywordAction::headerKeywordAction
//      {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInZNumbers},
//      keywordAction::headerKeywordAction
//      {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
//      keywordAction::headerKeywordAction
//      {SPRITE_DISPLAY_IN_FORGROUND, readBoolSection}};

//   std::vector<std::string> targets {};
//   std::string targetFound {};

//     readSectionOpeningBracket
//     (rawRules, buffPos,
//      concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
// 	    " from \"", rulesFileName, "\""),
//      concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));
  
//   for(auto keywordAction: bgSpriteHeaderKeywordActions)
//     {
//       targets.push_back(keywordAction.keyword);
//     }

//   bgSpriteInitialData bgSpriteInitData {};

//   // Parse background sprite sub sections.
//   for(int targetIter {}; targetIter < (int)bgSpriteHeaderKeywordActions.size();
//       targetIter++)
//     {
//       targetFound = skipSpaceUpTo(rawRules, buffPos, targets);

//       if(targetFound == "")
// 	{
// 	  break;
// 	}

//       /* Target found, now check which object it's associated with and perform
// 	 targets associated action. */
//       for(int foundIter {}; foundIter < (int)bgSpriteHeaderKeywordActions.size();
// 	  foundIter++)
// 	{
// 	  if(targetFound == bgSpriteHeaderKeywordActions[foundIter].keyword)
// 	    {
// 	      if(bgSpriteHeaderKeywordActions[foundIter].found)
// 		{
// 		  std::stringstream e {};
// 		  e<<"Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \""<<rulesFileName
// 		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
// 		    "reading header sub section background sprite section, "
// 		    "however this keyword is only expected once in this "
// 		    "section.\n";
// 		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 		}
// 	      switch(bgSpriteSectionKeywordToId.at(targetFound).order)
// 		{
// 		case 0:
// 		  bgSpriteHeaderKeywordActions[foundIter].found = true;
// 		  bgSpriteHeaderKeywordActions[foundIter].action
// 		    (rawRules, buffPos,
// 		     concat("reading background sprite dir strings from ",
// 			    RULES_CONFIG_FILE_EXTENSION, " header file \"", rulesFileName, "\""),
// 		     &bgSpriteInitData.spritePaths);
// 		  break;
// 		case 1:
// 		  bgSpriteHeaderKeywordActions[foundIter].found = true;
// 		  bgSpriteHeaderKeywordActions[foundIter].action
// 		    (rawRules, buffPos,
// 		     concat("reading single coord section from ",
// 	 RULES_CONFIG_FILE_EXTENSION, " header"
// 			    "file  \"", rulesFileName, "\""),
// 		     &bgSpriteInitData.coordinate);
// 		  break;
// 		case 2:
// 		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
// 		case 3:
// 		  bgSpriteHeaderKeywordActions[foundIter].found = true;
// 		  bgSpriteHeaderKeywordActions[foundIter].action
// 		    (rawRules, buffPos,
// 		     concat("reading boolean section from ", RULES_CONFIG_FILE_EXTENSION, " header file"
// 			    " \"", rulesFileName, "\""),
// 		     &bgSpriteInitData.displayInForground);
// 		  break;
// 		}

// 	      if(false)
// 		{
// 		ENCOUNTERED_FORBIDDEN_KEYWORD:
// 		  std::stringstream e {};
// 		  e<<"Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \""<<rulesFileName
// 		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
// 		    "reading header sub section background sprite section, "
// 		    "however this keyword is forbidden.\n";
// 		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
// 		}
// 	    }
// 	}
//     }

//     readSectionEndingBracket
//     (rawRules, buffPos,
//      concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
// 	    " from \"", rulesFileName, "\""),
//      concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));

//     // Check that we've encountered all keywords that were searched for.
//   for(auto keywordAction: bgSpriteHeaderKeywordActions)
//     {
//       if(!keywordAction.found)
// 	{
// 	  // See if there is a default value for not found.
// 	  checkForDefaultBgSpriteValues
// 	    (bgSpriteHeaderKeywordActions, keywordAction, bgSpriteInitData,
// 	     buffPos, rulesFileName);
// 	}
//     }

//   levelRules.bgSprites.push_back
//     (new bgSprite(bgSpriteInitData.spritePaths, maxyx, bgSize,
// 		  bgSpriteInitData.coordinate, bgSpriteInitData.direction,
// 		  bgSpriteInitData.displayInForground));
// }


// void readStartOfHeader(const std::string & buff,
// 		       std::string::const_iterator & buffPos,
// 		       const std::string & eMsg)
// {
//   readSectionOpeningBracket(buff, buffPos, eMsg, std::string{"the header"});
// }

