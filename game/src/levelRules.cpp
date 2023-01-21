#include "include/levelRules.hpp"
#include "include/player.hpp"
#include <string>


namespace levelFileKeywords
{
  namespace keywordAction
  {
    struct headerKeywordAction
    {
      headerKeywordAction
      (const std::string & kword, void (* a)
       (const std::string & buff, std::string::const_iterator & buffPos,
	const std::string & eMsg, void * retObj),
       void (*hA)(const yx maxyx, const char rulesFileName[],
		  rules &levelRules, const size_t bgSize,
		  const std::string &rawRules,
		  std::string::const_iterator &buffPos) = nullptr,
       const bool fMO = false)
	: keyword(kword), foundMultipleOptional(fMO), action(a), headerAction(hA)
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
      void (*action)(const std::string &buff,
		     std::string::const_iterator &buffPos,
		     const std::string &eMsg, void *retObj);
      /* This function should be used as the action instead of the above if we
	 are at the top level of the header (in terms of sections). */
      void (*headerAction)(const yx maxyx, const char rulesFileName[],
			   rules &levelRules, const size_t bgSize,
			   const std::string &rawRules,
			   std::string::const_iterator &buffPos);
    };
  };
  // Each new section should start with a header char followed by this char.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  // Header section header.
  const std::string PLAYER_HEADER_SECTION_SPECIFIER {"player"};
  const std::string BG_SPRITE_HEADER_SECTION_SPECIFIER {"backgroundSprite"};
  // Header sub section headers.
  // Sub section headers for general player and sprites.
  const std::string SPRITE_FILE_SECTION_HEADER		{"sprites"};
  const std::string SPRITE_INIT_COORD_SECTION_HEADER	{"initialCoordinate"};
  // Sub section headers for player sprite.
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
  const std::map<std::string, orderAndDefaultVal> headerSectionKeywordToId {
    {PLAYER_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {0, false}},
    {BG_SPRITE_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {1, false}}
  };
  const std::map<std::string, orderAndDefaultVal> playerSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
    {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
    {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
    {SPRITE_HEALTH_SECTION_HEADER,	orderAndDefaultVal {3, true}},
    {SPRITE_GRAV_CONST_SECTION_HEADER,	orderAndDefaultVal {4, true}},
    {SPRITE_MAX_VERT_V_SECTION_HEADER,	orderAndDefaultVal {5, true}},
    {SPRITE_MAX_FALL_JMP_SECTION_HEADER, orderAndDefaultVal {6, true}},
    {SPRITE_MAX_JMP_NUM_SECTION_HEADER,	orderAndDefaultVal {7, true}}
  };
  const std::map<std::string, orderAndDefaultVal> bgSpriteSectionKeywordToId {
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
      const yx coordinate {0, 1};
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
  }
  /* This struct should be populated with the values that the player will
     eventually be initialised with. */
  struct playerInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
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
  // Misc.
  constexpr char STRING_DENOTATION		{'\"'};
  constexpr char STRING_SEPARATION		{','};
  constexpr char STRING_ESC			{'\\'};
  constexpr char COORD_SEPARATION		{','};
  constexpr char DIR_START_ABS			{'/'};
  constexpr char DIR_START_REL			{'.'};
  /* The character used for escape sequences (within a string) in .rules.lev
     files. */
  constexpr char ESCAPE_CHAR {'\\'};
  constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
  constexpr char NULL_BYTE {'\0'};
  /* If this character is encountered in the main section of a rules.lev file
     the character 2 places after it should be an ASCII number. After this
     number there can be a string of contiguous ASCII numbers (up to some
     maximum) that together represent some larger number. This number is the
     number of times the ASCII character before the number should be repeated. */
  constexpr char RULES_MAIN_RUNLENGTH_BEGIN_CHAR {'R'};
}


rules::coordRulesType rules::loadAndInitialiseCoordRules
(const yx expectedChunkSize, const backgroundData & background,
 const char bgFileName [], const char coordRulesFileName [])
{
  std::string rawCoordRules {};
  coordRulesType coordRules;
  
  loadFileIntoString(coordRulesFileName, rawCoordRules,
		     "trying to read .coordRules.lev file");

  // parseRulesHeader(expectedChunkSize, coordRulesFileName, levelRules, bgSize, rawCoordRules, buffPos);
  initialiseCoordRules
    (expectedChunkSize, background, bgFileName, coordRulesFileName, coordRules,
     rawCoordRules);
  return coordRules;
}


void rules::initialiseCoordRules
(const yx expectedChunkSize, const backgroundData & background,
 const char bgFileName [], const char coordRulesFileName [],
 rules::coordRulesType & coordRuless, const std::string & coordRulesData)
{
  // std::string::const_iterator buffPos {coordRulesData.begin()};
  // const size_t expectedLineLength {bgSize / expectedChunkSize.y};
  // int lineNumber {};
  // int lineLength {};
  yx chunkCoord {};
  std::string chunk {};
  coordRulesChunk rawChunk {};
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
      if(getChunkCoordinate(coordRulesData, buffPos,
			    concat("trying  to read coord no. ", chunksReadIn,
				   " from coordRules.lev file \"",
				   coordRulesFileName, "\""), chunkCoord))
	{
	  skipSpaceUpToNextLine
	    (coordRulesData, buffPos,
	     concat("Error: trying to read chunk no. ", chunksReadIn,
		    ". Expected '\\n' after reading chunk coordinate (",
		    chunkCoord.y, ", ", chunkCoord.x, ") from coordRules.lev "
		    "file \"", coordRulesFileName, "\". Encountered other "
		    "character or EOF."));
	  // Get chunk from coord rules read from file.
	  getChunk(coordRulesData, buffPos,
		   concat("trying to read in chunk no. ", chunksReadIn,
			  ". from coordRules.lev file \"", coordRulesFileName,
			  "\"."), chunk, expectedChunkSize);

	  // Decompress chunk and return in rawChunk.
	  decompressChunk
	    (chunk, rawChunk, expectedChunkSize, chunksReadIn,
	     coordRulesFileName);
	  // Verify decompressed size...
	  // Insert chunk into...

	  chunksReadIn++;
	  // GetChunk() clears it's argument (chunk).
	  rawChunk.clear();
	}
      else
	{
	  break;
	}
    }
}


void rules::decompressChunk
(const std::string & chunkIn, coordRulesChunk & rawChunk,
 const yx expectedChunkSize, const ssize_t chunksReadIn,
 const char coordRulesFileName[])
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
		   ("Error: reading rules.lev header file \"",
		    coordRulesFileName, "\". Encountered line of length (",
		    lineLength, ") when ", "decompressing chunk no. ",
		    chunksReadIn, " (on line ", lineNumber + 1, " of chunk.) ",
		    "Expected a line of length (", expectedChunkSize.x, ")."),
		   ERROR_BACKGROUND);
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
	   ("Error: reading rules.lev header file \"", coordRulesFileName,
	    "\". Encountered line of length (", lineLength, ") when ",
	    "decompressing chunk no. ", chunksReadIn, " (on line ",
	    lineNumber + 1, " of chunk.) Expected a line of length (",
	    expectedChunkSize.x, ")."), ERROR_BACKGROUND);
    }

  const ssize_t expectedChunkSizeLinear
    {expectedChunkSize.y * expectedChunkSize.x};
  if(rawChunk.size() != expectedChunkSizeLinear)
    {
      exit(concat
	   ("Error: reading rules.lev header file \"", coordRulesFileName,
	    "\".  Size (", rawChunk.size(), ") of chunk no. ", chunksReadIn,
	    " not equal to expected size (", expectedChunkSizeLinear, ") of ",
	    "chunk."), ERROR_BACKGROUND);
    }
}


void rules::checkRuleChar(const char potentialRule, const std::string eMsg)
{
  for(char rule: boarderRuleChars::CHARS)
    {
      if(potentialRule == rule || potentialRule == ' ')
	{
	  return;
	}
    }
  std::stringstream e {};
  endwin();
  std::cout<<'\n'<<potentialRule<<std::endl;
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


// SkipSpace has a default value.
void rules::readSectionOpeningBracket
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


void rules::readSectionEndingBracket
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


void rules::readStringsSection
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


void rules::readSingleCoordSectionInNNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers");
}


void rules::readSingleCoordSectionInNNumbersNoSkpSp
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers (without "
			 "skipping space up until the coordinate)", false);
}


void rules::readSingleCoordSectionInZNumbers
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, true, coord, "integers");
}


// // SkipSpace has a default value.
void rules::readSingleCoordSection
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


void rules::readBoolSection
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


void rules::readEndOfHeader
(const std::string & buff, std::string::const_iterator & buffPos,
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
      exit(concat
	   ("Error: expected \"", RULES_HEADER_END_DENOTATION, "\" when ", eMsg,
	    ". Encountered \"", *buffPos, *(++buffPos), *(++buffPos), "\".\n"),
	   ERROR_RULES_LEV_HEADER);
    }
  buffPos += 2;
}


// void parseRulesMain(const yx maxyx, const char bgFileName [],
// 		    const char rulesFileName[], rules & levelRules,
// 		    const size_t bgSize,
// 		    const std::string & rawRules,
// 		    std::string::const_iterator & buffPos)
// {
//   // Main body of rules file should start on a new line (account for if.)
//   buffPos++;			
//   const size_t expectedLineLength {bgSize / maxyx.y};
//   int lineNumber {};
//   int lineLength {};
  
//   while(buffPos < rawRules.end())
//     {
//       if(*buffPos == levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR)
// 	{
// 	  // We've encountered a run-length encoding character.
// 	  const char ruleChar {*(++buffPos)};
// 	  ++buffPos;

// 	  checkRuleChar
// 	    (ruleChar,
// 	     concat("parsing main section of \"", rulesFileName, "\" (on line ",
// 		    lineNumber, " of main section)"));

// 	  const int runLength {readSingleNum
// 	    (rawRules, buffPos,
// 	     concat("reading run-length encoding length as a result of "
// 		    "encountering run-length encoding character \"",
// 		    levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR,
// 		    "\" while parsing main section of \"", rulesFileName,
// 		    "\""), false)};
// 	  --buffPos;

// 	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
// 	    {
// 	      levelRules.coordRules.push_back(ruleChar);
// 	    }
// 	  lineLength += runLength;
// 	}
//       else
// 	{
// 	  checkRuleChar(*buffPos,
// 			concat("parsing main section of \"",
// 			       rulesFileName,  "\" (on line ", lineNumber,
// 			       " of main section)"));
// 	  levelRules.coordRules.push_back(*buffPos);
// 	  lineLength++;
// 	}
      
//       buffPos++;
//       if(*buffPos == '\n')
// 	{
// 	  if(size_t(lineLength) != expectedLineLength)
// 	    {
// 	      std::stringstream e {};
// 	      e<<"Error: reading rules.lev header file \""<<rulesFileName
// 	       <<"\". Encountered line of length ("<<lineLength<<") (on line "
// 	       <<lineNumber<<" of main section) when reading main section of "
// 		"file. Expected a line of length ("<<expectedLineLength<<").";

// 	      exit(e.str().c_str(), ERROR_BACKGROUND);
// 	    }
// 	  lineLength = 0;
// 	  lineNumber++;
// 	  buffPos++;
// 	}
//     }

//   if(size_t(lineLength) != expectedLineLength)
//     {
//       std::stringstream e {};
//       e<<"Error: reading rules.lev header file \""<<rulesFileName
//        <<"\". Encountered line of length ("<<lineLength<<") (on line "
//        <<lineNumber<<") when reading main section of file. Expected a line "
// 	"of length ("<<expectedLineLength<<").";
//       exit(e.str().c_str(), ERROR_BACKGROUND);
//     }

//   if(levelRules.coordRules.size() != bgSize)
//     {
//       std::stringstream e {};
//       e << "Error: reading rules.lev header file \"" << rulesFileName
// 	<< "\". Size ("<<levelRules.coordRules.size()<<") of main section of "
// 	"file not equal to size ("<<bgSize<<") of background file \""
// 	<<bgFileName<<"\".";
//       exit(e.str().c_str(), ERROR_BACKGROUND);
//     }
// }


// void checkForDefaultPlayerValues
// (std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
//  playerHeaderKeywordActions,
//  levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
//  levelFileKeywords::playerInitialData & playerInitData, 
//  std::string::const_iterator & buffPos,
//  const char rulesFileName [])
// {
//   using namespace levelFileKeywords;

//   if(!playerSectionKeywordToId.at(keywordAction.keyword).defaultVal)
//     {
//       goto DEFAULT;
//     }
  
//   switch(playerSectionKeywordToId.at(keywordAction.keyword).order)
//     {
//     case 0:
//       playerInitData.spritePaths = defaultValues::player::spritePaths;
//       break;
//     case 1:
//       playerInitData.coordinate = defaultValues::player::coordinate;
//       break;
//     case 2:
//       playerInitData.direction = defaultValues::player::direction;
//       break;
//     case 3:
//       playerInitData.health = defaultValues::player::health;
//       break;
//     case 4:
//       playerInitData.gravitationalConstant =
// 	defaultValues::player::gravitationalConstant;
//       break;
//     case 5:
//       playerInitData.maxVerticalVelocity =
// 	defaultValues::player::maxVerticalVelocity;
//       break;
//     case 6:
//       playerInitData.maxFallingJumpNumber =
// 	defaultValues::player::maxFallingJumpNumber;
//       break;
//     case 7:
//       playerInitData.maxJumpNumber = defaultValues::player::maxJumpNumber;
//       break;
//     default:
//     DEFAULT:
//       std::stringstream e {};
//       e<<"Error: expected section\\s \"";
//       for(auto keywordAction: playerHeaderKeywordActions)
// 	{
// 	  if(!keywordAction.found && keywordAction.action != nullptr)
// 	    {
// 	      e<<keywordAction.keyword<<", ";
// 	    }
// 	}
//       e<<"\" in header sub section player. Encountered character \""
//        <<*buffPos<<"\", when reading \""<<rulesFileName<<"\" file\n";
//       exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
//     }
// }


// void checkForDefaultBgSpriteValues
// (std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
//  bgSpriteHeaderKeywordActions,
//  levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
//  levelFileKeywords::bgSpriteInitialData &bgSpriteInitData,
//  std::string::const_iterator &buffPos, const char rulesFileName[])
// {
//   using namespace levelFileKeywords;


//   if(!bgSpriteSectionKeywordToId.at(keywordAction.keyword).defaultVal)
//     {
//       goto DEFAULT;
//     }

//   switch(bgSpriteSectionKeywordToId.at(keywordAction.keyword).order)
//     {
//     case 0:
//       bgSpriteInitData.spritePaths = defaultValues::bgSprites::spritePaths;
//       break;
//     case 1:
//       bgSpriteInitData.coordinate = defaultValues::bgSprites::coordinate;
//       break;
//     case 2:
//       bgSpriteInitData.direction = defaultValues::bgSprites::direction;
//       break;
//     case 3:
//       bgSpriteInitData.displayInForground =
// 	defaultValues::bgSprites::displayInForground;
//       break;
//     default:
//     DEFAULT:
//       std::stringstream e {};
//       e<<"Error: expected section\\s \"";
//       for(auto keywordAction: bgSpriteHeaderKeywordActions)
// 	{
// 	  if(!keywordAction.found && keywordAction.action != nullptr)
// 	    {
// 	      e<<keywordAction.keyword<<", ";
// 	    }
// 	}
//       e<<"\" in header sub section backgroundSprite. Encountered character \""<<*buffPos
//        <<"\", when reading \""<<rulesFileName<<"\" file\n";
//       exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
//     }
// }


void rules::resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime)
{
  std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::high_resolution_clock::now()};
  if((duration_cast<std::chrono::duration<double>>
      (currentTime - oldTime)).count() >= rules::second)
    {
      oldTime = currentTime;
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


void rules::movePlayer(sprite::directions input,
		       int & position, const yx maxyx,
		       const size_t backgroundLength)
{ /* Move the player as long as they will stay within
     PLAYER_MOVMENT_INNER_MARGIN of the left and right borders. If the
     player is PLAYER_MOVMENT_INNER_MARGIN away from either the left or
     right boarder and tries to move closer to the boarder they are closest
     too then the background should be moved instead of the player in the
     appropriate direction (If there is background to spare.) If we are at
     either end of the level then the player cannot move further of
     course. */
  const int currDir {gamePlayer->getDirection()};
  
  if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
     )
    {
      // Start jump.
      //      gamePlayer->startJumping(position, maxyx, coordRulesCurrentContextBuffer);
      // We wan't to keep moving in the direction we were moving in before.
      input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
      //      gamePlayer->handleJumpingAndFalling(position, maxyx, coordRulesCurrentContextBuffer);
    }

  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      input = handleRightCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      input = handleLeftCollision(position, maxyx.y);
    }

  handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
    (input, position, maxyx, backgroundLength);
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength)
{
  const yx peekPos {gamePlayer->peekAtPos(newDir)};
  if(gamePlayer->inLevelY(peekPos.y, maxyx.y) &&
     gamePlayer->inLevelX(peekPos.x + position, backgroundLength))
    {
      // We won't be outside of the level if we move.
      handleFinalPlayerMovementAndWindowAndMarginInteractions
	(newDir, position, maxyx, backgroundLength, peekPos);
    }
  else
    {
      gamePlayer->updateDirection(sprite::DIR_NONE);
    }
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractions
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
    /* Make any final movement, check for window margin contact and take
     appropriate action if such contact is made. */
  if(((newDir == sprite::DIR_LEFT || newDir == sprite::DIR_RIGHT) &&
      gamePlayer->notInWindowInnerMarginX(peekPos.x,
					  PLAYER_MOVEMENT_INNER_MARGIN.x)) ||
     newDir == sprite::DIR_NONE)	// Only need to check for DIR_NONE here.
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else if((newDir == sprite::DIR_DOWN || newDir == sprite::DIR_UP)
	  && gamePlayer->notInWindowInnerMarginY(peekPos.y,
						 PLAYER_MOVEMENT_INNER_MARGIN.y))
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else
    {
      movePlayerWhenInteractingWithInnerMargin(newDir, position, maxyx,
					       backgroundLength, peekPos);
    }
}


void rules::movePlayerWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
  /* We use this variable in the call's to inWindowInnerMargin() when peekPos
     is out of the x boarder range. */
  constexpr int REACHED_INNER_MARGIN_X {0};
  switch(input)
    {
    case sprite::DIR_NONE:
      break;
    case sprite::DIR_UP:
      gamePlayer->updatePosRel(input);
      break;
    case sprite::DIR_RIGHT:
      movePlayerRightWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    case sprite::DIR_DOWN:
      gamePlayer->updatePosRel(input);
      break;
    case sprite::DIR_LEFT:
      movePlayerLeftWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    }
}


void rules::movePlayerRightWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->leftOfWindowInnerRightMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x, maxyx))
    { // We are to the left of the inner right margin.
      gamePlayer->updatePosRel(input);
    }
  else if(size_t(position + maxyx.x) < backgroundLength)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position++;	// Move background.
    }
  else if(size_t(position + maxyx.x) == backgroundLength)
    { /* No background left, so move the player to the right
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


void rules::movePlayerLeftWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->rightOfWindowInnerLeftMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x))
    { // We are to the righ of the inner left margin.
      gamePlayer->updatePosRel(input);
    }
  else if(position > 0)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position--;	// Move background.
    }
  else if(position == 0)
    { /* No background left, so move the player to the left
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


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
sprite::directions rules::handleGroundCollision(const int position,
						const int backgroundHeight)
{
  sprite::directions retDir {sprite::DIR_DOWN};
  // for(yx coord:
  // 	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact(position))
  //   {
  //     char rule {};
  //     if(getCoordRule(coord, coordRules, backgroundHeight, rule) &&
  // 	 (rule == boarderRuleChars::BOARDER_CHAR ||
  // 	  rule == boarderRuleChars::PLATFORM_CHAR))
  // 	{
  // 	  retDir = sprite::DIR_NONE;
  // 	  break;
  // 	}
  //   }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
sprite::directions rules::handleRightCollision(const int position,
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


sprite::directions rules::handleLeftCollision(const int position,
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
#include <sstream>

void rules::printRuleChars(const int position, const int maxY, const int maxX)
{
  for(int y {}; y < maxY; ++y)
    {
      for(int x {}; x < (coordRules.size() / maxY); ++x)
	{
	  char coordRule;
	  getCoordRule(y, x, coordRules, maxY, coordRule);
	  if(coordRule != ' ' && (x - position) < maxX)
	    {
	      mvprintw(y, (x - position), (std::string {coordRule}).c_str());
	    }
	}
    }
  refresh();
}
#endif


void rules::physics
(const player::directionChars input, int & position, const yx maxyx,
 const size_t backgroundLength,
 std::__1::chrono::steady_clock::time_point & secStartTime)
{
#ifdef DEBUG
  printRuleChars(position, maxyx.y, maxyx.x);
#endif
  
  movePlayer(player::convertDirectionCharsToDirections(input), position, maxyx,
	     backgroundLength);
  sleep(engineSleepTime);
  resetOldTime(secStartTime);
}
