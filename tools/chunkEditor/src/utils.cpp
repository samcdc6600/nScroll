#include <chrono>
#include <ostream>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include "include/utils.hpp"
#include "include/colorS.hpp"


yx abs(const yx a)
{
  return yx{abs(a.y), abs(a.x)};
}


std::ostream & operator<<(std::ostream &lhs, const yx rhs)
{
  return lhs<<"("<<rhs.y<<", "<<rhs.x<<")";
}


/* Used to update yIter and xIter for loop in get chunk functions. */
void updateVirtualNestedYXLoop(int & xIter, int & yIter, const yx chunkSize);
/* Used to detect if the chunk is too big in get chunk functions. If
   multiChunkFile is true and yIter is too large the function will return
   false. Other wise it will return true. If multiChunkfile is false and yIter
   is too large the function will exit with an error message. */
bool checkYOfVirtualNestedLoop
(int & xIter, int & yIter, const yx chunkSize, const bool multiChunkFile,
 const std::string & eMsgStart);
/* Attempts to write out chunkCoord to file (which should be open) at the
   current position. Prints an error message and exits if there is an error. */
void writeOutChunkCoordToFile
(const std::string & fileName, std::ofstream & file, const yx chunkCoord);


namespace borderRuleChars
{
  /* NOTE: when adding border rule characters you must also add them to the
     CHARS array and add the extern declarations to utils.hpp. */
  // The player cannot move through border chars.
  constexpr char BORDER_CHAR = 'b';
  /* Same as border chars (except the player can move through them if
     moving up.) */
  constexpr char PLATFORM_CHAR = 'p';
  const std::vector<char> CHARS {BORDER_CHAR, PLATFORM_CHAR};
};
/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places than this. */
constexpr int MAX_COORD_LEN{10};


void updateVirtualNestedYXLoop(int & xIter, int & yIter, const yx chunkSize)
{
  xIter++;
  if(xIter > chunkSize.x -1)
    {
      xIter = 0;
      yIter++;
    }
}


bool checkYOfVirtualNestedLoop
(int & xIter, int & yIter, const yx chunkSize,
 const bool multiChunkFile, const std::string & eMsgStart)
{
  bool ret {true};
  if(yIter > chunkSize.y - 1)
    {
      if(!multiChunkFile)
	{
	  exit(concat(eMsgStart, "Chunk is too large, equal to or over ",
		      yIter * chunkSize.x + xIter +1, " It should be ",
		      chunkSize.y * chunkSize.x, "."),
	       ERROR_MALFORMED_FILE);
	}
      else
	{
	  ret = false;
	}
    }

  return ret;
}


void writeOutChunkCoordToFile
(const std::string & fileName, std::ofstream & file, const yx chunkCoord)
{
  auto intOut = [& fileName, & file] (const int a)
  {
    char outputCoord [sizeof(int)];
    
    for(int iter = 0; (size_t)iter < sizeof(int); ++iter)
      {
	outputCoord[iter] = (a >> (iter * 8)) & 0xFF;
      }

    if(!file.write(outputCoord, sizeof(int)))
      {
	exit(concat("Error: trying to write chunkCoord (", a, ") to \"",
		    fileName, "\"."), ERROR_WRITING_TO_FILE);
      }
    
  };

  intOut(chunkCoord.y);
  intOut(chunkCoord.x);
}


bool readInChunkCoordFromFile
(yx & retVal, const std::string & fileName, std::fstream & file,
 const bool exitOnError)
{
  auto intIn = [& fileName, & file, exitOnError] (int & coord)
  {
    char inputCoord [sizeof(int)];

    if(!file.read(inputCoord, sizeof(int)))
      {
	if(exitOnError)
	  {
	    exit(concat("Error: trying to read int from \"", fileName, "\". "
			"The file size may be of note."),
		 ERROR_MALFORMED_FILE);
	  }
	else
	  {
	    return false;
	  }
      }

    coord = 0;

    for(int iter {}; (size_t)iter < sizeof(int); iter++)
      {
	coord |= (static_cast<int>(inputCoord[iter]) & 0xFF) << (iter * 8);
      }

    return true;
  };

  bool ret;

  if((ret = intIn(retVal.y)))
    {
      ret = intIn(retVal.x);
    }
  
  return ret;
}


void disableBlockingUserInput()
{
  noecho();			// Turn echoing off on the terminal
  // Dissable blocking while waiting for input (use non blocking sys call.)
  nodelay(stdscr, TRUE);
}


void enableBlockingUserInput()
{
  echo();
  nodelay(stdscr, FALSE);
}


void move(const yx pos)
{
  move(pos.y, pos.x);
}


void mvprintw(const yx pos, const std::string & str)
{
  mvprintw(pos.y, pos.x, str.c_str());
}


void progressivePrintMessage
(const std::string & msg, const yx viewPortSize, const int interCharacterSleep,
 const int afterMsgSleep)
{
  progressivePrintMessage(msg, viewPortSize, interCharacterSleep, afterMsgSleep,
			  true, true);
}


void progressivePrintMessage
(const std::string & msg, const yx viewPortSize, const int interCharacterSleep,
 const int afterMsgSleep, const bool clearScreen, const bool printProgressively)
{
  std::string msgNew {msg};
  setColorMode colorMode {};
  struct colorInfo
  {
    int linearPos;
    int color;
  };
  /* Stores any colors specified with the "\nX" construction, where X is a
     positive integer number in the range [1, effectiveGameColorPairsNo].
     And also stores the indexes in msgNew of the characters where the new color
     is to be applied. */
  std::vector<colorInfo> nonDefaultColors {};
  int nonDefaultColorsPos {};	// Used to index into nonDefaultColors.

  auto printProgressivelyFunc = [printProgressively, interCharacterSleep]()
  {
    if(printProgressively)
      {
	refresh();
	sleep(interCharacterSleep);
      }
  };
  
  auto printHorizontalBorder =
    [printProgressivelyFunc, viewPortSize, & colorMode]
    (const int msgLineLength, const int marginSingle,
     const int padding, const int marginTop, const int lineNo,
     const bool top)
    {
      // Print left corner.
      colorMode.setRandomColor();
      mvprintw(lineNo + marginTop, marginSingle,
	       top? progressivePrintMessageTopLeftCornerBorderChar.c_str():
	       progressivePrintMessageBottomLeftCornerBorderChar.c_str());
      printProgressivelyFunc();
      // Print centre.
      int lineChars {};
      for( ; lineChars < msgLineLength + (padding * 2); lineChars++)
	{
	  colorMode.setRandomColor();
	  mvprintw(lineNo + marginTop, lineChars + 1 + marginSingle,
		   progressivePrintMessageHorizBorderChar.c_str());
	  printProgressivelyFunc();
	}
      // Print right corner.
      colorMode.setRandomColor();
      mvprintw(lineNo+ marginTop, lineChars + 1 + marginSingle,
	       top? progressivePrintMessageTopRightCornerBorderChar.c_str():
	       progressivePrintMessageBottomRightCornerBorderChar.c_str());
      printProgressivelyFunc();
  };

  /* Returns the number of characters ('\n' characters will be added to the
     length returned.) We think there is still a small bug in this function.
     TODO: re-write this comment to include info about new functionality related
     to nonDefaultColors].*/
  auto countVirtualLength = [& msgNew, & nonDefaultColors]
    (const int msgLineLength)
  {
    int virtualLengthRet {};
    int currentLineLen {};

    for(int iter {}; (size_t)iter < msgNew.size(); iter++)
      {
	switch(msgNew[iter])
	  {
	  case '\n':
	    virtualLengthRet += msgLineLength;
	    currentLineLen = 0;
	    break;
	    /* '\r' signifies a color code. A base 10 integer number should
	       follow the \r. This will be used as the color of the next
	       character (after the number). The '\r' and the following number
	       will not be counted in the virtual length.  */
	  case '\r':
	    {
	      try
		{
		  /* Will throw an out_of_range exception if iter is out of
		     range. Stoi may throw an out_of_range exception or throw an
		     invalid_argument exception. */
		  std::size_t numLen {};
		  iter++;	// Move past '\r'.
		  nonDefaultColors.push_back
		    (colorInfo{iter -1, std::stoi(msgNew.substr(iter),
					       &numLen)});
		  
		  if(nonDefaultColors.back().color < 1 ||
		     nonDefaultColors.back().color >
		     colorParams::effectiveGameColorPairsNo)
		    {
		      exit(concat
			   ("Error: encountered color pair number (",
			    nonDefaultColors.back().color, ") after \"\\r\" "
			    "character that is outside of the range "
			    "[1, ", colorParams::effectiveGameColorPairsNo, "]"
			    " (in string passed to progressivePrintMessage().)"
			    ), ERROR_COLOR_CODE_RANGE);
		    }
		  /* Yes this is very inefficient (iter -1 because we wan't to
		     remove '\n' chars.) */
		  msgNew = compactStr(msgNew, iter -1, iter + numLen -1);
		  iter -= 2;	// Iter was moved past '\r' and we removed '\r'.
		}
	      catch(std::invalid_argument const & exception)
		{
		  exit(concat("Error (", exception.what(), "): encountered "
			      "invalid sequence after \"\\r\" character in "
			      "string passed to progressivePrintMessage() "
			      "(expected a positive integer.)"),
		       ERROR_MALFORMED_STRING);
		}
	      catch(std::out_of_range const & exception)
		{
		  exit(concat
		       ("Error (", exception.what(), "): encountered out of "
			"range color pair number after \"\\r\" character in "
			"string passed to progressivePrintMessage() (expected "
			"an integer in the range [1, ",
			colorParams::effectiveGameColorPairsNo, "].) Or "
			"Encountered end of string after \"\\r\" character in "
			"string passed to progressivePrintMessage()) "),
		       ERROR_MALFORMED_STRING);
		}
	    }
	    break;
	  default:
	    currentLineLen++;
	    if(currentLineLen > (msgLineLength -1))
	       {
		 virtualLengthRet += currentLineLen;
		 currentLineLen = 0;
	       }
	  }
      }

    if(currentLineLen != 0)
      {
	virtualLengthRet += msgLineLength;
      }

    return virtualLengthRet;
  };

  setColorMode color   	{};
  // This is for both sides and includes border characters.
  const int lRMargin   	{4};
  const int padding	{2};
  const int border	{1};
  const int msgLineLength
    			{170 - (lRMargin * 2) - ((padding + border) * 2)};
  const int msgLength  	{countVirtualLength(msgLineLength)};
  const int noOfLines	       	{msgLength / msgLineLength};
  const int marginTop
    {(viewPortSize.y / 2) - (padding + border) - (noOfLines / 2) +1};
  int charsPrinted     	{};
  int lines	       	{};

  /* This lambda func is used for both padding and partially blank lines /
     lines caused by new lines. When used for padding forNewLine should be set
     to false. */
  auto printBlanksForRemainderOfLine =
    [printProgressivelyFunc, lRMargin, border, msgLineLength, marginTop,
     & lines, & charsPrinted]
    (const int msgLinePrintLoopControlVar, int & lineCharIter,
     const bool forNewLine = true)
    {
      int virtualLineChars {lineCharIter};
      for( ; virtualLineChars < msgLineLength; virtualLineChars++)
	{
	  mvprintw
	    (yx{lines + marginTop,
		virtualLineChars + lRMargin + border + padding}, " ");
	  printProgressivelyFunc();
	}
      if(forNewLine)
	{
	  // Count new line char.
	  charsPrinted++;
	}
      // We need to break out of the outer for loop.
      lineCharIter = msgLinePrintLoopControlVar;
    };

  auto setCurrentColor =[& nonDefaultColors, & nonDefaultColorsPos, & color,
			 & charsPrinted](int & currentColor)
  {
    if(nonDefaultColors.size() > 0)
      {
	if(nonDefaultColorsPos < nonDefaultColors.size())
	  {
	    if(nonDefaultColors[nonDefaultColorsPos].linearPos == charsPrinted)
	      {
		currentColor =
		  nonDefaultColors[nonDefaultColorsPos].color;
		nonDefaultColorsPos++;
	      }
	  }
      }
    color.setColor(currentColor);
  };

  if(clearScreen)
    {
      clear();
    }
  
  // Print top border.
  printHorizontalBorder
    (msgLineLength, lRMargin, padding, marginTop, lines -1,
     true);

  int currentColor {helpColorPair};
  /* Was there a new line at the start of a new line (caused by
     wraparound.) */
  bool foundNLAtWraparound {false};
  // Print bulk of message (if it is one line or more.)
  for( ; lines < noOfLines + (padding * 2); lines++)
    {
      // Print left vertical border character.
      color.setRandomColor();
      mvprintw(lines + marginTop, lRMargin,
	       progressivePrintMessageVertBorderChar.c_str());
      printProgressivelyFunc();
      // Print left padding.
      color.setColor(helpColorPair);
      for(int iter {}; iter < padding; iter++)
	{
	  mvprintw(lines + marginTop, lRMargin + border + iter, " ");
	  printProgressivelyFunc();
	}

      int lineCharIter	{};
      
      lineCharIter = 0;
      if(lines < padding || lines > (noOfLines + padding -1))
	{
	  color.setColor(helpColorPair);
	  // Print top / bottom padding.
	  printBlanksForRemainderOfLine
	    (msgLineLength, lineCharIter, false);
	}
      else
	{
	  yx msgCharPos	{};
	  for( ; lineCharIter < msgLineLength; lineCharIter++)
	    {
	      msgCharPos = yx
		{lines + marginTop, lineCharIter + lRMargin + padding + border};
	      setCurrentColor(currentColor);

	      /* PrintBlanksForRemainderOfLine() may increment charsPrinted. So
		 we must test for that here. */
	      if(charsPrinted < msgNew.size())
		{
		  switch(msgNew[charsPrinted])
		    {
		    case '\n':
		      {
			if(lineCharIter != 0 || foundNLAtWraparound)
			  {
			    printBlanksForRemainderOfLine
			      (msgLineLength, lineCharIter);
			  }
			else
			  {
			    /* We don't want to print a double new line on wrap
			       around. Unfortunately this will increase the
			       bottom padding size by 1.*/
			    foundNLAtWraparound = true;
			    charsPrinted++;
			    lineCharIter--;
			  }
		      }
		      break;
		    default:
		      mvprintw
			(msgCharPos, concat("", msgNew[charsPrinted]).c_str());
		      foundNLAtWraparound = false;
		      charsPrinted++;
		    }
		}
	      else
		{
		  /* End of line (after msg has been printed) will be printed in
		     color of last character of msg. */
		  mvprintw
		    (msgCharPos, " ");
		}
	      printProgressivelyFunc();
	    }
	}

      color.setColor(helpColorPair);
      // Print right padding.
      for(int iter {}; iter < padding; iter++)
	{
	  mvprintw(lines + marginTop, msgLineLength + lRMargin +
		   padding + border + iter, " ");
	  printProgressivelyFunc();
	}
      // Print right vertical border character.
      color.setRandomColor();
      mvprintw
	(lines + marginTop, msgLineLength + lRMargin + border +
	 (padding * 2),
	 progressivePrintMessageVertBorderChar.c_str());
      printProgressivelyFunc();
    }

  // Print bottom border.
  printHorizontalBorder
    (msgLineLength, lRMargin, padding, marginTop, lines,
     false);
  printProgressivelyFunc();
  
  /* We need to move back to just after this point after printing the bottom
     border for messages prompting for user input. */
  color.setColor(helpColorPair);
  move(noOfLines + padding + border + marginTop - 2,
       msgLineLength - (msgLength - (int)msg.size())
  + lRMargin + padding + border);

  refresh();
  sleep(afterMsgSleep);
}


void printMessageNoWin
(const std::string & msg, const int interCharacterSleep,
 const int afterMsgSleep)
{
  for(char c: msg)
    {
      std::cout<<c<<std::flush;
      sleep(interCharacterSleep);
    }
  std::cout<<std::endl;
  sleep(afterMsgSleep);
}


void printBgChunk
(const backgroundChunkCharType bgChunk[][xWidth], const yx viewPortSize)
{
  for(int yIter {}; yIter < viewPortSize.y; yIter++)
    {
      for(int xIter {}; xIter < viewPortSize.x; xIter++)
	{
	  int ch;
	  bool foundACSCode;
	      
	  setColorFromChar(bgChunk[yIter][xIter]);
	  ch = getChar(bgChunk[yIter][xIter], foundACSCode);

	  if(foundACSCode)
	    {
	      printACS(yx{yIter, xIter}, ch);
	    }
	  else
	    {
	      mvprintw(yIter, xIter, concat("", (char)ch).c_str());
	    }
	}
    }
}


void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize)
{
  setColorMode colorMode {};
  
  for(int yIter {}; yIter < viewPortSize.y; ++yIter)
    {
      for(int xIter {}; xIter < viewPortSize.x; ++xIter)
	{
	  if(cRChunk[yIter][xIter] != emptyCharChar)
	    {
	      colorMode.setRandomColor();
	      mvprintw(yIter, xIter, concat("", cRChunk[yIter][xIter]).c_str());
	    }
	}
    }
}


bool getConfirmation
(const yx viewPortSize, const int msgColorPair, int & userInput,
 const int editSubMenuSleepTimeMs, const std::string & question)
{
  bool affirmative {false};
  
  setColorMode colorMode {};
  colorMode.setColor(msgColorPair);
  progressivePrintMessage
    (question, viewPortSize, 0, 0, false, false);
      
  nodelay(stdscr, FALSE);
  safeScreenExit(userInput, editSubMenuSleepTimeMs);
  nodelay(stdscr, TRUE);
  if(userInput == 'y')
    {
      affirmative = true;
    }

  return affirmative;
}


bool confirmQuit(const yx viewPortSize, int & input, const int quitChar)
{
  bool quit {false};
  if(input == quitChar)
    {
      quit = getConfirmation
	(viewPortSize, helpColorPair, input, subMenuSleepTimeMs,
	 "Do you really want to quit y/n? ");
    }

  return quit;
}


void safeScreenExit(int & userInput, const int editSubMenuSleepTimeMs)
{
  sleep(editSubMenuSleepTimeMs);
  userInput = getch();
}


void setColorFromChar(const int ch)
{
  setColorMode colorMode {};
  int colorCode = getColor(ch);
  colorMode.setColor(colorCode);
}


int getColor(const int ch)
{
  /* A character is encoded as it's basic value + it's color value -1 times
     maxCharNum. Since integer division rounds down (ch -1) / maxCharNum should
     give the color code. less 1. */
  int color {((ch -1) / maxCharNum) +1};
  if(color > colorParams::effectiveGameColorPairsNo)
    {
      exit(concat("Error (in getColor()): encountered colour (", color, ") "
		  "code that is out of range.\n"),
	   ERROR_COLOR_CODE_RANGE);
    }

  return color;
}


int getChar(const int ch, bool & aCS)
{
  // Remove colour information (first color starts at 1, so we remove 1.)
  int rawCh {ch - ((getColor(ch) -1) * maxCharNum)};
  aCS = false;

  if(rawCh > aSCIIChMaxEffective && rawCh <= maxCharNum)
    {
      // We have an ACS char.
      aCS = true;
    }
  else if(rawCh < 0 || rawCh > aSCIIChMaxEffective)
    {
      exit(concat("Error: encountered character (", (int)rawCh, ") that is less"
		  " than 0 or greater than ", maxCharNum,". after having color "
		  "info removed."),
	   ERROR_CHARACTER_RANGE);
    }

  /* To avoid having to store character color pair combinations using
     ints (as opposed to unsigned shorts) we remove lowerUnusedASCIIChNum from
     each raw character (one that has no color pair info). So we must
     add it back here! */
  return rawCh + lowerUnusedASCIIChNum;
}


void printACS(const yx position, const int aCSChar)
{
  move(position);
  printACS(aCSChar);
}


void printACS(const int aCSChar)
{
    switch(aCSChar)
    {
    case 128:		  
      addch(ACS_ULCORNER);
      break;
    case 129:
      addch(ACS_LLCORNER);
      break;
    case 130:
      addch(ACS_LRCORNER);
      break;
    case 131:
      addch(ACS_LTEE);
      break;	      
    case 132:
      addch(ACS_RTEE);
      break;
    case 133:
      addch(ACS_BTEE);
      break;
    case 134:
      addch(ACS_TTEE);
      break;
    case 135:
      addch(ACS_HLINE);
      break;
    case 136:
      addch(ACS_VLINE);
      break;
    case 137:
      addch(ACS_PLUS);
      break;
    case 138:
      addch(ACS_S1);
      break;
    case 139:
      addch(ACS_S3);
      break;
    case 140:
      addch(ACS_S7);
      break;
    case 141:
      addch(ACS_S9);
      break;
    case 142:
      addch(ACS_DIAMOND);
      break;
    case 143:
      addch(ACS_CKBOARD);
      break;
    case 144:
      addch(ACS_DEGREE);
      break;
    case 145:
      addch(ACS_PLMINUS);
      break;
    case 146:
      addch(ACS_BULLET);
      break;
    case 147:
      addch(ACS_LARROW);
      break;
    case 148:
      addch(ACS_RARROW);
      break;
    case 149:
      addch(ACS_DARROW);
      break;
    case 150:
      addch(ACS_UARROW);
      break;
    case 151:
      addch(ACS_BOARD);
      break;
    case 152:
      addch(ACS_LANTERN);
      break;
    case 153:
      addch(ACS_BLOCK);
      break;
    case 154:
      addch(ACS_LEQUAL);
      break;
    case 155:
      addch(ACS_GEQUAL);
      break;
    case 156:
      addch(ACS_PI);
      break;
    case 157:
      addch(ACS_NEQUAL);
      break;
    case 158:
      addch(ACS_STERLING);
      break;
    default:
      exit(concat("Error: encountered ACS character (", aCSChar, ") character "
		  "that is out of range"), ERROR_CHARACTER_RANGE);
      break;
    }
}


void sleep(const unsigned long long t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


bool isNum(const char c)
{
  return (c >= '0' && c <= '9') ? true: false;
}


void loadFileIntoString(const char name [], std::string & buff,
			const std::string & eMsg)
{
  std::ifstream file {};
  file.open(name);
  if(!file.is_open())
    {				// Couldn't open file.
      file.close();
      exit(concat("Error: opening file \"", name, "\" when ", eMsg, "."),
	   ERROR_OPENING_FILE);
    }
  char c;
  while(file.get(c))		// Copy file content's to buff.
    {
      buff.push_back(c);
    }
  file.close();

  if(buff.size() == 0)
    {
      exit(concat("Error: found file \"", name, "\" to be empty when ", eMsg,
		  "."), ERROR_OPENING_FILE);
    }
}


/* TODO: See if this needs to be added anywhere! Ask chatGPT about options when
   opening files (they effect if a file will be created or not.) */
void createFileIfNoneExists
(const std::string & fileName, const std::string & eMsg)
{
  if(!fileExists(fileName))
    {
      createFile(fileName, eMsg);
    }
}


///* TODO: add code to call this function and createFile when opening files to see
//   if they actually exist and create them if they do not (obviously this should
//   only be done for files that can be considered as valid if they empty.) */
bool fileExists(const std::string & fileName)
{
  return std::__fs::filesystem::exists(fileName);
}

void createFile(const std::string & fileName, const std::string & eMsg)
{
  std::ofstream file(fileName);
  if(file.is_open())
    {
      file.close();
    }
  else
    {
      exit(concat("Error: failed to create file \"", fileName, "\" when ", eMsg,
		  "."), ERROR_OPENING_FILE);
    }
}


int readSingleNum
(const std::string & buff, std::string::const_iterator & buffPos,
 const std::string & eMsg, const bool useIntegers)
{
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
      exit(concat("Error: expected ", useIntegers ? "integer": "natural",
		  " number when ", eMsg, ". Encountered \"", *buffPos, "\".\n"),
	   ERROR_RULES_LEV_HEADER);
    }

  int decPlacesIter {};
  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
      // There must be at least 1 digit after a negative sign.
      if(isNum(*buffPos))
	{
	  number<<*buffPos;
	  decPlacesIter++;
	  buffPos++;
	}
      else
	{
	  exit(concat("Error: expected integer number when ", eMsg,
		      ". Encountered \"", *buffPos, "\".\n"),
	       ERROR_RULES_LEV_HEADER);
	}
    }
  
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
      exit(concat
	   ("Error: number \"", number.str(), "\" too long (longer than \"",
	    MAX_COORD_LEN, "\") when ", eMsg,
	    ". Encountered \"", *buffPos, "\".\n").c_str(),
	   ERROR_RULES_LEV_HEADER);
    }

  int ret;
  number>>ret;
  return ret;
}


void readInBgChunkFile
(const std::string fileName, backgroundChunkCharType chunk[yHeight][xWidth],
 const yx chunkSize, yx & chunkCoord, bool & foundCoord)
{
  std::fstream chunkFile;

  chunkFile.open(fileName, std::ios::in);

  if(!chunkFile.is_open())
    {
      chunkFile.open(fileName, std::ios::out);
      if(!chunkFile.is_open())
	{
	  exit(concat("Error could not open or create file ", fileName),
	       ERROR_OPENING_FILE);
	}
      progressivePrintMessage(concat("", fileName, " not found. Creating new"
			  " file..."), chunkSize, printCharSpeed,
		   afterPrintSleep);
      chunkFile.close();
      foundCoord = false;
    }
  else
    {
      // File already exists, try reading in and decompressing contents.
      getBgChunk(chunkFile, chunk, chunkSize, chunkCoord, fileName);
      chunkFile.close();
      foundCoord = true;
    }
}


void readInCRChunkFile
(const std::string fileName, char chunk[][xWidth], const yx chunkSize,
 yx & chunkCoord, bool & foundCoord)
{
  std::fstream chunkFile;

  /* NOTE: here we would ideally like to use std::ios::binary, however when we
     do the next if statement is false! We have no idea why this is the case! */
  chunkFile.open(fileName, std::ios::in);

  if(!chunkFile.is_open())
    {
      chunkFile.open(fileName, std::ios::out);
      if(!chunkFile.is_open())
	{
	  exit(concat("Error could not open or create file ", fileName),
	       ERROR_OPENING_FILE);
	}
      progressivePrintMessage(concat("", fileName, " not found. Creating new"
			  " file..."), chunkSize, printCharSpeed,
		   afterPrintSleep);
      chunkFile.close();
      foundCoord = false;
    }
  else
    {
      // File already exists, try reading in and decompressing contents.
      getCRChunk(chunkFile, chunk, chunkSize, chunkCoord, fileName);
      chunkFile.close();
      foundCoord = true;
    }
}


// Note that multiChunkFile has a default value of false.
bool getBgChunk
(std::fstream & file, backgroundChunkCharType chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile)
{
  const std::string eMsgStart
    {concat("Error: trying to read in background chunk file \"", fileName,
	    "\". ")};  
  
  if(!readInChunkCoordFromFile(chunkCoord, fileName, file, !multiChunkFile))
    {
      return false;
    }
  
  // Read in chunk body.
  int yIter {}, xIter {};
  backgroundChunkCharType bgChar {};
  int chunkCharsFound {};
  while(file.read(reinterpret_cast<char*>(&bgChar),
		  sizeof(backgroundChunkCharType)))
    {      
      if(bgChar == bgRunLengthSequenceSignifier)
	{
	  backgroundChunkCharType runLength {};
	  if(!file.read(reinterpret_cast<char*>(&runLength),
			sizeof(backgroundChunkCharType)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier (or an IO error has occurred.)"),
		   ERROR_MALFORMED_FILE);
	    }
	  if(!file.read(reinterpret_cast<char*>(&bgChar),
			sizeof(backgroundChunkCharType)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier and length (or an IO error has occurred.)"
			  ), ERROR_MALFORMED_FILE);
	    }
	  for(int iter {}; iter < runLength; iter++)
	    {
	      if(!checkYOfVirtualNestedLoop
		 (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
		{
		  /* This is a multi chunk file and we've just read past the end
		     of a chunk so we need to back up. */
		  file.seekg(-sizeof(backgroundChunkCharType), std::ios::cur);
		  goto MEGA_BREAK;
		}
	      chunk[yIter][xIter] = bgChar;
	      chunkCharsFound++;
	      updateVirtualNestedYXLoop(xIter, yIter, chunkSize);
	    }
	}
      else
	{
	  if(!checkYOfVirtualNestedLoop
	     (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
	    {
	      /* This is a multi chunk file and we've just read past the end
		 of a chunk so we need to back up. */
	      file.seekg(-sizeof(backgroundChunkCharType), std::ios::cur);
	      goto MEGA_BREAK;
	    }
	  chunk[yIter][xIter] = bgChar;
	  chunkCharsFound++;
	  updateVirtualNestedYXLoop(xIter, yIter, chunkSize);
	}
    }

 MEGA_BREAK:  
  if(chunkCharsFound < chunkSize.y * chunkSize.x)
    {
      if(multiChunkFile)
	{
	  return false;
	}
      else
	{
	  exit(concat(eMsgStart, "Chunk is too small (", chunkCharsFound, ") "
		      "It should be ", chunkSize.y * chunkSize.x, "."),
	       ERROR_MALFORMED_FILE);
	}
    }

  return true;
}


// Note that multiChunkFile has a default value of false.
bool getCRChunk
(std::fstream & file, char chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile)
{
  const std::string eMsgStart
    {concat("Error: trying to read in coord rules chunk file "
	    "\"", fileName, "\". ")};
  
  if(!readInChunkCoordFromFile(chunkCoord, fileName, file, !multiChunkFile))
    {
      return false;
    }

  // Read in chunk body.
  int yIter {}, xIter {};
  char cRChar {};
  int chunkCharsFound {};
  while(file.read(&cRChar, sizeof(char)))
    {
      if(cRChar == cRRunLengthSequenceSignifier)
	{
	  char highByte {}, lowByte {};
	  if(!file.read(&highByte, sizeof(char)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier (or an IO error has occurred.)"),
		   ERROR_MALFORMED_FILE);
	    }
	  if(!file.read(&lowByte, sizeof(char)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier and first byte of length (or an IO error "
			  "has occurred.)"), ERROR_MALFORMED_FILE);
	    }
	  if(!file.read(&cRChar, sizeof(char)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length "
			  "sequence signifier and length bytes (or an IO error "
			  "has occurred.)"), ERROR_MALFORMED_FILE);
	    }
	  for(int iter {}; iter < ((static_cast<unsigned char>(highByte) << 8) |
				   static_cast<unsigned char>(lowByte)); iter++)
	    {
	      if(!checkYOfVirtualNestedLoop
		 (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
		{
		  /* This is a multi chunk file and we've just read past the end of
		     a chunk so we need to back up. */
		  file.seekg(-sizeof(char), std::ios::cur);
		  goto MEGA_BREAK;
		}
	      chunk[yIter][xIter] = cRChar;
	      chunkCharsFound++;
	      updateVirtualNestedYXLoop(xIter, yIter, chunkSize);
	    }
	}
      else
	{
	  if(!checkYOfVirtualNestedLoop
	     (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
	    {
	      /* This is a multi chunk file and we've just read past the end of
		 a chunk so we need to back up. */
	      file.seekg(-sizeof(char), std::ios::cur);
	      goto MEGA_BREAK;
	    }
	  chunk[yIter][xIter] = cRChar;
	  chunkCharsFound++;
	  updateVirtualNestedYXLoop(xIter, yIter, chunkSize);
	}
    }

 MEGA_BREAK:
  if(chunkCharsFound < chunkSize.y * chunkSize.x)
    {
      if(multiChunkFile)
	{
	  return false;
	}
      else
	{
	  exit(concat(eMsgStart, "Chunk is too small (", chunkCharsFound, ") "
		      "It should be ", chunkSize.y * chunkSize.x, "."),
	       ERROR_MALFORMED_FILE);
	}
    }

  return true;
}


void compressAndWriteOutBgChunk
(const std::string & fileName, std::ofstream & file, const yx chunkCoord,
 const backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize)
{
  auto addLookAhead = [](std::vector<backgroundChunkCharType> & lookAhead,
			 std::vector<backgroundChunkCharType> & compressedChunk)
  {
    if(lookAhead.size() > bgCompressionAdvantagePoint)
      {
	// We will gain an advantage from compressing lookAhead.
	compressedChunk.push_back(bgRunLengthSequenceSignifier);
	compressedChunk.push_back(lookAhead.size());
	compressedChunk.push_back(lookAhead[0]);
      }
    else
      {
	// No advantage from compression. Append lookAhead to compressedChunk.
	compressedChunk.insert
	  (compressedChunk.end(), lookAhead.begin(), lookAhead.end());
      }

    lookAhead.clear();
  };
  
  std::vector<backgroundChunkCharType> compressedChunk {}; 
  std::vector<backgroundChunkCharType> lookAhead {};

  writeOutChunkCoordToFile(fileName, file, chunkCoord);
  for(int yIter {}; yIter < chunkSize.y; ++yIter)
    {
      for(int xIter {}; xIter < chunkSize.x; ++xIter)
	{
	  if(!lookAhead.empty())
	    {
	      if(lookAhead.back() == bgChunk[yIter][xIter].ch)
		{
		  lookAhead.push_back(bgChunk[yIter][xIter].ch);
		}
	      else
		{
		  addLookAhead(lookAhead, compressedChunk);
		  lookAhead.push_back(bgChunk[yIter][xIter].ch);
		}
	    }
	  else
	    {
	      lookAhead.push_back(bgChunk[yIter][xIter].ch);
	    }
	}
    }
  if(lookAhead.size() > 0)
    {
      addLookAhead(lookAhead, compressedChunk);
    }

  for(backgroundChunkCharType ch: compressedChunk)
    {
      // Write out bits for ch.
      file.write(reinterpret_cast<const char *>(&ch),
		 sizeof(backgroundChunkCharType));
    }
}


void compressAndWriteOutCRChunk
(const std::string & fileName, std::ofstream & file, const yx chunkCoord,
 const char cRChunk[][xWidth], const yx chunkSize)
{
  auto addLookAhead = [](std::vector<char> & lookAhead,
			 std::vector<char> & compressedChunk)
  {
    if(lookAhead.size() > cRCompressionAdvantagePoint)
      {
	// We will gain an advantage from compressing lookAhead.
	compressedChunk.push_back(cRRunLengthSequenceSignifier);
	/* Note that we store the length of a run as two bytes here and as such
	   if bigger chunks are desired the number of bytes will of course have
	   to be increased. */
	char lowByte = static_cast<char>(lookAhead.size() & 0xFF);
	char highByte = static_cast<char>((lookAhead.size() >> 8) & 0xFF);
	compressedChunk.push_back(highByte);
	compressedChunk.push_back(lowByte);
	compressedChunk.push_back(lookAhead[0]);
      }
    else
      {
	// No advantage from compression. Append lookAhead to compressedChunk.
	compressedChunk.insert
	  (compressedChunk.end(), lookAhead.begin(), lookAhead.end());
      }

    lookAhead.clear();
  };

  std::vector<char> compressedChunk {};
  std::vector<char> lookAhead {};

  writeOutChunkCoordToFile(fileName, file, chunkCoord);
  for(int yIter {}; yIter < chunkSize.y; ++yIter)
    {
      for(int xIter {}; xIter < chunkSize.x; ++xIter)
	{
	  if(!lookAhead.empty())
	    {
	      if(lookAhead.back() == cRChunk[yIter][xIter])
		{
		  lookAhead.push_back(cRChunk[yIter][xIter]);
		}
	      else
		{
		  addLookAhead(lookAhead, compressedChunk);
		  lookAhead.push_back(cRChunk[yIter][xIter]);
		}
	    }
	  else
	    {
	      lookAhead.push_back(cRChunk[yIter][xIter]);
	    }
	}
    }
  if(lookAhead.size() > 0)
    {
      addLookAhead(lookAhead, compressedChunk);
    }
  
  for(char ch: compressedChunk)
    {
      // Write out bits for ch.
      file.write(reinterpret_cast<const char *>(&ch),
		 sizeof(char));
    }
}


// SkipSpace has a default value.
std::string skipSpaceUpTo(const std::string & buff,
			  std::string::const_iterator & buffPos,
			  std::vector<std::string> & targets,
			  const bool skipSpace)
{
  std::string targetFound {};
  std::string::const_iterator outerPeekPos {buffPos};
  
  if(outerPeekPos != buff.end())
    {
      sort(targets.begin(), targets.end()); // Sort alphabetically.
      stable_sort(targets.begin(), targets.end(),
		  [](const std::string & first, const std::string & second)
		  {
		    return first.size() > second.size();
		  });			// Stable sort by length in desc order.

      // If the first char isn't white space.
      if(*outerPeekPos != ' ' &&
	 *outerPeekPos != '\n' &&
	 *outerPeekPos != '\r' &&
	 *outerPeekPos != '\t')
	{
	  // Check for targets at buffPos.
	  targetFound = findTargetInBuff(outerPeekPos, targets);
	  for(int iter {}; size_t(iter) < targetFound.size(); ++iter)
	    {
	      outerPeekPos++;
	    }
	}
      else
	{
	  // skip spaces.
	  while(outerPeekPos != buff.end() && skipSpace &&
		(*outerPeekPos == ' ' ||
		 *outerPeekPos == '\n' ||
		 *outerPeekPos == '\r' ||
		 *outerPeekPos == '\t'))
	    {
	      outerPeekPos++;
	    }

	  // Check for targets again.
	  if(outerPeekPos != buff.end())
	    {
	      targetFound = findTargetInBuff(outerPeekPos, targets);
	      for(int iter {}; size_t(iter) < targetFound.size(); ++iter)
		{
		  outerPeekPos++;
		}
	    }
	}
    }

  buffPos = outerPeekPos;
  return targetFound;
}


static std::string findTargetInBuff
(const std::string::const_iterator & outerPeekPos,
 const std::vector<std::string> & targets)
{
  std::string targetFound {};
  std::string::const_iterator peekPos {};
  // Iterate over targets.
  for(int targetsIter {}; size_t(targetsIter) < targets.size();
      ++targetsIter)
    {
      peekPos = outerPeekPos;
      bool found {true};
      
      // Iterate over characters in target.
      for(int targetIter {}; size_t(targetIter) < targets[targetsIter].size();
	  ++targetIter)
	{
	  if(*peekPos != targets[targetsIter][targetIter])
	    {
	      found = false;
	      break;
	    }
	  peekPos++;
	}
      if(found)
	{
	  targetFound = targets[targetsIter];
	  goto RETURN;
	}
    }

 RETURN:
  return targetFound;
}


bool checkForPostfix(const char str [], const char postfix [])
{
  const char * postfixPos {strstr(str, postfix)};

  if(postfixPos == nullptr ||
     (size_t)((postfixPos + strlen(postfix)) - str) != strlen(str))
    {
      return false;
    }

  return true;
}


void getMinMaxYAndX(const std::vector<yx> & coords, yx & minMaxY, yx & minMaxX)
{
  if(coords.size() < 1)
    {
      exit("Error: getMinMaxYAndX() called with coords.size() < 1, This is a "
	   "logic error. The size of coords should always be checked to make "
	   "sure it's greater than 0 before calling getMinMaxYAndX()!",
	   ERROR_BAD_LOGIC);
    }

  minMaxY.y = coords[0].y;
  minMaxY.x = coords[0].y;

  minMaxX.y = coords[0].x;
  minMaxX.x = coords[0].x;  
  
  for(yx coord: coords)
    {
      // Find min for y...
      minMaxY.y = coord.y < minMaxY.y? coord.y: minMaxY.y;
      // Find max for y...
      minMaxY.x = coord.y > minMaxY.x? coord.y: minMaxY.x;

      // Find min for x...
      minMaxX.y = coord.x < minMaxX.y? coord.x: minMaxX.y;
      // Find max for x...
      minMaxX.x = coord.x > minMaxX.x? coord.x: minMaxX.x;
    }
}


std::string compactStr
(const std::string & str, const size_t startIndex,
 const size_t endIndex)
{
  return str.substr(0, startIndex) + str.substr(endIndex + 1);
}


/* Calls endwin() then print's e to std::cerr and finally call's exit() with
   status */
void exit(const std::string & e, const int status)
{
  endwin();
  std::cerr<<e<<'\n';
  exit(status);
}
