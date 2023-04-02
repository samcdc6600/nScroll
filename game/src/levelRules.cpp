#include "include/levelRules.hpp"
#include "include/player.hpp"
#include <ctime>
#include <string>


namespace levelFileKeywords
{
  namespace keywordAction
  {
    struct headerKeywordAction
    {
      headerKeywordAction
      (const std::string & kword,
       void (* a)
       (const std::string & buff, std::string::const_iterator & buffPos,
	const std::string & eMsg, void * retObj),
       void (*hA)
       (const yx viewPortSize, const char rulesFileName[],
	rules & levelRules, const backgroundData & background,
	const std::string & rawRules,
	std::string::const_iterator & buffPos) = nullptr,
       const bool fMO = false)
	: keyword(kword), foundMultipleOptional(fMO), action(a),
	  headerAction(hA)
      { }

      const std::string keyword;
      // Found this keyword already (for keywords that can only appear once.).
      bool found{false};
      /* Set to true in the constructor if keyword is optional and can appear
	 multiple times. */
      const bool foundMultipleOptional;
      /* If action needs to return anything the address of a variable of the
	 appropriate type should be passed to the function via retObj. If the
	 function doesn't encounter any error (in which case it should print a
	 message and exit) *retObj should be populated with what is essential the
	 return value of the function. */
      void (* action)(const std::string & buff,
		     std::string::const_iterator & buffPos,
		     const std::string & eMsg, void * retObj);
      /* This function should be used as the action instead of the above if we
	 are at the top level of the header (in terms of sections). */
      void (* headerAction)(const yx viewPortSize, const char rulesFileName[],
			   rules &levelRules, const backgroundData & background,
			   const std::string & rawRules,
			   std::string::const_iterator & buffPos);
    };
  };
  // Each new section should start with a header char followed by this char.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  // constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  // Header section header. ====================================================
  const std::string PLAYER_HEADER_SECTION_SPECIFIER {"player"};
  const std::string BG_SPRITE_HEADER_SECTION_SPECIFIER {"backgroundSprite"};
  const std::string SPRITE_FILE_SECTION_HEADER		{"sprites"};
  // Header sub section headers. ===============================================
  // Sub section headers for general sprite stuff and player. ==================
  const std::string SPRITE_INIT_COORD_SECTION_HEADER	{"initialCoordinate"};
  // Sub section headers for player sprite. ====================================
  /* Initial position of the view port (relative to the top left of the
     player.) */
  const std::string PLAYER_VIEW_PORT_INIT_COORD_PLAYER_REL_SECTION_HEADER
    {"initialViewPortCoordinatePlayerRelative"};
  const std::string PLAYER_INIT_COORD_SECTION_HEADER
    {"initialCoordinate"};
  /* The player sprite cannot be within this many characters of any edge of the
     view port. */
  const std::string VIEW_PORT_PADDING			 {"viewPortPadding"};
  const std::string SPRITE_INIT_DIR_SECTION_HEADER	{"initialDirection"};
  const std::string SPRITE_HEALTH_SECTION_HEADER	{"initialHealth"};
  const std::string SPRITE_GRAV_CONST_SECTION_HEADER	{"gravConst"};
  const std::string SPRITE_MAX_VERT_V_SECTION_HEADER	{"maxVelocity"};
  const std::string SPRITE_MAX_FALL_JMP_SECTION_HEADER	{"maxJumpsAfterFall"};
  const std::string SPRITE_MAX_JMP_NUM_SECTION_HEADER	{"maxJumps"};
  // Sub section headers for background sprite.
  const std::string SPRITE_DISPLAY_IN_FORGROUND         {"displayInForground"};
  // Used to store the order of a keyword and whether it has a default value.
  struct orderAndDefaultVal
  {
    int order;
    bool defaultVal;
  };
  /* Used to map keywords to unique int values so appropriate action can be
     taken for those keywords when parsing specific section section. KEY ORDER
     (first value of orderAndDefaultVal) OF THESE OBJECTS SHOULD MATCH switches
     that use these objects.! */
  const std::map<std::string, orderAndDefaultVal> headerSectionKeywordToId
    {
      {PLAYER_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {0, false}},
      {BG_SPRITE_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {1, false}}
    };
  const std::map<std::string, orderAndDefaultVal> playerSectionKeywordToId
    {
      {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
      {PLAYER_VIEW_PORT_INIT_COORD_PLAYER_REL_SECTION_HEADER, orderAndDefaultVal
       {1, false}},
      {PLAYER_INIT_COORD_SECTION_HEADER,
       orderAndDefaultVal {2, false}},
      {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {3, true}},
      {SPRITE_HEALTH_SECTION_HEADER,	orderAndDefaultVal {4, true}},
      {SPRITE_GRAV_CONST_SECTION_HEADER,	orderAndDefaultVal {5, true}},
      {SPRITE_MAX_VERT_V_SECTION_HEADER,	orderAndDefaultVal {6, true}},
      {SPRITE_MAX_FALL_JMP_SECTION_HEADER, orderAndDefaultVal {7, true}},
      {SPRITE_MAX_JMP_NUM_SECTION_HEADER,	orderAndDefaultVal {8, true}},
      {VIEW_PORT_PADDING, orderAndDefaultVal {9, true}}
    };
  const std::map<std::string, orderAndDefaultVal> bgSpriteSectionKeywordToId
    {
      {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
      {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
      {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
      {SPRITE_DISPLAY_IN_FORGROUND,       orderAndDefaultVal {3, true}}
    };
  namespace defaultValues
  {
    /* These default values can only be used if defaultVal is set to true in
       orderAndDefaultVal for the values corresponding section in it's
       associated XXXSectionKeywordToId map (see above). */
    namespace player
    {
      const std::vector<std::string> spritePaths {{""}};
      //      const yx initialViewPortCoordinates {0, 0};
      //      const yx initialCoordinatesRelative {yHeight / 2, xWidth / 2};
      const yx initialCoordinates {yHeight / 2, xWidth / 2};
      const sprite::directions direction {sprite::DIR_NONE};
      const int health {16};
      const double gravitationalConstant {-0.38};
      const double maxVerticalVelocity {1.9};
      const unsigned maxFallingJumpNumber {1};
      const unsigned maxJumpNumber {3};
    }
    namespace bgSprites
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx coordinate {20, 62};
      const sprite::directions direction {sprite::DIR_NONE};
      const bool displayInForground {false};
    }
    namespace misc
    {
      const yx initialRelViewPortCoordinates {0, 0};
      const yx viewPortPadding {0, 0};
    }
  }
  /* This struct should be populated with the values that the player will
     eventually be initialised with. */
  struct playerInitialData
  {
    std::vector<std::string> spritePaths {};
    //    yx initialViewPortCoordinates {};
    yx initialCoordinates {};
    sprite::directions direction {};
    int health {};
    double gravitationalConstant {};
    double maxVerticalVelocity {};
    unsigned maxFallingJumpNumber {};
    unsigned maxJumpNumber {};
  };
  /* This struct should be populated with the values read in for use with the
     constructor of the next background sprite to be created. */
  struct bgSpriteInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
    sprite::directions direction {};
    bool displayInForground {};
  };
  /* Misc is used for variables read in that are used in the rules object
     itself. */
  struct
  {
    yx initialRelViewPortCoordinates;
    yx viewPortPadding;
  } misc;
  // Misc.
  constexpr char STRING_DENOTATION		{'\"'};
  constexpr char STRING_SEPARATION		{','};
  constexpr char STRING_ESC			{'\\'};
  constexpr char COORD_SEPARATION		{','};
  constexpr char DIR_START_ABS			{'/'};
  constexpr char DIR_START_REL			{'.'};
  /* The character used for escape sequences (within a string) in
     RULES_CONFIG_FILE_EXTENSION files. */
  constexpr char ESCAPE_CHAR {'\\'};
  constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
  constexpr char NULL_BYTE {'\0'};
  /* If this character is encountered in the main section of a
     RULES_CONFIG_FILE_EXTENSION file the character 2 places after it should be
     an ASCII number. After this number there can be a string of contiguous
     ASCII numbers (up to some maximum) that together represent some larger
     number. This number is the number of times the ASCII character before the
     number should be repeated. */
  constexpr char RULES_MAIN_RUNLENGTH_BEGIN_CHAR {'R'};
} // namespace levelFileKeywords


// ===== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ====
// =============================================================================
void initPlayer
(const yx viewPortSize, const char rulesFileName [], rules & levelRules,
 const backgroundData & background, const std::string & rawRules,
 std::string::const_iterator & buffPos);
/* This function should be called for each background sprite section that's
   encountered. */
void initBgSprites
(const yx viewPortSize, const char rulesFileName[], rules & levelRules,
 const backgroundData & background, const std::string & rawRules,
 std::string::const_iterator & buffPos);
void readSectionOpeningBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section,
 const bool skipSpace = true);
/* Attempts to read the bracket at the end of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionEndingBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section);
/* Attempts to read the strings from a string section in a
   RULES_CONFIG_FILE_EXTENSION file. Should be called when a series of strings
   is expected. Buff is the buffer where the strings should be located and
   buffPos is the position to start reading from. EMsg will be embedded in any
   error message/s the function spits out and should say something about the
   context in which readStringsSection() was called. Returns the strings that
   were read. */
void readStringsSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * retObj);
/* Attempts to read one coordinate from a section in buff starting at buffPos.
   Emsg will be embedded in any error message/s the function spits out and
   should say something about the context in which the function was
   called. Returns the coordinate read. Only reads natural numbers (inclusive
   of 0.)*/
void readSingleCoordSectionInNNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord);
/* This function is the same as readSingleCoordSectionInNNumbers() with the one
   exception that it does not skip any spaces i.e. the first character of the
   coordinate (the "(" terminal) should be at *buffPos. Otherwise the program will
   be halted with an error message. */
void readSingleCoordSectionInNNumbersNoSkpSp
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord);
/* Same as readSingleCoordSectionInNNumbers() with the exception that it can
   read integers. */
void readSingleCoordSectionInZNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord);
/* This function should be called through readSingleCoordSectionInNNumbers() or
   readSingleCoordSectionInZNumbers(). SkipSpace suppresses the skipping of
   spaces before the opening terminal "(". */
void readSingleCoordSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers,
 void * coord, const std::string typeOfNumber, const bool skipSpace = true);
/* Attempts to read a boolean for a section in buff starting at buffPos. Emsg
   will be embedded in any error message/s the function spits out and should
   START say something about the context in which the function was called. Returns
   the bool read. Can read a boolean in a string or integer format i.e. "true" /
   "false" or 1 / 0. */
void readBoolSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * boolean);

/* Should be called after checking for all header sections related to the
   player. Checks if all sections were found. If a section is encountered that
   wasn't found, then we check if it has a default value. If so we apply the
   default value. If it doesn't have a default value then call exit()*/
void checkForDefaultPlayerValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 playerHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::playerInitialData & playerInitData,
 std::string::const_iterator & buffPos, const char rulesFileName[]);
void checkForDefaultBgSpriteValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 bgSpriteHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName []);
// ====== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files END =====
// =============================================================================


void rules::loadAndInitialiseCoordRules(const yx expectedChunkSize,
					const char coordRulesFileName[],
					const backgroundData & background)
{
  std::string rawCoordRules {};
  
  loadFileIntoString(coordRulesFileName, rawCoordRules,
		     concat("trying to read ", COORD_RULES_FILE_EXTENSION,
			    " file"));
  initialiseCoordRules
    (expectedChunkSize, coordRulesFileName, chunkMap, rawCoordRules,
     background);
}


void rules::initialiseCoordRules
(const yx expectedChunkSize, const char coordRulesFileName [],
 chunkMapType & coordRules, const std::string & coordRulesData,
 const backgroundData & background) const
{
  yx chunkCoord {};
  std::string chunk {};
  chunkType rawChunk {};
  std::string::const_iterator buffPos {std::begin(coordRulesData)};
  ssize_t chunksReadIn {};

  // Extract chunks from coordRulesData ========================================
  while(true)
    {
      /* Each chunk should have a header that contains it's coordinates
	 in the level. The unit of the coordinates should be chunks. So
	 for a chunk that starts at (0, 170) in character coordinates
	 (assuming the size of chunks for this file are 170 in the x
	 dimension), the coordinate in the header would be (0, 1) and
	 for (0, 340) it would be (0, 2), etc... */
      if(getChunkCoordinate
	 (coordRulesData, buffPos,
	  concat("trying  to read coord no. ", chunksReadIn, " from ",
		 COORD_RULES_FILE_EXTENSION, " file \"", coordRulesFileName,
		 "\""), chunkCoord))
	{
	  skipSpaceUpToNextLine
	    (coordRulesData, buffPos,
	     concat("Error: trying to read chunk no. ", chunksReadIn,
		    ". Expected '\\n' after reading chunk coordinate (",
		    chunkCoord.y, ", ", chunkCoord.x, ") from ",
		    COORD_RULES_FILE_EXTENSION, " file \"", coordRulesFileName,
		    "\". Encountered other character or EOF."));
	  // Get chunk from coord rules read from file.
	  getChunk
	    (coordRulesData, buffPos,
	     concat("trying to read in chunk no. ", chunksReadIn, ". from ",
		    COORD_RULES_FILE_EXTENSION, " file \"", coordRulesFileName,
		    "\"."), chunk, expectedChunkSize);
	  // Decompress chunk, verify size and return in rawChunk.
	  decompressChunk
	    (chunk, rawChunk, expectedChunkSize, chunksReadIn,
	     coordRulesFileName);
	  insertChunk(chunkCoord, rawChunk, chunksReadIn, coordRulesFileName,
		      coordRules);
	  
	  chunksReadIn++;
	  // GetChunk() clears it's argument (chunk).
	  rawChunk.clear();
	}
      else
	{
	  break;
	}
    }

  /* Each chunk in background must have a matching chunk in coordRules. This
     will simplify logic in other areas somewhat. It does means that
     COORD_RULES_FILE_EXTENSION files will be larger than they would technically
     need to be, however this shouldn't be much of a problem since the files
     wouldn't be too large anyway (especially with compressed chunks.) Although
     some extra memory will be used the game logic will be slightly simpler. */
  verifyTotalOneToOneOntoMappingOfCoordToBgKeys(coordRules, background);
}


void rules::decompressChunk
(const std::string & chunkIn, chunkType & rawChunk,
 const yx expectedChunkSize, const ssize_t chunksReadIn,
 const char coordRulesFileName[]) const
{
  const char runLenBeginChar
    {levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR};
  std::string::const_iterator buffPos {std::begin(chunkIn)};
  int lineNumber {};
  int lineLength {};
    
  while(buffPos < chunkIn.end())
    {
      if(*buffPos == runLenBeginChar)
	{
	  // We've encountered a run-length encoding character.
	  const char ruleChar {*(++buffPos)};
	  ++buffPos;

	  checkRuleChar
	    (ruleChar,
	     concat("decompressing chunk no. ", chunksReadIn, " from file \"",
		    coordRulesFileName, "\" (on line ", lineNumber + 1,
		    " of chunk.) "));

	  const int runLength
	    {readSingleNum
	     (chunkIn, buffPos,
	      concat("reading run-length encoding length as a result of "
		     "encountering run-length encoding character \"",
		     runLenBeginChar, "\" while  decompressing chunk no. ",
		     chunksReadIn, " from file \"",
		     coordRulesFileName, "\" (on line ", lineNumber + 1,
		     " of chunk.) "), false)};
	  --buffPos;

	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
	    {
	      rawChunk.push_back(ruleChar);
	    }
	  lineLength += runLength;
	}
      else
	{
	  checkRuleChar
	    (*buffPos,
	     concat("decompressing chunk no. ", chunksReadIn, " from file \"",
		    coordRulesFileName, "\" (on line ", lineNumber + 1,
		    " of chunk.) "));
	  rawChunk.push_back(*buffPos);
	  lineLength++;
	}
      
      buffPos++;
      if(*buffPos == '\n')
	{
	  if(lineLength != expectedChunkSize.x)
	    {
	      exit(concat
		   ("Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header ",
		    "file \"", coordRulesFileName, "\". Encountered line of ",
		    "length (", lineLength, ") when ", "decompressing chunk ",
		    "no. ", chunksReadIn, " (on line ", lineNumber + 1, " of ",
		    "chunk.) Expected a line of length (", expectedChunkSize.x,
		    ")."), ERROR_BACKGROUND);
	    }
	  lineLength = 0;
	  lineNumber++;
	  buffPos++;
	}
    }

  if(lineLength != expectedChunkSize.x)
    {
      exit(concat("\n", *buffPos, *(++buffPos), *(++buffPos), "\n"), 1);
      exit(concat
	   ("Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \"",
	    coordRulesFileName, "\". Encountered line of length (", lineLength,
	    ") when decompressing chunk no. ", chunksReadIn, " (on line ",
	    lineNumber + 1, " of chunk.) Expected a line of length (",
	    expectedChunkSize.x, ")."), ERROR_BACKGROUND);
    }

  const ssize_t expectedChunkSizeLinear
    {expectedChunkSize.y * expectedChunkSize.x};
  if(rawChunk.size() != expectedChunkSizeLinear)
    {
      exit(concat
	   ("Error: reading ", RULES_CONFIG_FILE_EXTENSION, " header file \"",
	    coordRulesFileName, "\".  Size (", rawChunk.size(), ") of chunk ",
	    "no. ", chunksReadIn, " not equal to expected size (",
	    expectedChunkSizeLinear, ") of ", "chunk."), ERROR_BACKGROUND);
    }
}


void rules::checkRuleChar
    (const char potentialRule, const std::string eMsg) const
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


void rules::verifyTotalOneToOneOntoMappingOfCoordToBgKeys
(const chunkMapType & coordRules, const backgroundData & background) const
{
  if(coordRules.size() != background.numberOfChunks())
    {
      exit
	(concat("Error: number (", background.numberOfChunks(), ") of chunks "
		"read in from ", BACKGROUND_FILE_EXTENSION, " file (",
		background.fileName, ") not equal to number (",
		coordRules.size(), ") of chunks read in from ",
		COORD_RULES_FILE_EXTENSION, " file (",
		fileName, ")."), ERROR_GENERIC_RANGE_ERROR);
    }

  for(const auto & chunkTypeTwoTuple : coordRules)
    {
      if(!background.keyExists(chunkTypeTwoTuple.first))
	{
	  exit
	    (concat("Error: found chunk (", chunkTypeTwoTuple.first,
		    ") in ", COORD_RULES_FILE_EXTENSION, " file (",
		    fileName, ") with no counterpart in ",
		    BACKGROUND_FILE_EXTENSION, " file (",
		    background.fileName, ")."),
	     ERROR_RULES_CHAR_FILE);
	}
    }
}



void rules::parseRulesConfigFileAndInitialiseVariables
(const char rulesFileName [], const std::string & rulesBuffer,
 const backgroundData & background)
{
  using namespace levelFileKeywords;

  std::string::const_iterator buffPos {std::begin(rulesBuffer)};

  readSectionOpeningBracket
    (rulesBuffer, buffPos,
     concat("reading start of ",
	    RULES_CONFIG_FILE_EXTENSION,
	    " header file \"", rulesFileName, "\""),
     concat("the header"));

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
      targetFound = skipSpaceUpTo(rulesBuffer, buffPos, targets);
      
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
		   e<<"Error: reading "<<RULES_CONFIG_FILE_EXTENSION
		    <<" header file \""<<rulesFileName
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
		     (chunkSize, rulesFileName, *this, background,
		      rulesBuffer, buffPos);
		   break;
		 case 1:
		   /* We don't set found here because this keyword should have
		      headerKeywordAction.foundMultipleOptional set to true. */
		   headerKeywordActions[foundIter].headerAction
		     (chunkSize, rulesFileName, *this, background,
		      rulesBuffer, buffPos);
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

  
  // Init class member variables with values from misc struct.
  PLAYER_MOVEMENT_AREA_PADDING = misc.viewPortPadding;
  INITIAL_REL_VIEW_PORT_COORDINATES = misc.initialRelViewPortCoordinates;
}


void rules::checkInitViewPortPosAndPadding()
{
  /* We must check the view port position and padding as the absolute view port
     position is calculated using the player position and the relative view
     port position and as such if the magnitude of the relative view port
     position is too large the player will not be visible or will be in the
     padding area. This of course is an error. */

  const yx maxBR {gamePlayer->getMaxBottomRightOffset()};

  if(INITIAL_REL_VIEW_PORT_COORDINATES.y > 0 ||
     INITIAL_REL_VIEW_PORT_COORDINATES.x > 0)
    {
      // Initial VPP is too large.
      exit(concat("Error: initialViewPortCoordinatePlayerRelative ",
		  INITIAL_REL_VIEW_PORT_COORDINATES, " must not be more than"
		  " (0, 0)."), ERROR_VIEWPORT_POS_RANGE);
    }
  else if((abs(INITIAL_REL_VIEW_PORT_COORDINATES) + yx{1}).y + maxBR.y / 2
	  > (chunkSize.y / 2) ||
	  (abs(INITIAL_REL_VIEW_PORT_COORDINATES) + yx{1}).x + maxBR.x / 2
	  > (chunkSize.x / 2))
    {
      // Abs(initial VPP) is too large.
      exit(concat("Error: initialViewPortCoordinatePlayerRelative + "
		  "(player height and width) / 2 ",
		  abs(INITIAL_REL_VIEW_PORT_COORDINATES) + yx{1} +
		  yx{maxBR.y / 2, maxBR.x / 2}, " is more than chunk size / 2 ",
		  yx{chunkSize.y / 2, chunkSize.x / 2}, " and shouldn't be."),
	   ERROR_VIEWPORT_POS_RANGE);
    }
  else if((PLAYER_MOVEMENT_AREA_PADDING  + yx{1}).y + maxBR.y / 2 >
	  (chunkSize.y / 2) ||
	  (PLAYER_MOVEMENT_AREA_PADDING + yx{1}).x + maxBR.x / 2 >
	  (chunkSize.x / 2))
    {
      // Padding is too large.
      exit(concat("Error: viewPortPadding + (player height and width) / 2 ",
		  PLAYER_MOVEMENT_AREA_PADDING + yx{1} +
		  yx{maxBR.y / 2, maxBR.x / 2}, " is more than chunk size / 2 ",
		  yx{chunkSize.y / 2, chunkSize.x / 2}, " and shouldn't be."),
	   ERROR_VIEWPORT_POS_RANGE);
    }
  else if(PLAYER_MOVEMENT_AREA_PADDING.y >
	  abs(INITIAL_REL_VIEW_PORT_COORDINATES.y) ||
	  PLAYER_MOVEMENT_AREA_PADDING.x >
	  abs(INITIAL_REL_VIEW_PORT_COORDINATES.x))
    {
      // Padding is larger than abs(initial VPP).
      exit(concat("Error: viewPortPadding ", PLAYER_MOVEMENT_AREA_PADDING,
                  " is greater than "
                  "abs(initialViewPortCoordinatePlayerRelative) ",
		  INITIAL_REL_VIEW_PORT_COORDINATES, " and shouldn't be."),
	   ERROR_VIEWPORT_POS_RANGE);
    }
}


void initPlayer
(const yx viewPortSize, const char rulesFileName[], rules & levelRules,
 const backgroundData & background, const std::string & rawRules,
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
     {PLAYER_VIEW_PORT_INIT_COORD_PLAYER_REL_SECTION_HEADER,
      /* View port coordinate is relative to the top left of the player and must
	 be 0 or less (depending on the value of the padding.) */
      readSingleCoordSectionInZNumbers},
     keywordAction::headerKeywordAction
     {PLAYER_INIT_COORD_SECTION_HEADER,
      readSingleCoordSectionInZNumbers},
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
     {SPRITE_MAX_JMP_NUM_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {VIEW_PORT_PADDING, readSingleCoordSectionInNNumbers}};

  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
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
		  e<<"Error: reading "<<RULES_CONFIG_FILE_EXTENSION
		   <<" header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section "
		   <<PLAYER_HEADER_SECTION_SPECIFIER<<", however this keyword "
		    "is only expected once in this section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(playerSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading sprite dir strings from ",
			    RULES_CONFIG_FILE_EXTENSION, " header "
			    "file \"", rulesFileName, "\""),
		     &playerInitData.spritePaths);
		  break;
		case 1:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading coord section (from ",
			    RULES_CONFIG_FILE_EXTENSION, " header file "
			    "\"", rulesFileName, "\") for the initial view port"
			    " position"),
		     &misc.initialRelViewPortCoordinates);
		     //&playerInitData.initialViewPortCoordinates);
		  break;
		case 2:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading coord section (from ",
			    RULES_CONFIG_FILE_EXTENSION, " header file "
			    "\"", rulesFileName, "\") for the initial player "
			    "coordinate"),
		     &playerInitData.initialCoordinates);
		  break;
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
		case 8:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 9:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading \"coord section\" (from",
			    RULES_CONFIG_FILE_EXTENSION, " header file "
			    "\"", rulesFileName, "\") for the view port "
			    "padding values"),
		     &misc.viewPortPadding);
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading "<<RULES_CONFIG_FILE_EXTENSION
		   <<" header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section "
		   <<PLAYER_HEADER_SECTION_SPECIFIER<<", however this keyword "
		    "is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

  readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat(PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
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
    new player(background,
	       playerInitData.spritePaths,
	       // misc.viewPortPadding,
	       // misc.initialRelViewPortCoordinates,
	       playerInitData.initialCoordinates,
	       playerInitData.direction, playerInitData.health,
	       playerInitData.gravitationalConstant,
	       playerInitData.maxVerticalVelocity,
	       playerInitData.maxFallingJumpNumber,
	       playerInitData.maxJumpNumber);
}


void initBgSprites
(const yx viewPortSize, const char rulesFileName[], rules & levelRules,
 const backgroundData & background, const std::string & rawRules,
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
     {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_DISPLAY_IN_FORGROUND, readBoolSection}};

  std::vector<std::string> targets {};
  std::string targetFound {};

    readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));
  
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
		  e<<"Error: reading "<<RULES_CONFIG_FILE_EXTENSION
		   <<" header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section "
		   <<BG_SPRITE_HEADER_SECTION_SPECIFIER<<" section, however "
		    "this keyword is only expected once in this section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(bgSpriteSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading background sprite dir strings from "
			    "", RULES_CONFIG_FILE_EXTENSION, " header file \"",
			    rulesFileName, "\""),
		     &bgSpriteInitData.spritePaths);
		  break;
		case 1:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading single coord section from ",
			    RULES_CONFIG_FILE_EXTENSION, " header"
			    "file  \"", rulesFileName, "\""),
		     &bgSpriteInitData.coordinate);
		  break;
		case 2:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 3:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading boolean section from ",
			    RULES_CONFIG_FILE_EXTENSION, " header file"
			    " \"", rulesFileName, "\""),
		     &bgSpriteInitData.displayInForground);
		  break;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading "<<RULES_CONFIG_FILE_EXTENSION
		   <<" header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section "
		   <<BG_SPRITE_HEADER_SECTION_SPECIFIER<<" section, however "
		    "this keyword is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

    readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));

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
    (new bgSprite(bgSpriteInitData.spritePaths, viewPortSize,
		  bgSpriteInitData.coordinate, bgSpriteInitData.direction,
		  bgSpriteInitData.displayInForground));
}



// SkipSpace has a default value.
void readSectionOpeningBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section, const bool skipSpace)
{
  using namespace levelFileKeywords;
    
  std::vector<std::string> targets {};
  std::string targetFound {};
      
  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets, skipSpace);
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {      
      exit(concat
	   ("Error: expected \"", RULES_HEADER_SECTION_START_DENOTATION,
	    "\" to denote the start of ", section, " when ", eMsg,
	    ". Encountered \"", *buffPos, "\"\n"), ERROR_RULES_LEV_HEADER);
    }
}


void readSectionEndingBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg,  const std::string & section)
{
  using namespace levelFileKeywords;
    
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets = {std::string {RULES_HEADER_SECTION_END_DENOTATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      exit(concat
	   ("Error: expected \"", RULES_HEADER_SECTION_END_DENOTATION, "\" to "
	    "denote the end of ", section, " when ", eMsg, ". Encountered \"",
	    *buffPos, "\"\n"), ERROR_RULES_LEV_HEADER);
    }
}


void readStringsSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * strings)
{
  using namespace levelFileKeywords;
  
  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (buff, buffPos, eMsg, concat("string section"));
  
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
          exit(concat
	       ("Error: expected \"", STRING_DENOTATION, "\" to denote the ",
		"start of a string when ", eMsg, ". Encountered \"", *buffPos,
		"\".\n"), ERROR_RULES_LEV_HEADER);
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
		  exit(concat
		       ("Error: file ended unexpectedly when ", eMsg, ".\n"),
		       ERROR_RULES_LEV_HEADER);
		}
	      else if(*buffPos == STRING_ESC)
		{
		  if(*peekBuffPos != STRING_DENOTATION &&
		     *peekBuffPos != STRING_ESC)
		    {
		      exit(concat
			   ("Error: encountered character (\"", *peekBuffPos,
			    "\") other then \"", STRING_ESC, "\" or \"",
			    STRING_DENOTATION, "\" after escape character \"",
			    STRING_ESC, "\" when ", eMsg, ".\n"),
			   ERROR_RULES_LEV_HEADER);
		    }
		  else
		    {
		      // Skip adding char at buffPos!
		      newString.push_back(*peekBuffPos);
		      peekBuffPos++;
		      if(peekBuffPos == buff.end())
			{
			  exit(concat
			       ("Error: file ended unexpectedly when ", eMsg,
				".\n"),
			       ERROR_RULES_LEV_HEADER);
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
	  exit(concat
	       ("Error: expected \"", STRING_SEPARATION, "\" or \"",
		RULES_HEADER_SECTION_END_DENOTATION, "\" to separate strings ",
		"or end the string section when ", eMsg, ". Encountered \"",
		*buffPos, "\".\n"), ERROR_RULES_LEV_HEADER);
	}
      else if(targetFound == std::string {RULES_HEADER_SECTION_END_DENOTATION})
	{
	  // We've read all the strings in the string section.
	  break;
	}
    }
}


void readSingleCoordSectionInNNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers");
}


void readSingleCoordSectionInNNumbersNoSkpSp
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers (without "
			 "skipping space up until the coordinate)", false);
}


void readSingleCoordSectionInZNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, true, coord, "integers");
}


// // SkipSpace has a default value.
void readSingleCoordSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers,
 void * coord, const std::string typeOfNumber, const bool skipSpace)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets {};
  std::string targetFound {};
  readSectionOpeningBracket
    (buff, buffPos, eMsg,
     concat("single coordinate section (with " , typeOfNumber, ")"),
     skipSpace);
  
  ((yx*)coord)->y = readSingleNum(buff, buffPos, eMsg, useIntegers);
      
  targets = {std::string {COORD_SEPARATION}};
  targetFound = skipSpaceUpTo
    (buff, buffPos, targets);
  if(targetFound == "")
    {
      exit(concat
	   ("Error: expected \"", COORD_SEPARATION, "\" before second ",
	    "coordinate component in single coordinate section (with ",
	    typeOfNumber, ") when ", eMsg, ". Encountered ", "\"", *buffPos,
	    "\"\n"), ERROR_RULES_LEV_HEADER);
    }

  ((yx*)coord)->x = readSingleNum(buff, buffPos, eMsg, useIntegers);

  readSectionEndingBracket
    (buff, buffPos, eMsg,
     concat("single coordinate section (with ", typeOfNumber, ")"));
}


void readBoolSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * boolean)
{
  using namespace levelFileKeywords;

  std::string sFalse {"false"}, sTrue {"true"}, nFalse {"0"}, nTrue {"1"};
  std::vector<std::string> targets
    {sFalse, sTrue, nFalse, nTrue};
  std::string targetFound {};

  readSectionOpeningBracket(buff, buffPos, eMsg, concat("bool section"));

  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == sFalse || targetFound == nFalse)
  {
    *((bool *)boolean) = false;
  }
  else if(targetFound == sTrue || targetFound == nTrue)
    {
      *((bool *)boolean) = true;
    }
  else
    {
      std::stringstream e {};
      e<<"Error: expected one of (";
	for(auto target: targets)
	  {
	    e<<target<<", ";
	  }
	e<<") in boolean section. When "<<eMsg<<". Encountered \""
	 <<*buffPos<<"\".\n";
	exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  readSectionEndingBracket(buff, buffPos, eMsg, concat("bool section"));
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
      misc.initialRelViewPortCoordinates = 
	defaultValues::misc::initialRelViewPortCoordinates;
    case 2:
      playerInitData.initialCoordinates =
	defaultValues::player::initialCoordinates;
      break;
    case 3:
      playerInitData.direction = defaultValues::player::direction;
      break;
    case 4:
      playerInitData.health = defaultValues::player::health;
      break;
    case 5:
      playerInitData.gravitationalConstant =
	defaultValues::player::gravitationalConstant;
      break;
    case 6:
      playerInitData.maxVerticalVelocity =
	defaultValues::player::maxVerticalVelocity;
      break;
    case 7:
      playerInitData.maxFallingJumpNumber =
	defaultValues::player::maxFallingJumpNumber;
      break;
    case 8:
      playerInitData.maxJumpNumber = defaultValues::player::maxJumpNumber;
      break;
    case 9:
      misc.viewPortPadding = defaultValues::misc::viewPortPadding;
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
    case 3:
      bgSpriteInitData.displayInForground =
	defaultValues::bgSprites::displayInForground;
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


//test level agains sprite's
char rules::intersection(const std::string & boundsInteraction,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any character other then space in
     boundsInteration return that charater */
  return 'n';
}


//test player sprite against sprite's
char rules::intersection(const std::vector<int> playerSpChoords,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return characte code or something */
  return 'n';
}


//test level agains sprite's
char rules::nearPass(const std::string & boundsInteraction,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords is next to any character other then space in
     boundsInteration return that charater */
  return 'n';
}


//test player sprite against sprite's
char rules::nearPass(const std::vector<int> playerSpChoords,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return a character code that corispondest to the side that was matched
     relative to spChoords for example 't' for top. (this might be used to
     detect that the player has "touched" the top of the sprite and if it is an
     enemy sprite then appropriate action can be taken. */
  return 'n';
}


void rules::movePlayer
  (backgroundData & background, sprite::directions input)
{
  const int currDir {gamePlayer->getDirection()};
  
  if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
     )
    {
      // Start jump.
      gamePlayer->startJumping(chunkSize, secondStageRulesBuffer);
      // We wan't to keep moving in the direction we were moving in before.
      input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
      gamePlayer->handleJumpingAndFalling
	(chunkSize, secondStageRulesBuffer);
    }

  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision();
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      // input = handleRightCollision(viewPortPosition, viewPortSize.y);
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      // input = handleLeftCollision(viewPortPosition, viewPortSize.y);
    }

  //handleFinalPlayerMovementAndWindowAndPaddingInteractionsSafe(input);
  // handleFinalPlayerMovement(input);

  gamePlayer->updatePosRel(input);
}


// void rules::handleFinalPlayerMovementAndWindowAndPaddingInteractionsSafe
// (const sprite::directions newDir)
// {
//   const yx peekPos {gamePlayer->peekAtPos(newDir)};
  
//       // We won't be outside of the level if we move.
//       handleFinalPlayerMovementAndWindowAndPaddingInteractions
// 	(newDir, peekPos);
// }


// void rules::handleFinalPlayerMovement(const sprite::directions newDir)
// {
  
// }


// void rules::movePlayerWhenInteractingWithInnerPadding
// (const sprite::directions input, const yx peekPos)
// {
//   /* We use this variable in the call's to inWindowInnerPadding() when peekPos
//      is out of the x boarder range. */
//   constexpr int REACHED_INNER_PADDING_X {0};
//   switch(input)
//     {
//     case sprite::DIR_NONE:
//       break;
//     case sprite::DIR_UP:
//       gamePlayer->updatePosRel(input);
//       break;
//     case sprite::DIR_RIGHT:
//       movePlayerRightWhenInteractingWithInnerPadding
// 	(input, peekPos, REACHED_INNER_PADDING_X);
//       break;
//     case sprite::DIR_DOWN:
//       gamePlayer->updatePosRel(input);
//       break;
//     case sprite::DIR_LEFT:
//       movePlayerLeftWhenInteractingWithInnerPadding
// 	(input, peekPos, REACHED_INNER_PADDING_X);
//       break;
//     }
// }


// void rules::movePlayerRightWhenInteractingWithInnerPadding
// (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
//  const size_t backgroundLength, const yx peekPos,
//  const int REACHED_INNER_PADDING_X)
// {
//   if(gamePlayer->leftOfWindowInnerRightPadding
//      (peekPos.x, PLAYER_MOVEMENT_AREA_PADDING.x, viewPortSize))
//     { // We are to the left of the inner right padding.
//       gamePlayer->updatePosRel(input);
//     }
//   else if(size_t(viewPortPosition.x + viewPortSize.x) < backgroundLength)
//     { // There is still background left to spare.
//       gamePlayer->updateDirection(input);
//       viewPortPosition.x++;	// Move background.
//     }
//   else if(size_t(viewPortPosition.x + viewPortSize.x) == backgroundLength)
//     { /* No background left, so move the player to the right
// 	 edge of the background. */
//       gamePlayer->updatePosRel(input);
//     }
// }


// void rules::movePlayerLeftWhenInteractingWithInnerPadding
// (const sprite::directions input, yx & viewPortPosition, const yx viewPortSize,
//  const size_t backgroundLength, const yx peekPos,
//  const int REACHED_INNER_PADDING_X)
// {
//   if(gamePlayer->rightOfWindowInnerLeftPadding
//      (peekPos.x, PLAYER_MOVEMENT_AREA_PADDING.x))
//     { // We are to the righ of the inner left padding.
//       gamePlayer->updatePosRel(input);
//     }
//   else if(viewPortPosition.x > 0)
//     { // There is still background left to spare.
//       gamePlayer->updateDirection(input);
//       viewPortPosition.x--;	// Move background.
//     }
//   else if(viewPortPosition.x == 0)
//     { /* No background left, so move the player to the left
// 	 edge of the background. */
//       gamePlayer->updatePosRel(input);
//     }
// }


// int rules::getClosestBoarderCharAbove(const int position,
// 				      const bool directContact)
// {
//   int minBoarderCharDistanceRet {0};
//   int yPos {};
    
//   std::vector<std::string> spriteTopCoords
//     {gamePlayer->getTopXAbsRangeAsStrsForOneOffContact(position)};

//   // All coordinates from spriteTopCoords and above.
//   std::vector<std::string> spriteTopCoordsColumn {};

//   for(std::string strCoord: spriteTopCoords)
//     {
//       std::stringstream coord {strCoord};
//       int spCoordY, spCoordX;
//       char eat;
//       coord>>spCoordY;
//       coord>>eat;
//       coord>>spCoordX;

//       yPos = spCoordY;

//       for(int y {spCoordY}; y >= 0; y--)
// 	{
// 	  std::stringstream newCoord {};
// 	  newCoord<<y;
// 	  newCoord<<',';
// 	  newCoord<<spCoordX;
// 	  spriteTopCoordsColumn.push_back(newCoord.str());
// 	}
//     }


//   bool foundBoarderChar {false};
//   for(std::string coord: spriteTopCoordsColumn)
//     {
//       if(coordChars.find(coord) != coordChars.end())
// 	{
// 	  foundBoarderChar = true;
// 	  std::stringstream boarderCoord {coord};
// 	  int boarderY {};
// 	  boarderCoord>>boarderY;
	  
// 	  minBoarderCharDistanceRet =
// 	    boarderY > minBoarderCharDistanceRet ? (yPos - boarderY):
// 	    (yPos - minBoarderCharDistanceRet);
// 	}
//       mvprintw(0, 0, "%d", minBoarderCharDistanceRet);
//     }

//   // for(std::string strCoord: spriteTopCoordsColumn)
//   //   {
//   //     std::stringstream coord {strCoord};
//   //     int y, x;
//   //     char eat;

//   //     coord>>y;
//   //     coord>>eat;
//   //     coord>>x;

//   //     if((x - position) < 250)
//   // 	{
//   // 	  mvprintw(y, (x - position), "@");
//   // 	  refresh();
//   // 	}
//   //     coord.clear();
//   //   }

//   return foundBoarderChar ? minBoarderCharDistanceRet: -1;
// }


/* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
sprite::directions rules::handleGroundCollision()
{
  sprite::directions retDir {sprite::DIR_DOWN};
  for(yx coord:
	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact())
    {
      char rule {};
      if(getCoordRule(coord, chunkSize, secondStageRulesBuffer, rule) &&
	 (rule == boarderRuleChars::BOARDER_CHAR ||
	  rule == boarderRuleChars::PLATFORM_CHAR))
	{
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
sprite::directions rules::handleRightCollision(const yx viewPortPosition,
					       const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_RIGHT};
  // const std::vector<yx> playerCoords
  //   {gamePlayer->getRightYAbsRangeAsStrsForOneOffContact(position)};
  // const yx bottomRightPlayerCoord
  //   {playerCoords[playerCoords.size() -1]};
  // bool stoppingContact {false};

  // char rule {};
  // for(yx playerCoord: playerCoords)
  //   {
  //     // If there is near contact and it's not with the bottom right coord.
  //     if(playerCoord != bottomRightPlayerCoord &&
  // 	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
  // 	 rule == BOARDER_CHAR)
  // 	{
  // 	  stoppingContact = true;
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }
  
  // if(!stoppingContact &&
  //    getCoordRule(bottomRightPlayerCoord, coordRules, backgroundHeight, rule) &&
  //    (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
  //   {
  //     if(gamePlayer->getPos().y > 0)
  // 	{
  // 	  /* If we've hit a "step" (as in the things that constitute staircases)
  // 	     and we are not at the minimum (top of window) y position, then
  // 	     "step up" :). */
  // 	  gamePlayer->updatePosRel(sprite::DIR_UP);
  // 	}
  //     else
  // 	{
  // 	  // We've hit the top of the window.
  // 	  retDir = sprite::DIR_NONE;
  // 	}
  //   }
  
  return retDir;
}


sprite::directions rules::handleLeftCollision(const yx viewPortPosition,
					      const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_LEFT};
  // const std::vector<yx> playerCoords
  //   {gamePlayer->getLeftYAbsRangeAsStrsForOneOffContact(position)};
  // const yx bottomLeftPlayerCoord
  //   {playerCoords[playerCoords.size() -1]};
  // bool stoppingContact {false};

  // char rule {};
  // for(yx playerCoord: playerCoords)
  //   {
  //     // If there is near contact and it's not with the bottom right coord.
  //     if(playerCoord != bottomLeftPlayerCoord &&
  // 	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
  // 	 rule == BOARDER_CHAR)
  // 	{
  // 	  stoppingContact = true;
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }

  // if(!stoppingContact &&
  //    getCoordRule(bottomLeftPlayerCoord, coordRules, backgroundHeight, rule) &&
  //    (rule == BOARDER_CHAR ||
  //     rule == PLATFORM_CHAR))
  //   {
  //     if(gamePlayer->getPos().y > 0)
  // 	{
  // 	  /* If we've hit a "step" and we are not at the minimum (top of window)
  // 	     y position, then "step up" :) */
  // 	  gamePlayer->updatePosRel(sprite::DIR_UP);
  // 	}
  //     else
  // 	{
  // 	  // We've hit the top of the window.
  // 	  retDir = sprite::DIR_NONE;
  // 	}
  //   }

  return retDir;
}


#ifdef DEBUG
#include <curses.h>
#include <stdlib.h>
#include "include/colorS.hpp"

void rules::printRuleChars(const yx viewPortSize)
{
  const colourMap cuMap;
  setColorMode setCuMode {0};
  
  for(int yIter {}; yIter < viewPortSize.y; ++yIter)
    {
      for(int xIter {}; xIter < viewPortSize.x; ++xIter)
	{
	  int sSRBIndex {yIter * viewPortSize.x + xIter};
	  if(secondStageRulesBuffer[sSRBIndex] != ' ')
	    {
	      setCuMode.setColor((sSRBIndex * rand()) % cuMap.colorPairs.size());
	      mvprintw(yIter, xIter,
		       concat("", secondStageRulesBuffer[sSRBIndex]).c_str());
	    }
	}
    }

  refresh();
  /* The game will slow down a bit in debug mode. */
  sleep(3);
}
#endif


void rules::startTimers()
{
  gameTiming.movePlayer.start();
}


void rules::physics
(backgroundData & background, const sprite::directions input)
{ 
  this->updateBuffers();
  
#ifdef DEBUG
  printRuleChars(background.chunkSize);
#endif

  if(gameTiming.movePlayer.startNextTick())
    {
      movePlayer
	(background, input);
      // background.updateViewPortPosition
      // 	(PLAYER_MOVEMENT_AREA_PADDING,
      // 	 gamePlayer->getPos(),
      // 	 gamePlayer->getMaxBottomRightOffset());      
      // gamePlayer->resetPositionWithPreviousPos
      // 	(updateViewPortPosition
      // 	 (PLAYER_MOVEMENT_AREA_PADDING,
      // 	  gamePlayer->getPos(background.getViewPortPosition()),
      // 	  gamePlayer->getMaxBottomRightOffset()));
    }
}
