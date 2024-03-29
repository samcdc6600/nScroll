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
       (const double fixedTimeStep, const yx viewPortSize,
	const char rulesFileName[], rules & levelRules,
	const backgroundData & background, const std::string & rawRules,
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
	 message and exit) *retObj should be populated with what is essential
	 the return value of the function. */
      void (* action)(const std::string & buff,
		     std::string::const_iterator & buffPos,
		     const std::string & eMsg, void * retObj);
      /* This function should be used as the action instead of the above if we
	 are at the top level of the header (in terms of sections). */
      void (* headerAction)(const double fixedTimeStep, const yx viewPortSize,
			    const char rulesFileName[], rules &levelRules,
			    const backgroundData & background,
			    const std::string & rawRules,
			    std::string::const_iterator & buffPos);
    };
  };

  namespace alphabet
  {
    namespace alphMisc
    {
      const std::string COMMENT_START_DELIMITER {"/*"};
      const std::string COMMENT_END_DELIMITER   {"*/"};
      // Each new section should start with a header char followed by this char.
      constexpr char RULES_HEADER_SECTION_START_DENOTATION  {'('};
      constexpr char RULES_HEADER_SECTION_END_DENOTATION    {')'};
      constexpr char STRING_DENOTATION		{'\"'};
      constexpr char STRING_SEPARATION		{','};
      constexpr char ALPHA_ESC_CHAR			{'\\'};
      constexpr char COORD_SEPARATION		{','};
      /* If this character is encountered in the main section of a
	 RULES_CONFIG_FILE_EXTENSION file the character 2 places after it should
	 be an ASCII number. After this number there can be a string of
	 contiguous ASCII numbers (up to some maximum) that together represent
	 some larger number. This number is the number of times the ASCII
	 character before the number should be repeated. */
      constexpr char RULES_MAIN_RUNLENGTH_BEGIN_CHAR {'R'};
    };
    
    namespace mainHeaders
    {
      /* Headers that can appear in the top level of a rules.lev file (they may
	 also appear in inner levels.) */
      const std::string PLAYER_HEADER_SECTION_SPECIFIER   {"player"};
      const std::string BG_SPRITE_HEADER_SECTION_SPECIFIER{"backgroundSprite"};
    };

    namespace sprites
    {
      // Header denoting a file directory/s section for sprite file/s.
      const std::string FILE_SECTION_HEADER	{"sprites"};
      
      namespace player
      {
	const std::string VIEW_PORT_INIT_COORD	{"initialViewPortCoordinate"};
	// Initial coordinate of the player relative to the view port.
	const std::string INIT_COORD_VP_REL
	  				  {"initialCoordinateViewPortRelative"};
	/* The player sprite cannot be within this many characters of any edge
	   of the view port. */
	const std::string VIEW_PORT_PADDING	{"viewPortPadding"};
      };
      
      const std::string INIT_COORD		{"initialCoordinate"};
      const std::string INIT_DIR 		{"initialDirection"};
      const std::string HEALTH      		{"initialHealth"};
      const std::string GRAV_CONST  		{"gravConst"};  
      /* The added to the y component of a sprites velocity when it jumps. This
         is also the maximum y velocity in the up direction. */
      const std::string JUMPING_POWER              {"jumpingPower"};
      /* How many jumps can a sprite do after it falls (without jumping off the
	 ground first) */
      const std::string MAX_FALL_JMP		{"maxJumpsAfterFall"};
      const std::string MAX_JMP_NUM 		{"maxJumps"};
      const std::string MAX_X_VELOCITY             {"maxXVelocity"};
      const std::string MAX_FALLING_VELOCITY	   {"maxFallingVelocity"};
      const std::string LEFT_ACCELERATION          {"leftAcceleration"};
      const std::string RIGHT_ACCELERATION         {"rightAcceleration"};
      /* Display sprite in front of or behind the player sprite. */
      const std::string DISPLAY_IN_FORGROUND	{"displayInForground"};
    };
  };

  
  // Used to store the order of a keyword and whether it has a default value.
  struct orderAndDefaultVal
  {
    int order;
    bool defaultVal;
  };
  /* Used to map keywords to unique int values so appropriate action can be
     taken for those keywords when parsing specific sections. KEY ORDER
     (first value of orderAndDefaultVal) OF THESE OBJECTS SHOULD MATCH switches
     that use these objects.! */
  const std::map<std::string, orderAndDefaultVal> headerSectionKeywordToId
    {
      {alphabet::mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
       						orderAndDefaultVal {0, false}},
      {alphabet::mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER,
       						orderAndDefaultVal {1, false}}
    };
  const std::map<std::string, orderAndDefaultVal> playerSectionKeywordToId
    {
      {alphabet::sprites::FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
      {alphabet::sprites::player::VIEW_PORT_INIT_COORD,
       						orderAndDefaultVal {1, false}},
      {alphabet::sprites::player::INIT_COORD_VP_REL,
       						orderAndDefaultVal {2, false}},
      {alphabet::sprites::INIT_DIR,	       	orderAndDefaultVal {3, true}},
      {alphabet::sprites::HEALTH,	       	orderAndDefaultVal {4, true}},
      {alphabet::sprites::GRAV_CONST,	       	orderAndDefaultVal {5, true}},
      {alphabet::sprites::JUMPING_POWER,	orderAndDefaultVal {6, true}},
      {alphabet::sprites::MAX_FALL_JMP,		orderAndDefaultVal {7, true}},
      {alphabet::sprites::MAX_JMP_NUM,	       	orderAndDefaultVal {8, true}},
      {alphabet::sprites::player::VIEW_PORT_PADDING,
					        orderAndDefaultVal {9, true}},
      {alphabet::sprites::MAX_X_VELOCITY,	orderAndDefaultVal {10, true}},
      {alphabet::sprites::MAX_FALLING_VELOCITY, orderAndDefaultVal {11, true}},
      {alphabet::sprites::LEFT_ACCELERATION, 	orderAndDefaultVal {12, true}},
      {alphabet::sprites::RIGHT_ACCELERATION,	orderAndDefaultVal {13, true}}
    };
  const std::map<std::string, orderAndDefaultVal> bgSpriteSectionKeywordToId
    {
      {alphabet::sprites::FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
      {alphabet::sprites::INIT_COORD,		orderAndDefaultVal {1, false}},
      {alphabet::sprites::INIT_DIR,     	orderAndDefaultVal {2, true}},
      {alphabet::sprites::DISPLAY_IN_FORGROUND,	orderAndDefaultVal {3, true}}
    };
  
  namespace defaultValues
  {
    /* These default values can only be used if defaultVal is set to true in
       orderAndDefaultVal for the values corresponding section in it's
       associated XXXSectionKeywordToId map (see above). */
    namespace player
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx initialCoordinatesVPRel		 {yHeight / 2, xWidth / 2};
      const sprite::directions direction	 {sprite::DIR_NONE};
      const int health				 {16};
      const double gravitationalConstant	 {-1.5};
      const double jumpingPower			 {80};
      const unsigned maxFallingJumpNumber	 {1};
      const unsigned maxJumpNumber		 {3};
      const double maxXVelocity			 {44};
      const double maxFallingVelocity		 {jumpingPower};
      const double leftAcceleration		 {2.5};
      const double rightAcceleration		 {2.5};
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
      const yx initialViewPortCoordinates {0, 0};
      const yx viewPortPadding {0, 0};
    }
  }
  /* This struct should be populated with the values that the player will
     eventually be initialised with. */
  struct playerInitialData
  {
    std::vector<std::string> spritePaths       	{};
    yx initialCoordinatesVPRel 		       	{};
    sprite::directions direction 		{};
    int health 					{};
    double gravitationalConstant 		{};
    double jumpingPower 			{};
    unsigned maxFallingJumpNumber 		{};
    unsigned maxJumpNumber 			{};
    double maxXVelocity 			{};
    double maxFallingVelocity 			{};
    double leftAcceleration			{};
    double rightAcceleration			{};
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
    yx initialViewPortCoordinates;
    yx viewPortPadding;
  } misc;
}


// ===== Headers Related To Loading RULES_CONFIG_FILE_EXTENSION Files START ====
// =============================================================================
/* removes any comments of the form from
   COMMENT_START_DELIMITER...COMMENT_END_DELIMITER
   Where ... is some string of zero or more length. Everything
   in between and including the comment delimiters is removed in place. If a
   malformed comment is found then eMsg is printed and the program is
   exited. Note that a comment can be placed anywhere except it cannot start in
   a string. Where fileType is the file type that buffer was read from. */
void removeComments(std::string & buffer, const std::string & eMsg,
		    const std::string fileType);
void initPlayer
(const double fixedTimeStep, const yx viewPortSize, const char rulesFileName [],
 rules & levelRules, const backgroundData & background,
 const std::string & rawRules, std::string::const_iterator & buffPos);
/* This function should be called for each background sprite section that's
   encountered. */
void initBgSprites
(const double fixedTimeStep, const yx viewPortSize, const char rulesFileName[],
 rules & levelRules, const backgroundData & background,
 const std::string & rawRules, std::string::const_iterator & buffPos);
void readSectionOpeningBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section,
 const bool skipSpace = true);
/* Attempts to read the bracket at the end of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionEndingBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section);
/* Creates a sprites using the arguments fixedTimeStep, viewPortSize and
   bgSpriteInitData. Stores this sprite in a vector in the
   map levelRules.bgSprites. The key to the vector is generated by calling the
   function createChunkCoordKeyFromCharCoord() with the sprites coordinates as
   it's argument. */
void createAndSaveSprite
(const double fixedTimeStep, const yx viewPortSize,
 levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, rules & levelRules);
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
/* Reads a section with one real number. */
void readSingleRealNumSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * real);
/* Same as readSingleRealNumSection with the exception that it will print an
   error message (that includes eMsg) if the number read is less than 0. */
void readSinglePositiveRealNumSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * real);
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


void rules::loadAndInitialiseCoordRules(const backgroundData & background)
{
    // if(!checkForPostfix(&fileName[0], COORD_RULES_FILE_EXTENSION))
    // {
    //   exit(concat
    // 	   ("Error: opening file \"", fileName, "\" as a coord rules "
    // 	    "file. Postfix for ", COORD_RULES_FILE_EXTENSION, " files should "
    // 	    "be \"", COORD_RULES_FILE_EXTENSION, "\"."), ERROR_OPENING_FILE);
    // }

  //   std::fstream cRFile(fileName, std::ios::in | std::ios::binary);

  // if(!cRFile)
  //   {      exit(concat("Error: failed to open coord rules file \"",
  // 		       fileName, "\"."), ERROR_OPENING_FILE);
  //   }
  // else if(cRFile.peek() == std::ifstream::traits_type::eof())
  //   {
  //     exit(concat("Error: coord rules file \"", fileName, "\" found to be "
  // 		  "empty. \"", COORD_RULES_FILE_EXTENSION, "\" files must not "
  // 		  "be empty."), ERROR_BACKGROUND);
  //   }
  
  // initialiseCoordRules
  //   (cRFile, chunkMap, background);
  // cRFile.close();
}


void rules::initialiseCoordRules
(std::fstream & cRFile, chunkMapType & coordRules,
 const backgroundData & background) const
{
  yx mapCoord {};
  std::vector<chunkElementBaseType> mapChunk;
  int mapChunksRead {};

    while(getCRChunk(cRFile, mapChunk, chunkSize, mapCoord,
		   fileName, true))
      {
	mapChunksRead++;
	insertChunk(mapCoord, mapChunk, mapChunksRead, fileName,
		    coordRules);
	mapChunk.clear();
      }

  /* Each chunk in background must have a matching chunk in coordRules. This
     will simplify logic in other areas somewhat. It does means that
     COORD_RULES_FILE_EXTENSION files will be larger than they would technically
     need to be, however this shouldn't be much of a problem since the files
     wouldn't be too large anyway (especially with compressed chunks.) Although
     some extra memory will be used the game logic will be slightly simpler. */
  verifyTotalOneToOneOntoMappingOfCoordToBgKeys(coordRules, background);
}


// Note that multiChunkFile has a default value of false.
bool rules::getCRChunk
(std::fstream & file, std::vector<chunkElementBaseType> & chunk,
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile) const
{
 //  const std::string eMsgStart
 //    {concat("Error: trying to read in coord rules chunk file "
 // 	    "\"", fileName, "\". ")};
  
 //  if(!readInChunkCoordFromFile(chunkCoord, fileName, file, !multiChunkFile))
 //    {
 //      return false;
 //    }

 //  // Read in chunk body.
 //  int yIter {}, xIter {};
 //  char cRChar {};
 //  int chunkCharsFound {};
 //  while(file.read(&cRChar, sizeof(char)))
 //    {
 //      if(cRChar == cRRunLengthSequenceSignifier)
 // 	{
 // 	  char highByte {}, lowByte {};
 // 	  if(!file.read(&highByte, sizeof(char)))
 // 	    {
 // 	      exit(concat(eMsgStart, "File ends after run-length sequence "
 // 			  "signifier (or an IO error has occurred.)"),
 // 		   ERROR_MALFORMED_FILE);
 // 	    }
 // 	  if(!file.read(&lowByte, sizeof(char)))
 // 	    {
 // 	      exit(concat(eMsgStart, "File ends after run-length sequence "
 // 			  "signifier and first byte of length (or an IO error "
 // 			  "has occurred.)"), ERROR_MALFORMED_FILE);
 // 	    }
 // 	  if(!file.read(&cRChar, sizeof(char)))
 // 	    {
 // 	      exit(concat(eMsgStart, "File ends after run-length "
 // 			  "sequence signifier and length bytes (or an IO error "
 // 			  "has occurred.)"), ERROR_MALFORMED_FILE);
 // 	    }
 // 	  for(int iter {}; iter < ((static_cast<unsigned char>(highByte) << 8) |
 // 				   static_cast<unsigned char>(lowByte)); iter++)
 // 	    {
 // 	      if(!checkYOfVirtualNestedChunkLoop
 // 		 (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
 // 		{
 // 		  /* This is a multi chunk file and we've just read past the end of
 // 		     a chunk so we need to back up. */
 // 		  file.seekg(-sizeof(char), std::ios::cur);
 // 		  goto MEGA_BREAK;
 // 		}
 // 	      chunk.push_back(cRChar);
 // 	      chunkCharsFound++;
 // 	      updateVirtualNestedChunkYXLoop(xIter, yIter, chunkSize);
 // 	    }
 // 	}
 //      else
 // 	{
 // 	  if(!checkYOfVirtualNestedChunkLoop
 // 	     (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
 // 	    {
 // 	      /* This is a multi chunk file and we've just read past the end of
 // 		 a chunk so we need to back up. */
 // 	      file.seekg(-sizeof(char), std::ios::cur);
 // 	      goto MEGA_BREAK;
 // 	    }
 // 	  chunk.push_back(cRChar);
 // 	  chunkCharsFound++;
 // 	  updateVirtualNestedChunkYXLoop(xIter, yIter, chunkSize);
 // 	}
 //    }

 // MEGA_BREAK:
 //  if(chunkCharsFound < chunkSize.y * chunkSize.x)
 //    {
 //      if(multiChunkFile)
 // 	{
 // 	  return false;
 // 	}
 //      else
 // 	{
 // 	  exit(concat(eMsgStart, "Chunk is too small (", chunkCharsFound, ") "
 // 		      "It should be ", chunkSize.y * chunkSize.x, "."),
 // 	       ERROR_MALFORMED_FILE);
 // 	}
 //    }

  return true;
}


void rules::verifyTotalOneToOneOntoMappingOfCoordToBgKeys
(const chunkMapType & coordRules, const backgroundData & background) const
{
  if(coordRules.size() != (unsigned long)background.numberOfChunks())
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


void rules::parseRulesConfigFileAndInitialiseVariablesStageOne
(const char rulesFileName [], std::string & rulesBuffer,
 const backgroundData & background)
{
  removeComments(rulesBuffer, concat
		 ("attempting to remove comments from ",
		  COORD_RULES_FILE_EXTENSION, " file (", rulesFileName, ")"),
		 COORD_RULES_FILE_EXTENSION);
  parseRulesConfigFileAndInitialiseVariablesStageTwo
    (rulesFileName, rulesBuffer, background);
}


void rules::parseRulesConfigFileAndInitialiseVariablesStageTwo
(const char rulesFileName [], const std::string & rulesBuffer,
 const backgroundData & background)
{
  using namespace levelFileKeywords;
  using namespace alphabet;

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
     {mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER, nullptr, initPlayer, false},
     keywordAction::headerKeywordAction
     {mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER, nullptr, initBgSprites,
      true}};

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
		     (this->fixedTimeStep, chunkSize, rulesFileName,
		      *this, background, rulesBuffer, buffPos);
		   break;
		 case 1:
		   /* We don't set found here because this keyword should have
		      headerKeywordAction.foundMultipleOptional set to true. */
		   headerKeywordActions[foundIter].headerAction
		     (this->fixedTimeStep, chunkSize, rulesFileName,
		      *this, background, rulesBuffer, buffPos);
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
  INITIAL_VIEW_PORT_COORDINATES = misc.initialViewPortCoordinates;
}


void rules::checkInitPlayerPosAndPadding()
{
  /* We must check the player position and padding as the absolute player
     position is calculated using the relative player position and the view port
     port position and as such if the magnitude of the relative player position
     is too large the player will not be visible or will be in the
     padding area. This of course is an error. */

  const yx maxBR {gamePlayer->getMaxBottomRightOffset()};
  const yx relPlayerPos {gamePlayer->getPos()};
  

  if((PLAYER_MOVEMENT_AREA_PADDING + yx{1}).y + maxBR.y / 2 >
     (chunkSize.y / 2) ||
     (PLAYER_MOVEMENT_AREA_PADDING + yx{1}).x + maxBR.x / 2 >
     (chunkSize.x / 2))
    {
      // Padding is too large.
      exit(concat("Error: ", levelFileKeywords::alphabet::sprites::player::
		  VIEW_PORT_PADDING, " + ", maxBR + yx{1}, " (player height and"
		  " width) / 2 ", PLAYER_MOVEMENT_AREA_PADDING + yx{1} +
		  yx{maxBR.y / 2, maxBR.x / 2}, " is more than chunk size / 2 ",
		  yx{chunkSize.y / 2, chunkSize.x / 2}, " and shouldn't be."),
	   ERROR_PLAYER_PADDING_RANGE);
    }
  else if(relPlayerPos.y < PLAYER_MOVEMENT_AREA_PADDING.y ||
	  relPlayerPos.x < PLAYER_MOVEMENT_AREA_PADDING.x ||
	  (relPlayerPos + maxBR + yx{1}).y >
	  (chunkSize - PLAYER_MOVEMENT_AREA_PADDING).y ||
	  (relPlayerPos + maxBR + yx{1}).x >
	  (chunkSize - PLAYER_MOVEMENT_AREA_PADDING).x)
    {
      // Player position is out of bounds.
      exit(concat("Error: initial relative player position ", relPlayerPos,
		  " out of bounds. Must be within padding (relative to the "
		  "view port). That is between ", PLAYER_MOVEMENT_AREA_PADDING,
		  " and ", chunkSize - (PLAYER_MOVEMENT_AREA_PADDING + maxBR +
					yx{1}), "."),
	   ERROR_PLAYER_PADDING_RANGE);
    }
}


void removeComments(std::string & buffer, const std::string & eMsg,
		    const std::string fileType)
{
  using namespace levelFileKeywords::alphabet::alphMisc;

  bool inComment {false};
  bool inString {false};
  ssize_t commentStartsAt {};
  
  for(ssize_t iter {};
      iter < (ssize_t)buffer.size() -
	((ssize_t)COMMENT_END_DELIMITER.size() -1) &&
	iter < (ssize_t)buffer.size() -
	((ssize_t)COMMENT_START_DELIMITER.size() -1); ++iter)
    {
      if(!inComment)
	{
	  if(!checkForStringInBufferAtPos
	     (buffer, iter, concat("", ALPHA_ESC_CHAR, STRING_DENOTATION)) &&
	     checkForStringInBufferAtPos
	     (buffer, iter +1, concat("", STRING_DENOTATION)))
	    {
	      if(!inString)
		{
		  /* We've encountered an un-escaped STRING_DENOTATION character
		     when not in a comment or string. This means that we are now
		     in a string. */
		  inString = true;
		}
	      else
		{
		  /* We've encountered an un-escaped STRING_DENOTATION character
		     when not in a comment but in a string. This means that we
		     are no longer in a string. */
		  inString = false;
		}
	    }
	  else if(checkForStringInBufferAtPos
	      (buffer, iter, COMMENT_START_DELIMITER) && !inString)
	    {
	      /* NOTE THAT A COMMENT START CANNOT BE ESCAPED. */
	      inComment = true;
	      commentStartsAt = iter;
	      iter += COMMENT_START_DELIMITER.size() -1;
	    }
	}
      else if(checkForStringInBufferAtPos
	      (buffer, iter, COMMENT_END_DELIMITER))
	{
	  /* NOTE THAT A COMMENT END CANNOT BE ESCAPED. */
	  inComment = false;
	  buffer.erase(commentStartsAt, iter + COMMENT_END_DELIMITER.size() - commentStartsAt);
	  iter = commentStartsAt;
	}
    }

  if(inComment)
    {
      exit(concat("Error: found un-closed comment when ", eMsg, ". Note that "
		  "comments can be placed anywhere in a ", fileType, " file, "
		  "with the exception that a comment cannot start inside a "
		  "string (that is after an uneven number of (",
		  STRING_DENOTATION, ")'s that are not in comments and are not "
		  "escaped.)"),
	   ERROR_BAD_COMMENT);
    }
}


void initPlayer
(const double fixedTimeStep, const yx viewPortSize, const char rulesFileName[],
 rules & levelRules, const backgroundData & background,
 const std::string & rawRules, std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;
  using namespace alphabet;

  /* Setup keyword actions associations for player section. This should contain
     an entry for each thing that the player constructor takes. If there is a
     variable that the player constructor needs but we don't want to be
     specifiable in rules.lev files then it's entry here should have a default
     value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> playerHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {sprites::FILE_SECTION_HEADER,	     readStringsSection},
     keywordAction::headerKeywordAction
     {sprites::player::VIEW_PORT_INIT_COORD, readSingleCoordSectionInZNumbers},
     keywordAction::headerKeywordAction
     {sprites::player::INIT_COORD_VP_REL,    readSingleCoordSectionInNNumbers},
     keywordAction::headerKeywordAction
     {sprites::INIT_DIR,		     nullptr},
     keywordAction::headerKeywordAction
     {sprites::HEALTH,	 		     nullptr},
     keywordAction::headerKeywordAction
     {sprites::GRAV_CONST, 		     nullptr},
     keywordAction::headerKeywordAction
     {sprites::JUMPING_POWER,		     readSinglePositiveRealNumSection},
     keywordAction::headerKeywordAction
     {sprites::MAX_FALL_JMP, 		     nullptr},
     keywordAction::headerKeywordAction
     {sprites::MAX_JMP_NUM, 		     nullptr},
     keywordAction::headerKeywordAction
     {sprites::player::VIEW_PORT_PADDING,    readSingleCoordSectionInNNumbers},
     keywordAction::headerKeywordAction
     {sprites::MAX_X_VELOCITY,		     readSinglePositiveRealNumSection},
     keywordAction::headerKeywordAction
     {sprites::MAX_FALLING_VELOCITY,	     readSinglePositiveRealNumSection},
     keywordAction::headerKeywordAction
     {sprites::LEFT_ACCELERATION,	     readSinglePositiveRealNumSection},
     keywordAction::headerKeywordAction
     {sprites::RIGHT_ACCELERATION,	     readSinglePositiveRealNumSection}};

  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ",
	    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER, " from \"",
	    rulesFileName, "\""),
     concat("a ", mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
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
		  exit(concat
		       ("Error: reading ", RULES_CONFIG_FILE_EXTENSION,
			" header file \"", rulesFileName, "\". Encountered "
			"keyword \"", targetFound, "\" twice when reading "
			"header sub section ",
			mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			", however this keyword is only expected once in this "
			"section.\n").c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(playerSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading sprite dir strings form ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\""),
		     &playerInitData.spritePaths);
		  break;
		case 1:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading coord section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the "
			    "initial view port position"),
		     &misc.initialViewPortCoordinates);
		  break;
		case 2:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading coord section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the "
			    "initial relative player coordinate"),
		     &playerInitData.initialCoordinatesVPRel);
		  break;
		case 3:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 4:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 5:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 6:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat
		     ("reading positive real number section (from ",
		      mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
		      " section of ", RULES_CONFIG_FILE_EXTENSION, " header "
		      "file \"", rulesFileName, "\") for the initial players "
		      "jumping power"), &playerInitData.jumpingPower);
		  break;
                case 7:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 8:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 9:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading \"coord section\" (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the view"
			    "port padding values"),
		     &misc.viewPortPadding);
		  break;
		case 10:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading positive real number section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the "
			    "maximum x velocity"),
		     &playerInitData.maxXVelocity);
		  break;
		case 11:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading positive real number section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the "
			    "maximum falling velocity"),
		     &playerInitData.maxFallingVelocity);
		  break;
		case 12:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading positive real number section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the left"
			    " acceleration"),
		     &playerInitData.leftAcceleration);
		  break;
		case 13:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading positive real number section (from ",
			    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			    " section of ", RULES_CONFIG_FILE_EXTENSION,
			    " header file \"", rulesFileName, "\") for the "
			    "right acceleration"),
		     &playerInitData.rightAcceleration);
		  break;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  exit(concat
		       ("Error: reading ", RULES_CONFIG_FILE_EXTENSION,
			" header file \"", rulesFileName, "\". Encountered "
			"keyword \"", targetFound, "\" when reading header sub "
			"section ",
			mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER,
			", however this keyword is forbidden.\n").c_str(),
		       ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

  readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ",
	    mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER, " from \"",
	    rulesFileName, "\""),
     concat(mainHeaders::PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
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

  /* NOTE THAT PLAYER::VERIFYIFMOVEMENTLIMITSLESSTHANFIXEDTIMESTEP() MUST BE
     UPDATED IF ANY PLAYER MOVEMENT LIMITS ARE ADDED HERE! */
  levelRules.gamePlayer =
    new player(fixedTimeStep,
	       background,
	       playerInitData.spritePaths,
	       playerInitData.initialCoordinatesVPRel,
	       playerInitData.direction,
	       levelRules.firstStageBufferSizeInChars,
	       playerInitData.health,
	       playerInitData.gravitationalConstant,
	       playerInitData.jumpingPower,
	       playerInitData.maxFallingJumpNumber,
	       playerInitData.maxJumpNumber,
	       playerInitData.maxXVelocity,
	       playerInitData.maxFallingVelocity,
	       playerInitData.leftAcceleration,
	       playerInitData.rightAcceleration);
}


void initBgSprites
(const double fixedTimeStep, const yx viewPortSize, const char rulesFileName[],
 rules & levelRules, const backgroundData & background,
 const std::string & rawRules, std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;
  using namespace alphabet;
  /* Setup keyword actions associations for background sprite section. This
     should contain an entry for each thing that the bgSprite constructor takes.
     If there is a variable that the bgSprite constructor needs but we don't
     want to be specifiable in rules.lev files then it's entry here should have
     a default value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> bgSpriteHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {alphabet::sprites::FILE_SECTION_HEADER, readStringsSection},
     keywordAction::headerKeywordAction
     {sprites::INIT_COORD,		      readSingleCoordSectionInZNumbers},
     keywordAction::headerKeywordAction
     {sprites::INIT_DIR,		      nullptr},
     keywordAction::headerKeywordAction
     {sprites::DISPLAY_IN_FORGROUND,	      readBoolSection}};

  std::vector<std::string> targets {};
  std::string targetFound {};

    readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ",
	    mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER, " from \"",
	    rulesFileName, "\""),
     concat("a ", mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));
  
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
		  exit(concat
		       ("Error: reading ", RULES_CONFIG_FILE_EXTENSION,
			" header file \"", rulesFileName, "\". Encountered "
			"keyword \"", targetFound, "\" twice when reading "
			"header sub section ",
			mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER,
			" section, however this keyword is only expected once "
			"in this section.\n").c_str(), ERROR_RULES_LEV_HEADER);
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
		  exit(concat
		       ("Error: reading ", RULES_CONFIG_FILE_EXTENSION,
			" header file \"", rulesFileName, "\". Encountered "
			"keyword \"", targetFound, "\" when reading header sub"
			"section ",
			mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER,
			" section, however this keyword is forbidden.\n"
			).c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

    readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ",
	    mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER, " from \"",
	    rulesFileName, "\""),
     concat("a ", mainHeaders::BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));

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

  /* Create sprite and copy save reference into bgSprites data structure. */
  createAndSaveSprite
    (fixedTimeStep, viewPortSize, bgSpriteInitData, levelRules);
}



// SkipSpace has a default value.
void readSectionOpeningBracket
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const std::string & section, const bool skipSpace)
{
  using namespace levelFileKeywords::alphabet::alphMisc;
    
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
  using namespace levelFileKeywords::alphabet::alphMisc;
    
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


void createAndSaveSprite
(const double fixedTimeStep, const yx viewPortSize,
 levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, rules & levelRules)
{
    using namespace levelFileKeywords;
    /* bgSpriteInitData.coordinate to chunk coordinate is already in bgSprites
     retrieve associated vector from bg sprites and push_back new sprite.
     Else push back new vector with new sprite. */

    // Map key that will hold the vector the new sprite will be referenced by.
    std::string spriteChunkKey
      {createChunkCoordKeyFromCharCoord(bgSpriteInitData.coordinate)};
    bgSprite * newBgSprite
      {(new bgSprite(fixedTimeStep, bgSpriteInitData.spritePaths, viewPortSize,
		     bgSpriteInitData.coordinate, bgSpriteInitData.direction,
		     bgSpriteInitData.displayInForground))};    
  try
    {
      /* Try and get reference to vector containing bg sprites in the same
	 chunk as newBgSprite. Then add newBgSprite to vector. If no vector
	 exists for the key then at will throw an exception.  */
      std::vector<bgSprite *> & chunkBgSprites
	{levelRules.bgSprites.at
	 (createChunkCoordKeyFromCharCoord(bgSpriteInitData.coordinate))};
      chunkBgSprites.push_back(newBgSprite);
    }
  catch(const std::out_of_range & err)
    {
      /* Vector for (potential) sprites in the chunk this sprite is in was not
	 found. Create one, add newBgSprite to it and add it to the map using
	 the key. */
      std::vector<bgSprite *> newBgSpritesVector {newBgSprite};
      if(levelRules.bgSprites.insert
	 (std::pair<std::string, std::vector<bgSprite *>>
	  (spriteChunkKey, newBgSpritesVector)).second == false)
	{
	  exit(concat
	       ("Error: duplicate background sprite chunk key generated when "
		"trying to add background sprite chunk vector to "
		"levelRules.bgSprites. The duplicate key is (",
		spriteChunkKey, "). Note that the cause of this message is a "
		"software bug as no attempt should be made to insert a vector "
		"with a duplicate key."),
	       ERROR_BAD_LOGIC);
	}
    }
}


void readStringsSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * strings)
{
  using namespace levelFileKeywords::alphabet::alphMisc;
  
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
	      else if(*buffPos == ALPHA_ESC_CHAR)
		{
		  if(*peekBuffPos != STRING_DENOTATION &&
		     *peekBuffPos != ALPHA_ESC_CHAR)
		    {
		      exit(concat
			   ("Error: encountered character (\"", *peekBuffPos,
			    "\") other then \"", ALPHA_ESC_CHAR, "\" or \"",
			    STRING_DENOTATION, "\" after escape character \"",
			    ALPHA_ESC_CHAR, "\" when ", eMsg, ".\n"),
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
		  // BuffPos isn't pointing at a ALPHA_ESC_CHAR char.
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


// SkipSpace has a default value.
void readSingleCoordSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers,
 void * coord, const std::string typeOfNumber, const bool skipSpace)
{
  using namespace levelFileKeywords::alphabet::alphMisc;
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


void readSingleRealNumSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * real)
{
  using namespace levelFileKeywords::alphabet::alphMisc;
  std::vector<std::string> targets {};
  std::string targetFound {};
  readSectionOpeningBracket
    (buff, buffPos, eMsg, concat("single real number section"));
  *((double *)real) = readSingleRNum(buff, buffPos, eMsg);
  readSectionEndingBracket
    (buff, buffPos, eMsg, concat("single real number section"));		    
}


void readSinglePositiveRealNumSection
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * real)
{
  using namespace levelFileKeywords::alphabet::alphMisc;
  std::vector<std::string> targets {};
  std::string targetFound {};
  readSectionOpeningBracket
    (buff, buffPos, eMsg, concat("single real number section"));
  *((double *)real) = readSingleRNum(buff, buffPos, eMsg);
  if(*((double *)real) < 0)
    {
      exit(concat
	   ("Error: expected positive real number when ", eMsg, ". Encountered "
	    "number (", *((double *)real), ").\n" ), ERROR_RULES_LEV_HEADER);
    }
  readSectionEndingBracket
    (buff, buffPos, eMsg, concat("single real number section"));			    
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
      misc.initialViewPortCoordinates = 
	defaultValues::misc::initialViewPortCoordinates;
    case 2:
      playerInitData.initialCoordinatesVPRel =
	defaultValues::player::initialCoordinatesVPRel;
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
      playerInitData.jumpingPower = defaultValues::player::jumpingPower;
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
    case 10:
      playerInitData.maxXVelocity = defaultValues::player::maxXVelocity;
      break;
    case 11:
      playerInitData.maxFallingVelocity =
	defaultValues::player::maxFallingVelocity;
      break;
    case 12:
      playerInitData.leftAcceleration = defaultValues::player::leftAcceleration;
      break;
    case 13:
      playerInitData.rightAcceleration =
	defaultValues::player::rightAcceleration;
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


// //test level agains sprite's
// char rules::intersection(const std::string & boundsInteraction,
// 			 const std::vector<int> spChoords)
// {
//   /* if any value in spChoords matches any character other then space in
//      boundsInteration return that charater */
//   return 'n';
// }


// //test player sprite against sprite's
// char rules::intersection(const std::vector<int> playerSpChoords,
// 			 const std::vector<int> spChoords)
// {
//   /* if any value in spChoords matches any value in playerSpChoords
//      return characte code or something */
//   return 'n';
// }


// //test level agains sprite's
// char rules::nearPass(const std::string & boundsInteraction,
// 		     const std::vector<int> spChoords)
// {
//   /* if any value in spChoords is next to any character other then space in
//      boundsInteration return that charater */
//   return 'n';
// }


// //test player sprite against sprite's
// char rules::nearPass(const std::vector<int> playerSpChoords,
// 		     const std::vector<int> spChoords)
// {
//   /* if any value in spChoords matches any value in playerSpChoords
//      return a character code that corispondest to the side that was matched
//      relative to spChoords for example 't' for top. (this might be used to
//      detect that the player has "touched" the top of the sprite and if it is an
//      enemy sprite then appropriate action can be taken. */
//   return 'n';
// }


// void rules::movePlayer
// (backgroundData & background, sprite::directions input,
//  const double timeElapsed)
// {
//   const int currDir {gamePlayer->getDirection()};
//   // const yx potentialTravel {gamePlayer->getPotentialDistTravelled()};
  
//   if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
//      )
//     {
//       // // Start jump.
//       // gamePlayer->startJumping(secondStageRulesBuffer);
//       // // We wan't to keep moving in the direction we were moving in before.
//       // input = (sprite::directions)currDir;
//     }
//   else
//     {
//       /* We are not jumping or we are past the start of a jump.
// 	 If we can move down. */
//       gamePlayer->handleJumpingAndFalling(secondStageRulesBuffer);
//     }

//   // Handle contact with boarder characters.
//   if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
//      input == sprite::DIR_DOWN)
//     {
//       input = handleGroundCollision();
//       if(input = sprite::DIR_NONE)
// 	{
// 	  gamePlayer->velComp.setVlctY(0);
// 	}
//     }
//   else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
// 	  input == sprite::DIR_RIGHT)
//     {
//       // input = handleRightCollision(potentialTravel.y);
//     }
//   else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
// 	  input == sprite::DIR_LEFT)
//     {
//       // input = handleLeftCollision(potentialTravel.y);
//     }

//   // handleFinalPlayerMovementAndWindowAndPaddingInteractionsSafe(input);
//   // handleFinalPlayerMovement(input);

//   gamePlayer->updatePosRel(input);





  
//   // const int currDir {gamePlayer->getDirection()};
  
//   // if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
//   //    )
//   //   {
//   //     // Start jump.
//   //     gamePlayer->startJumping(secondStageRulesBuffer);
//   //     // We wan't to keep moving in the direction we were moving in before.
//   //     input = (sprite::directions)currDir;
//   //   }
//   // else
//   //   {
//   //     /* We are not jumping or we are past the start of a jump.
//   // 	 If we can move down. */
//   //     gamePlayer->handleJumpingAndFalling(secondStageRulesBuffer);
//   //   }

//   // // Handle contact with boarder characters.
//   // if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
//   //    input == sprite::DIR_DOWN)
//   //   {
//   //     input = handleGroundCollision();
//   //   }
//   // else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
//   // 	  input == sprite::DIR_RIGHT)
//   //   {
//   //     input = handleRightCollision();
//   //   }
//   // else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
//   // 	  input == sprite::DIR_LEFT)
//   //   {
//   //     input = handleLeftCollision();
//   //   }

//   // // handleFinalPlayerMovementAndWindowAndPaddingInteractionsSafe(input);
//   // // handleFinalPlayerMovement(input);

//   // gamePlayer->updatePosRel(input);
// }


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



// /* Checks for contact with boarder characters when moving down. Returns the
//    direction that the player should be moving in. */
// sprite::directions rules::handleGroundCollision()
// {
//   sprite::directions retDir {sprite::DIR_DOWN};
//   for(yx coord:
// 	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact())
//     {
//       char rule {};
//       if(getCoordRule(coord, chunkSize, secondStageRulesBuffer, rule) &&
// 	 (rule == boarderRuleChars::BOARDER_CHAR ||
// 	  rule == boarderRuleChars::PLATFORM_CHAR))
// 	{
// 	  retDir = sprite::DIR_NONE;
// 	  break;
// 	}
//     }
//   return retDir;
// }


// /* Checks for contact with boarder characters when moving right. Moves the
//    player up one character if a "step" is encountered (as long as the player
//    will not go out of bounds.) Returns the direction the player should move in.
// */
// sprite::directions rules::handleRightCollision()
// {
//   using namespace boarderRuleChars;
//   sprite::directions retDir {sprite::DIR_RIGHT};
//   const std::vector<yx> playerCoords
//     {gamePlayer->getRightYAbsRangeAsStrsForOneOffContact()};
//   const yx bottomRightPlayerCoord
//     {playerCoords[playerCoords.size() -1]};
//   bool stoppingContact {false};

//   char rule {};
//   for(yx playerCoord: playerCoords)
//     {
//       // If there is near contact and it's not with the bottom right coord.
//       if(playerCoord != bottomRightPlayerCoord &&
// 	 getCoordRule(playerCoord, chunkSize, secondStageRulesBuffer, rule) &&
// 	 rule == BOARDER_CHAR)
// 	{
// 	  stoppingContact = true;
// 	  retDir = sprite::DIR_NONE;
// 	  break;
// 	}
//     }
  
//   if(!stoppingContact && getCoordRule
//      (bottomRightPlayerCoord, chunkSize, secondStageRulesBuffer, rule) &&
//      (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
//     {
//       if(gamePlayer->getPos().y > 0)
// 	{
// 	  /* If we've hit a "step" (as in the things that constitute staircases)
// 	     and we are not at the minimum (top of window) y position, then
// 	     "step up" :). */
// 	  gamePlayer->updatePosRel(sprite::DIR_UP);
// 	}
//       else
// 	{
// 	  // We've hit the top of the window.
// 	  retDir = sprite::DIR_NONE;
// 	}
//     }
  
//   return retDir;
// }


// sprite::directions rules::handleLeftCollision()
// {
//   using namespace boarderRuleChars;
//   sprite::directions retDir {sprite::DIR_LEFT};
//   const std::vector<yx> playerCoords
//     {gamePlayer->getLeftYAbsRangeAsStrsForOneOffContact()};
//   const yx bottomLeftPlayerCoord
//     {playerCoords[playerCoords.size() -1]};
//   bool stoppingContact {false};

//   char rule {};
//   for(yx playerCoord: playerCoords)
//     {
//       // If there is near contact and it's not with the bottom right coord.
//       if(playerCoord != bottomLeftPlayerCoord &&
// 	 getCoordRule(playerCoord, chunkSize, secondStageRulesBuffer, rule) &&
// 	 rule == BOARDER_CHAR)
// 	{
// 	  stoppingContact = true;
// 	  retDir = sprite::DIR_NONE;
// 	  break;
// 	}
//     }

//   if(!stoppingContact && getCoordRule
//      (bottomLeftPlayerCoord, chunkSize, secondStageRulesBuffer, rule) &&
//      (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
//     {
//       if(gamePlayer->getPos().y > 0)
// 	{
// 	  /* If we've hit a "step" and we are not at the minimum (top of window)
// 	     y position, then "step up" :) */
// 	  gamePlayer->updatePosRel(sprite::DIR_UP);
// 	}
//       else
// 	{
// 	  // We've hit the top of the window.
// 	  retDir = sprite::DIR_NONE;
// 	}
//     }

//   return retDir;
// }


#ifdef DEBUG
#include <stdlib.h>
#include "include/colorS.hpp"

void rules::printRuleChars(const long double printInterval)
{
  setColorMode setCuMode {};

  static std::__1::chrono::system_clock::time_point tLast
    {std::chrono::system_clock::now()};
  static bool printRules {true};
  
  if((duration_cast<std::chrono::duration<long double>>
      (std::chrono::system_clock::now() - tLast)).count() >
     chronological::millisecToSec(printInterval))
    {
      tLast = std::chrono::system_clock::now();
      printRules = !printRules;
      
    }

  if(printRules)
    {
      for(int yIter {}; yIter < chunkSize.y; ++yIter)
	{
	  for(int xIter {}; xIter < chunkSize.x; ++xIter)
	    {
	      /* For a 3x3 buffer with a chunkSize of (48, 170) this should
		 calculate:
		 ((3 / 2) * 48 *
		 3 * 170) +
		 (3 * 170) * 0 +
		 ((3 / 2) * 170) + 0
		 =
		 (170 * 3 * 48) + 170
		 =
		 24650 */
	      int sSRBIndex
		{(secondStageBufferSizeInChunks < 3) ?
		 yIter * chunkSize.x + xIter:
		 // Initial y offset to middle chunk in sSRB.
		 ((secondStageBufferSizeInChunks / 2) * chunkSize.y *
		  secondStageBufferSizeInChunks * chunkSize.x) +
		 // Add yIter y offset.
		 (secondStageBufferSizeInChunks * chunkSize.x) * yIter +
		 // Add x offset to left middle chunk in sSRB and add x offset.
		 ((secondStageBufferSizeInChunks / 2) * chunkSize.x)  + xIter};
	    
	      if(secondStageRulesBuffer[sSRBIndex] != ' ')
		{
		  // setCuMode.setColorFromCharsetColor((sSRBIndex * rand()) %
		  // 		     colorParams::effectiveGameColorPairsNo);
		  setCuMode.setRandomColor();
		  mvprintw(yIter, xIter,
			   concat("", secondStageRulesBuffer[sSRBIndex]).c_str());
		}
	    }
	}
      refresh();
    }
}
#endif


void rules::startTimers()
{
  gameTiming.physics.start();
}


void rules::physics
(backgroundData & background, const sprite::directions input)
{   
      gamePlayer->move(secondStageRulesBuffer, input);
      // Update 2nd stage rules buffer position based on player position.
      this->updateViewPortPosition
	(PLAYER_MOVEMENT_AREA_PADDING, gamePlayer->getPos(),
	 gamePlayer->getMaxBottomRightOffset(),
	 gamePlayer->testIntersectionWithPaddingInTopOrLeft,
	 gamePlayer->testIntersectionWithPaddingInBottomOrRight);
      // Update 2nd stage background position based on rules buffer position.
      background.updateViewPortPosition
	(getViewPortPosition());
      /* Move the player back into the view port if they have moved out of the
	 view port. */
      gamePlayer->moveIntoViewPort(PLAYER_MOVEMENT_AREA_PADDING);
      
      // gamePlayer->resetPositionWithPreviousPos
      // 	(updateViewPortPosition
      // 	 (PLAYER_MOVEMENT_AREA_PADDING,
      // 	  gamePlayer->getPos(background.getViewPortPosition()),
      // 	  gamePlayer->getMaxBottomRightOffset()));

      // If the player has moved we must update the buffers.
      this->updateBuffers();
      background.updateBuffers();
}
