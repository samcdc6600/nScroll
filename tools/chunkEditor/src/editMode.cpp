#include <filesystem>
#include <curses.h>
#include "include/editMode.hpp"


namespace editingSettings
{    
  setColorMode colorMode {};
}


struct editingState
{
  // Are we editing the cRChunk or the bgChunk?
  bool cRChunkToggle;
  // // Show character selection screen?
  // bool characterSelection;
  // // Show help screen.
  // bool help;
  // Used for blinking the cursor.
  bool cursorOn;
  std::chrono::steady_clock::time_point cursorVisibilityChangeTimeLast;
  int input;
  yx cursorPos;
  // The bg char that will be saved to bgChunk if we press space.

private:
  // Buffer to remember x of the last bg chars.
  static constexpr int bgCharsBufferSize {16};
  int bgCharsRingBuffer [bgCharsBufferSize] {};
  int currentBgCharIndex {};
  // The user can switch between the current and last bg chars.
  // int lastBgChar;

public:
  char currentCRChar;

  editingState
  (const bool cRChunkToggle, const bool cursorOn,
   const std::chrono::steady_clock::time_point cursorVisibilityChangeTimeLast,
   const int input, const yx cursorPos, const int currentBgChar,
   const int currentCRChar)
  {
    this->cRChunkToggle = cRChunkToggle;
    this->cursorOn = cursorOn;
    this->cursorVisibilityChangeTimeLast = cursorVisibilityChangeTimeLast;
    this->input = input;
    this->cursorPos = cursorPos;
    for(int iter {}; iter < bgCharsBufferSize; ++iter)
      {
	bgCharsRingBuffer[iter] = currentBgChar;
      }
    this->currentCRChar = currentCRChar;
  }
  
  int getCurrentBgChar()
  {
    return bgCharsRingBuffer[currentBgCharIndex];
  }

  void setCurrentBgChar(const int newChar)
  {
    // Cycle back to zero if index goes out of range.
    currentBgCharIndex < bgCharsBufferSize -1 ? currentBgCharIndex++:
      currentBgCharIndex = 0;
    bgCharsRingBuffer[currentBgCharIndex] = newChar;
  }

  void recallNextBgChar()
  {
    currentBgCharIndex < bgCharsBufferSize -1 ? currentBgCharIndex++:
      currentBgCharIndex = 0;
  }

  void recallLastBgChar()
  {
    currentBgCharIndex == 0 ?
      currentBgCharIndex = bgCharsBufferSize - 1:
      currentBgCharIndex--;
  }
};


/* Calls void readInBgChunkFile
   (const std::string fileName, int chunk[][xWidth], const yx chunkSize,
   yx & chunkCoord, bool & foundCoord); with a local array as the argument and
   then copies the values in that array to the argument chunks elements .ch */
void readInBgChunkFile
(const std::string fileName, backgroundChunkCharInfo chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, bool & foundCoord);
/* This is the main routine that handles chunk editing. */
void editModeProper(const yx chunkCoord, backgroundChunkCharInfo bgChunk[][xWidth],
                    char cRChunk[][xWidth], const yx chunkSize);
void actOnInput
(backgroundChunkCharInfo bgChunk[][xWidth], char cRChunk[][xWidth], const yx chunkSize,
 editingState & edState);
/* Updates the cursors position in edState if edState.input is one of cursorUp,
    cursorDown, cursorLeft or cursorRight and if the cursor will not go outside
    of the view port. */
bool updateCursorPos
(const yx chunkSize, editingState & edState);
void printCharacterSelection();
/* Displays available colors on the screen and waits for the user to select one
   using the cursor navigation keys (see editingSettings.) This color will be
   the fg color, then displayes available colors again. This color will be the
   bg color. This is done by calling getColorPairFromUser(). Then displayes
   available characters. This information will combined to create a valid
   character with color information, this will then be returned. The function
   will display suitable instructions at each set. */
void getBgCharFromUser(const yx chunkSize, editingState &edState);
/* Displayes availbable colors on the screen for selection by the user (as
   described above.) Where userQuit is set to true and the return value is
   invalid if the user hit the editChars::quit. Otherwise userQuit is false and
   the return value is valid. And where printHelpMsg() is a lambda function that
   prints a help message. */
int getColorPairFromUser(const yx chunkSize, editingState & edState,
			 bool & userQuit, std::function<void()> printHelpMsg);
/* Should be called when exiting an interactive "screen" or "menu". Sleeps for
   editingSettings::editSubMenuSleepTimeMs and then sets edState.input to the
   value returned from getch(). This give the user some time to take their
   finger off of editChars::quit. */
void safeScreenExit(editingState & edState);
/* Once entered this function will not exit untill one of editChars::quit or
   editChars::toggleHelpMsg is pressed.  */
void printEditHelp(const yx viewPortSize, editingState & edState);
void printBgChunk
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx viewPortSize);
void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize);
void printRandomColorAtCursoPos(editingState & edState);
void printCursorForCREditMode(const yx cursorPos, const char currentCRChar);
void printCursorForBgEditMode
(const yx cursorPos, const int currentBgChar);
void printACS(const yx position, const int aCSChar);
void printACS(const int aCSChar);
void setColorFromChar(const int ch);
void setRandomColor();
// bool charInColorCharRange(const int ch);
int getColor(const int ch);
/* Removes color info from ch. If ch is an aCS character sets aCS to true,
   otherwise sets aCS to false. Prints an error and exits if ch is out of
   range. */
int getChar(const int ch, bool & aCS);
std::chrono::steady_clock::time_point setCursorVisibility
(bool & cursorOn, const std::chrono::steady_clock::time_point tLast);


/* Fills chunk with filler. */
template <typename T1, typename T2, typename assignmentFunc>
void fillChunk(T1 chunk[][xWidth], const yx viewPortSize, const T2 filler,
	       assignmentFunc assign)
{
  
  for(int yIter {}; yIter < viewPortSize.y; ++yIter)
    {
      for(int xIter {}; xIter < viewPortSize.x; ++xIter)
	{
	  /* Chunk's elements may be structures or basic types so we use a
	     function that's passed in as the argument "assign" to do the
	     assignment. */
	  assign(chunk[yIter][xIter], filler);
	}
    }
}


void readInBgChunkFile
(const std::string fileName, backgroundChunkCharInfo chunk[][xWidth],
 const yx chunkSize, yx & chunkCoord, bool & foundCoord)
{
  int rawChunk[yHeight][xWidth];
  readInBgChunkFile(fileName, rawChunk, chunkSize, chunkCoord, foundCoord);

  for(int yIter {}; yIter < chunkSize.y; yIter++)
    {
      for(int xIter {}; xIter < chunkSize.x; xIter++)
	{
	  chunk[yIter][xIter].ch = rawChunk[yIter][xIter];
	  chunk[yIter][xIter].set = false;
	}
    }
}


void editMode
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 backgroundChunkCharInfo bgChunk[][xWidth], char cRChunk[][xWidth],
 const yx chunkSize)
{
  bool foundBgChunkCoord {false}, foundCRChunkCoord {false};
  yx bgChunkCoord {}, cRChunkCoord {};

  // Read files into bgChunk and cRChunk (if they exist.)
  readInBgChunkFile(bgChunkFileName, bgChunk, chunkSize, bgChunkCoord,
                    foundBgChunkCoord);
  readInCRChunkFile(cRChunkFileName, cRChunk, chunkSize, cRChunkCoord,
		    foundCRChunkCoord);
  if(!foundBgChunkCoord)
    {
      // Chunk coord not found i.e. file not found. So initialise chunk.
      fillChunk(bgChunk, chunkSize, (int)' ',
		[](auto & element, const int filler)
		{
		  element.ch = filler;
		});
    }
  if(!foundCRChunkCoord)
    {
      // Chunk coord not found i.e. file not found. So initialise chunk.
      fillChunk(cRChunk, chunkSize, editingSettings::rulesChars::nullRule,
		[](auto & element, const char filler)
		{
		  element = filler;
		});
    }
  if(!foundBgChunkCoord && !foundCRChunkCoord)
    {
      // No void files were found, so we must ask the user for coordinates.
      progressivePrintMessage
	(concat("Both ", bgChunkFileName, " and ", cRChunkFileName, " found to "
		"be empty. Please enter the y chunk coordinate followed by a "
		"space and then followed by the x chunk coordinate: "),
	 chunkSize, printCharSpeed, 0);

      enableBlockingUserInput();
      while(scanw("%d %d", &bgChunkCoord.y, &bgChunkCoord.x) != 2)
	{
	  progressivePrintMessage
	    (concat("Error: you must enter a chunk coordinate in the form of a "
		    "y and x coordinate seperated by a space. Please enter "
		    "coordinates: "), chunkSize, printCharSpeed, 0);
	}
      disableBlockingUserInput();
      editModeProper(bgChunkCoord, bgChunk, cRChunk, chunkSize);
    }
  else
    {
      // Both or just one coord could be valid, so we choose one that is.
      editModeProper(foundBgChunkCoord? bgChunkCoord: cRChunkCoord,
		     bgChunk, cRChunk, chunkSize);
    }
}


void editModeProper(const yx chunkCoord, backgroundChunkCharInfo bgChunk[][xWidth],
		    char cRChunk[][xWidth], const yx chunkSize)
{
  editingState edState
    {
      false,
      true,
      std::chrono::steady_clock::now(),
      ERR, 	        // This is returned by getch() when there is no input.
      yx{chunkSize.y / 2, chunkSize.x / 2},
      ' ',
      editingSettings::rulesChars::boarder
    };
  
  while(edState.input != editingSettings::editChars::quit)
    {      
      edState.input = getch();

      actOnInput(bgChunk, cRChunk, chunkSize, edState);

      // We always print the background chunk.
      printBgChunk(bgChunk, chunkSize);
      if(edState.cRChunkToggle)
	{
	  // We want to print coord rules over the bg chunk.
	  printCRChunk(cRChunk, chunkSize);
	}

	// The cursor should always be visible, so print last.
      if(edState.cRChunkToggle)
	{
	  printCursorForCREditMode(edState.cursorPos, edState.currentCRChar);
	}
      else
	{
	  printCursorForBgEditMode
	    (edState.cursorPos, edState.getCurrentBgChar());
	}

      edState.cursorVisibilityChangeTimeLast =
	setCursorVisibility(edState.cursorOn,
			    edState.cursorVisibilityChangeTimeLast);
      
      sleep(editingSettings::loopSleepTimeMs);
    }
}


void actOnInput
(backgroundChunkCharInfo bgChunk[][xWidth], char cRChunk[][xWidth], const yx chunkSize,
 editingState & edState)
{
  using namespace editingSettings::editChars;

  if(!updateCursorPos(chunkSize, edState))
    {
      switch(edState.input)
	{
	case performActionAtPos:
	  if(edState.cRChunkToggle)
	    {
	      cRChunk[edState.cursorPos.y][edState.cursorPos.x] = edState.currentCRChar;
	    }
	  else
	    {
	      bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch =
		edState.getCurrentBgChar();
	      bgChunk[edState.cursorPos.y][edState.cursorPos.x].set = true;
	    }
	  break;
	case toggleBetweenCRandBg:
	  edState.cRChunkToggle = !edState.cRChunkToggle;
	  break;
	case cREraseCRChar:
	  if(edState.cRChunkToggle)
	    {
	      edState.currentCRChar = editingSettings::rulesChars::nullRule;
	    }
	  break;
	case cRSetCRCharToBorder:
	  if(edState.cRChunkToggle)
	    {
	      edState.currentCRChar = editingSettings::rulesChars::boarder;
	    }
	  break;
	case cRSetCRCharToPlatform:
	  if(edState.cRChunkToggle)
	    {
	      edState.currentCRChar = editingSettings::rulesChars::platform;
	    }
	  break;
	case bGToggleCharacterSelection:
	  if(!edState.cRChunkToggle)
	    {
	      getBgCharFromUser(chunkSize, edState);
	    }
	  break;
	case bgNextCurrentChar:
	  if(!edState.cRChunkToggle)
	    {
	      edState.recallNextBgChar();
	    }
	  break;
	case bgLastCurrentChar:
	  if(!edState.cRChunkToggle)
	    {
	      edState.recallLastBgChar();
	    }
	  break;
	case bgGetCharAtPos:
	  if(!edState.cRChunkToggle)
	    {
	      edState.setCurrentBgChar
		(bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch);
	    }
	  break;
	case toggleHelpMsg:
	  printEditHelp(chunkSize, edState);
	}
    }
}


bool updateCursorPos
(const yx chunkSize, editingState & edState)
{
  using namespace editingSettings::editChars;

  bool ret {false};
  
  switch(edState.input)
    {
    case cursorUp:
      if(edState.cursorPos.y > 0)
	{
	  edState.cursorPos.y--;
	  move(edState.cursorPos);
	  ret = true;
	}
      break;
    case cursorDown:
      if(edState.cursorPos.y < chunkSize.y -1)
	{
	  edState.cursorPos.y++;
	  move(edState.cursorPos);
	  ret = true;
	}
      break;
    case cursorLeft:
      if(edState.cursorPos.x > 0)
	{
	  edState.cursorPos.x--;
	  move(edState.cursorPos);
	  ret = true;
	}
      break;
    case cursorRight:
      if(edState.cursorPos.x < chunkSize.x -1)
	{
	  edState.cursorPos.x++;
	  move(edState.cursorPos);
	  ret = true;
	}
      break;
    }

  return ret;
}


void getBgCharFromUser(const yx chunkSize, editingState & edState)
{
  using namespace editingSettings::editChars;

  const std::vector<long> aCSChars
    {ACS_ULCORNER,	ACS_LLCORNER,	ACS_LRCORNER,	ACS_LTEE,
     ACS_RTEE,		ACS_BTEE,	ACS_TTEE,	ACS_HLINE,
     ACS_VLINE,		ACS_PLUS,      	ACS_S1,		ACS_S3,
     ACS_S7,       	ACS_S9,		ACS_DIAMOND,	ACS_CKBOARD,
     ACS_DEGREE,	ACS_PLMINUS,	ACS_BULLET,	ACS_LARROW,
     ACS_RARROW,	ACS_DARROW,   	ACS_UARROW,	ACS_BOARD,
     ACS_LANTERN,  	ACS_BLOCK,	ACS_LEQUAL,	ACS_GEQUAL,
     ACS_PI,		ACS_NEQUAL,	ACS_STERLING};
    
  auto printAllCharacters = [chunkSize, aCSChars](int printACSAtYPos)
  {
    move(0, 0);
    editingSettings::colorMode.setColor(editingSettings::helpColor);
    /* All ASCII characters that are not control characters are
       contiguous. These characters start with ' ' (32) and end with '~'
       (126). */
    for(char charIter {' '}; charIter <= '~'; charIter++)
      {
	printw(concat("", charIter).c_str());
      }
    /* ASCII chars should only take up one line (unless the view port size is
       changed. We also have to know on which line the ACS characters are
       printed as we cannot tell just from mvwinch() if a character is an ASCII
       character or an ACS character. */
    move(printACSAtYPos, 0);
    for(auto aCSChar: aCSChars)
      {
	addch(aCSChar);
      }
    refresh();
  };
  
  auto printModeSpecificHelpMsg = [chunkSize]()
  {
    editingSettings::colorMode.setColor(editingSettings::helpColor);
    progressivePrintMessage
      (concat("\tPlease select a background color for the character.\t"),
       chunkSize, 0, 0, false, false);
  };

  // Used to reset the cursor pos at the end of the fuc.
  const yx initialCursorPos {edState.cursorPos};
  bool userQuit {false};
  const int colorPair
    {getColorPairFromUser
     (chunkSize, edState, userQuit, printModeSpecificHelpMsg)};
  bool foundChar {false};


  if(!userQuit)
    {
      /* We print the characters at the top of the screen so the cursor should
	 be at the top of the screen too. */
      edState.cursorPos = {0, 0};
      
      while(!foundChar && (char)edState.input != quit)
	{
	  /* We cannot tell mvwinch() if a character is an ASCII
	     character or an ACS character. So we print all ACS characters on a
	     different line. */
	  const int printACSAtY {1};
      
	  printAllCharacters(printACSAtY);
	  printModeSpecificHelpMsg();
	  /* Get character at pos here before changing it with
	     printRandomColorAtCursoPos() */
	  const chtype charAtPos 
	    {mvwinch(stdscr, edState.cursorPos.y, edState.cursorPos.x)};
	  printRandomColorAtCursoPos(edState);
	  edState.input = getch();
      

	  if(!updateCursorPos(chunkSize, edState))
	    {
	      switch(edState.input)
		{
		case toggleHelpMsg:
		  printEditHelp(chunkSize, edState);
		  break;
		case performActionAtPos:
		  const int charColorOffset {maxCharNum * (colorPair -1)};
		  /* As far as we know there is no direct way to tell if a
		     character on in the view port is an ASCII character or an ACS
		     character. We use the cursor position to determine which
		     character set we are dealing with. */
		  if(edState.cursorPos.y == printACSAtY)
		    {
		      bool foundACS {false};

		      for(int aCSIter {}; aCSIter < aCSChars.size(); ++aCSIter)
			{
			  if(acs_map[charAtPos & A_CHARTEXT] ==
			     aCSChars[aCSIter])
			    {
			      // edState.setastBgChar = edState.getCurrentBgChar();
			      edState.setCurrentBgChar
				(charColorOffset + ASCII_CH_MAX + aCSIter +1);
			      foundACS = true;
			    }
			}
		      if(!foundACS)
			{
			  exit(concat("Error (in getBgCharFromUser()) could not"
				      "find corresponding ACS character in "
				      "acs_map for ", charAtPos & A_CHARTEXT),
			       ERROR_CHARACTER_RANGE);
			}
		    }
		  else
		    {
		      // edState.lastBgChar = edState.getCurrentBgChar();
		      edState.setCurrentBgChar
			(charColorOffset + (charAtPos & A_CHARTEXT));
		    }
		  foundChar = true;
		  break;
		}
	    }

	  edState.cursorVisibilityChangeTimeLast =
	    setCursorVisibility(edState.cursorOn,
				edState.cursorVisibilityChangeTimeLast);
	  sleep(editingSettings::loopSleepTimeMs);
	}
    }
  
  edState.cursorPos = initialCursorPos;
  safeScreenExit(edState);
}


int getColorPairFromUser(const yx chunkSize, editingState & edState,
			 bool & userQuit, std::function<void()> printHelpMsg)
{
  using namespace editingSettings::editChars;

  auto printAllColors = []()
  {
    move(0, 0);
    for(int colorIter {1}; colorIter <= colorParams::gameColorPairsNo; colorIter++)
      {
	editingSettings::colorMode.setColor(colorIter);
	printw("     ");
      }
    refresh();
  };

  auto findColorPairWithColorComponents =
    [](int fgColorIndex, int bgColorIndex, int & pairFound) -> bool
    {
      bool ret {false};
      for(int pairNumberIter = 1; pairNumberIter <= colorParams::gameColorPairsNo;
	  ++pairNumberIter)
	{
	  short foundFgColorIndex, foundBgColorIndex;
	  pair_content(pairNumberIter, &foundFgColorIndex, &foundBgColorIndex);
	  if (foundFgColorIndex == fgColorIndex &&
	      foundBgColorIndex == bgColorIndex)
	    {
	      pairFound = pairNumberIter;
	      ret = true;
	      break;
	    }
	}

      return ret;
    };

  bool gotBgColor {false}, gotFgColor {false};
  short bgColorIndex {}, fgColorIndex {};

  userQuit = false;

  /* We print the colors at the top of the screen so the cursor should be at the
     top of the screen too. */
  edState.cursorPos = {0, 0};
  
  while((!gotBgColor || !gotFgColor) && (char)edState.input != quit)
    {
      printAllColors();
      printHelpMsg();
      /* Get color at pos here before changing it with
	 printRandomColorAtCursoPos() */
      int pairNumberAtCursorPos
	{PAIR_NUMBER(mvwinch(stdscr, edState.cursorPos.y,
			     edState.cursorPos.x))};
      printRandomColorAtCursoPos(edState);
      
      edState.input = getch();

      if(!updateCursorPos(chunkSize, edState))
	{
	  switch(edState.input)
	    {
	    case toggleHelpMsg:
	      printEditHelp(chunkSize, edState);
	      break;
	    case performActionAtPos:
	      {
		short dummyVal {}; // pair_content requires three arguments...
		// Get color at cursor pos.
		if(!gotFgColor)
		  {
		    /* Since we display two sets of colored spaces we always
		       wen't to get the bg color (as that is the visible
		       color.) */
		    pair_content
		      (pairNumberAtCursorPos, &dummyVal, &fgColorIndex);
		    gotFgColor = true;
		  }
		else
		  {
		    pair_content
		      (pairNumberAtCursorPos, &dummyVal, &bgColorIndex);
		    gotBgColor = true;
		  }
	      }
	      break;
	    }
	}
      edState.cursorVisibilityChangeTimeLast =
	setCursorVisibility(edState.cursorOn,
			    edState.cursorVisibilityChangeTimeLast);
      sleep(editingSettings::loopSleepTimeMs);
    }

  if(fgColorIndex == bgColorIndex)
    {
      /* Color pairs with the same fg and bg color are not supported as these
	 color pairs can be replicated with a ' ' character that has the desired
	 bg color. */
      fgColorIndex = editingSettings::validColorNumber;
    }

  int foundColorPairNumber {};
  
  if(!findColorPairWithColorComponents
     (fgColorIndex, bgColorIndex, foundColorPairNumber))
    {
      /* We didn't find the color pair. It may be an inverted color pair. In
	 which case we check for it's opposite and if found we add the total
	 number of color pairs colorParams::gameColorPairsNo to the found color
	 pair to get the "virtual" color pair (this color will be printed with
	 A_REVERSE set. */
      if(findColorPairWithColorComponents
	 (bgColorIndex, fgColorIndex, foundColorPairNumber))
	{
	  foundColorPairNumber += colorParams::gameColorPairsNo;
	}
      else if(edState.input != quit)
	{
	  exit(concat
	       ("Error: selected color pair with foreground color index (",
		fgColorIndex, ") and background color index (",
		bgColorIndex, ") does not exist. This is a logic error."),
	       ERROR_COLOR_CODE_RANGE);
	}
      else
	{
	  /* The user has pressed the quit button (foundColorPairNumber isn't
	     valid. */
	  userQuit = true;
	}
    }

  return foundColorPairNumber;
}


void safeScreenExit(editingState & edState)
{
  sleep(editingSettings::editSubMenuSleepTimeMs);
  edState.input = getch();
}


void printEditHelp(const yx viewPortSize, editingState & edState)
{
  using namespace editingSettings::editChars;
  
  editingSettings::colorMode.setColor(editingSettings::helpColor);
  progressivePrintMessage
    (concat
     ("\t~H~E~L~P~!~\t\t\t\t\t\t",
      cursorUp, ": to move the cursor up.\t\t\t",
      cursorDown, ": to move the cursor down.\t\t\t",
     cursorLeft, ": to move the cursor left.\t\t\t",
      cursorRight, ": to move the cursor right.\t\t\t"),
     viewPortSize, 0, 200, false, false);

  edState.input = getch();  
  while(edState.input != quit &&
	edState.input != toggleHelpMsg)
    {
      edState.input = getch();
      sleep(editingSettings::loopSleepTimeMs);
    }

  safeScreenExit(edState);
}


void printBgChunk
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx viewPortSize)
{
  for(int yIter {}; yIter < viewPortSize.y; yIter++)
    {
      for(int xIter {}; xIter < viewPortSize.x; xIter++)
	{
	  if(bgChunk[yIter][xIter].set == true)
	    {
	      int ch;
	      bool foundACSCode;
	      
	      setColorFromChar(bgChunk[yIter][xIter].ch);
	      ch = getChar(bgChunk[yIter][xIter].ch, foundACSCode);

	      if(foundACSCode)
		{
		  printACS(yx{yIter, xIter}, ch);
		}
	      else
		{
		  mvprintw(yIter, xIter, concat("", (char)ch).c_str());
		}
	    }
	  else
	    {
	      editingSettings::colorMode.setColor
		(editingSettings::noCharColorPair);
	      mvprintw(yIter, xIter, " ");
	    }
	}
    }
}


void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize)
{ 
  for(int yIter {}; yIter < viewPortSize.y; ++yIter)
    {
      for(int xIter {}; xIter < viewPortSize.x; ++xIter)
	{
	  if(cRChunk[yIter][xIter] != ' ')
	    {
	      setRandomColor();
	      mvprintw(yIter, xIter, concat("", cRChunk[yIter][xIter]).c_str());
	    }
	}
    }
}


void printRandomColorAtCursoPos(editingState & edState)
{
  setRandomColor();
  mvprintw(edState.cursorPos, concat(" ").c_str());
  move(edState.cursorPos);
};


void printCursorForCREditMode
(const yx cursorPos, const char currentCRChar)
{
  // TMP (We want to set the cursor colour to the opposite of the bg
  // colour. No matter which chunk editing mode we are in.)
  editingSettings::colorMode.setColor(editingSettings::noCharColorPair);
  mvprintw(cursorPos, concat("", currentCRChar).c_str());
  move(cursorPos);
}


void printCursorForBgEditMode
(const yx cursorPos, const int currentBgChar)
{
  bool aCSChar {false};
  /* Set cursor colour to opposite of bg color (maybe, we aren't doing
     it properly here.) at cursorPos. */
  // setColorFromChar
  //   ((bgChunk[cursorPos.y][cursorPos.x].ch +
  // 	       (colorParams::effectiveGameColorPairsNo / 2)) %
  // 	      colorParams::effectiveGameColorPairsNo);
  // const int bgChar {getChar(currentBgChar, aCSChar)};

  const int bgChar {getChar(currentBgChar, aCSChar)};
	  
  if(aCSChar)
    {
      printACS(cursorPos, bgChar);
    }
  else
    {
      mvprintw(cursorPos, concat("", (char)bgChar));
    }
  move(cursorPos);
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


void setColorFromChar(const int ch)
{
  int colorCode = getColor(ch);
  editingSettings::colorMode.setColor(colorCode);
}


void setRandomColor()
{
  editingSettings::colorMode.setColor
    (abs(rand() % colorParams::gameColorPairsNo) +1);
  if(rand() % 2)
    {
      attron(A_REVERSE);
    }
  else
    {
      attroff(A_REVERSE);
    }
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

  if(rawCh > ASCII_CH_MAX && rawCh <= maxCharNum)
    {
      // We have an ACS char.
      aCS = true;
    }
  else if(rawCh < 0 || rawCh > ASCII_CH_MAX)
    {
      exit(concat("Error: encountered character (", (int)rawCh, ") that is lass"
		  " than 0 or greater than ", maxCharNum,". after having color "
		  "info removed."),
	   ERROR_CHARACTER_RANGE);
    }
  
  return rawCh;
}


std::chrono::steady_clock::time_point setCursorVisibility
(bool & cursorOn, const std::chrono::steady_clock::time_point tLast)
{
  std::chrono::steady_clock::time_point retTime {tLast};
  
  if(std::chrono::steady_clock::now() - tLast >
     editingSettings::cursorBlinkTimeMs)
    {
      retTime = std::chrono::steady_clock::now();
	  
      if(cursorOn)
	{
	  // Set cursor to visible.
	  curs_set(2);
	  cursorOn = !cursorOn;
	}
      else
	{
	  curs_set(0);
	  cursorOn = !cursorOn;
	}
    }

  return retTime;
}
