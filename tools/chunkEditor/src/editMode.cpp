#include <chrono>
#include <filesystem>
#include <stack>
#include <curses.h>
#include <fstream>
#include <set>
#include "include/editMode.hpp"


namespace editingSettings
{    
  setColorMode colorMode {};
}


struct editingState
{
  struct selectionBufferElement
  {
    backgroundChunkCharType ch;
    yx coord;
  };
  // Are we editing the cRChunk or the bgChunk?
  bool cRChunkToggle;
  /* Show reference chunk (note that the chunk show is dependant on the state of
     cRChunkToggle */
  bool refrenceChunkToggle;
  bool lineDrawModeToggle;
  bool crosshairCursorToggle;
  int input;
  yx cursorPos;
  
private:
  
  static constexpr int bgCharsRingBufferSize {24};
  static constexpr int selectionRingBuffserSize {24};
  // Buffer to remember bgCharsRingBufferSize of the last bg chars.
  int bgCharsRingBuffer [bgCharsRingBufferSize] {};
  // Buffer to remeber selectionRingBuffserSize of the last selected selection.
  std::vector<selectionBufferElement>
  selectionRingBuffer [selectionRingBuffserSize] {};
  int currentBgCharIndex {};
  int currentSelectionIndex {};
  bool cursorOn;
  bool displaySelectionColorsForSelecting {false};
  
  const std::chrono::milliseconds crosshairCursorColorTransitionTime;
  const std::chrono::milliseconds displaySelectionColorsForSelectingTimeMs;
  std::chrono::steady_clock::time_point cursorVisibilityChangeTimeLast;
  std::chrono::steady_clock::time_point crosshairCursorColorTransitionTimeLast;
  std::chrono::steady_clock::time_point
  displaySelectionColorsForSelectingTimeLast;
  int crosshairCursorColorPair;

public:
  char currentCRChar;

  editingState
  (const bool cRChunkToggle, const bool cursorOn,
   const std::chrono::milliseconds crosshairTransitionTimeMs,
   const std::chrono::milliseconds dispSelectionSelectingTimeMs,
   const int input, const yx cursorPos, const int currentBgChar,
   const int currentCRChar):
    crosshairCursorColorTransitionTime(crosshairTransitionTimeMs),
    displaySelectionColorsForSelectingTimeMs(dispSelectionSelectingTimeMs)
  {
    this->cRChunkToggle		= cRChunkToggle;
    this->refrenceChunkToggle	= false;
    this->lineDrawModeToggle	= false;
    this->crosshairCursorToggle = false;
    this->cursorOn		= cursorOn;
    this->cursorVisibilityChangeTimeLast = std::chrono::steady_clock::now();
    this->displaySelectionColorsForSelectingTimeLast =
      std::chrono::steady_clock::now();
    this->input			= input;
    this->cursorPos		= cursorPos;
    for(int iter {}; iter < bgCharsRingBufferSize; ++iter)
      {
	bgCharsRingBuffer[iter] = currentBgChar;
      }
    this->currentCRChar = currentCRChar;
    this->crosshairCursorColorTransitionTimeLast =
      std::chrono::steady_clock::now();
    this->crosshairCursorColorPair = editingSettings::helpColor;
  }
  
  int getCurrentBgChar() const
  {
    return bgCharsRingBuffer[currentBgCharIndex];
  }

  std::vector<selectionBufferElement> getCurrentSelection()
  {
    return selectionRingBuffer[currentSelectionIndex];
  }

  void setCurrentBgChar(const int newChar)
  {
    // Cycle back to zero if index goes out of range.
    currentBgCharIndex = (currentBgCharIndex + 1) % bgCharsRingBufferSize;
    bgCharsRingBuffer[currentBgCharIndex] = newChar;
  }

  void setCurrentSelection(const std::vector<selectionBufferElement> & newSelection)
  {
    // currentSelectionIndex <
    currentSelectionIndex = (currentSelectionIndex + 1) %
      selectionRingBuffserSize;
    selectionRingBuffer[currentSelectionIndex] = newSelection;
  }

  void recallNextBgChar()
  {
    currentBgCharIndex < bgCharsRingBufferSize -1 ? currentBgCharIndex++:
      currentBgCharIndex = 0;
  }

  void recallLastBgChar()
  {
    currentBgCharIndex == 0 ?
      currentBgCharIndex = bgCharsRingBufferSize - 1:
      currentBgCharIndex--;
  }


  void setCursorVisibility()
  {
    if(std::chrono::steady_clock::now() - cursorVisibilityChangeTimeLast >
       editingSettings::cursorBlinkTimeMs)
      {
	cursorVisibilityChangeTimeLast = std::chrono::steady_clock::now();
	
	cursorOn = !cursorOn;

	if(cursorOn)
	  {
	    // Set cursor to visible.
	    curs_set(2);
	  }
	else
	  {
	    curs_set(0);
	  }
      }
  }


  int getCrosshairColor()
  {
    if(std::chrono::steady_clock::now() -
       crosshairCursorColorTransitionTimeLast >
       editingSettings::crosshairTransitionTimeMs)
      {
	crosshairCursorColorTransitionTimeLast = std::chrono::steady_clock::now();
	crosshairCursorColorPair = editingSettings::colorMode.getRandomColor();
      }
    
    return crosshairCursorColorPair;
  }

  /* Returns true if colors should be displayed for selecting (this should be
     used when a selection is being created (as opposed to when one is being
     used.) */
  bool shouldDisplaySelectionColorsForSelecting()
  {
    if(std::chrono::steady_clock::now() -
       displaySelectionColorsForSelectingTimeLast >
       displaySelectionColorsForSelectingTimeMs)
      {
	displaySelectionColorsForSelectingTimeLast =
	  std::chrono::steady_clock::now();
	displaySelectionColorsForSelecting =
	  !displaySelectionColorsForSelecting;
      }
    return displaySelectionColorsForSelecting;
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
void editModeProper
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk,
 const backgroundChunkCharType refBgChunk[yHeight][xWidth],
 const char refCRChunk[yHeight][xWidth], const yx chunkSize,
 const bool usingReferences);
std::chrono::steady_clock::time_point setCursorVisibility
(bool & cursorOn, const std::chrono::steady_clock::time_point tLast);
bool getConfirmation(const yx viewPortSize, editingState & edState,
		     const std::string & question);
void actOnInputLayer1
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx & chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 const bool usingReferences, editingState & edState);
/* Updates the cursors position in edState if edState.input is one of cursorUp,
   cursorDown, cursorLeft or cursorRight and if the cursor will not go outside
   of the view port. Also sets the character at edState.cursorPos in bgChunk or
   cRChunk to the current cursor character depending on various fields in
   edState. Namely edState.lineDrawModeToggle and edState.cRChunkToggle, where
   lineDrawModeToggle controls if a character will be set and cRChunkToggle
   controls weather it will be in bgChunk or cRChunk. */
bool updateCursorPos
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 editingState & edState);
/* Updates the cursors position in edState if edState.input is one of cursorUp,
   cursorDown, cursorLeft or cursorRight and if the cursor will not go outside
   of the view port. */
bool updateCursorPos(const yx chunkSize, editingState & edState);
/* Same as updateCursorPos(const yx chunkSize, editingState & edState) with the
   exception that the cursor pos can be set outside of the view port. */
bool updateCursorPos
(const yx chunkSize, editingState & edState, yx minPos, yx maxPos);
/* Updates character at edState.cursorPos in bgChunk or cRChunk with the current
   cursor ch depending the value of edState.cRChunkToggle.  */
void performActionAtPosFunc
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, editingState & edState);
void actOnInputLayer2
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx & chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 editingState & edState);
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
// /* If the character at edState.cursorPos is different from
//    edState.getCurrentBgChar() then all characters equal to the character at
//    edState.cursorPos that are contiguous with it will be set to
//    edState.getCurrentBgChar(). */
// void floodFillBg
// (T bgChunk[][xWidth], const yx chunkSize,
//  editingState & edState);
void floodFillBg
(backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState);
/* Allows the user to select a region (selection) for later pasting. */
void selectAndCopySelection
(backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState);
/* Allows the user to paste previously selected regions from the selection
   buffer in edState.*/
void pasteSelectionIntoBg
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk, const yx chunkSize,
 editingState & edState);
/* Prints changing colored emptyCharChar for each unset character in bgChunk (at the
   position of the unset character.) Returns if the user pressed
   editChars::quit. */
void showUnsetBgChars
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState);
/* Prints a message showing the current coordinates and asking the user to
   enter new coordinates, can be aborted using editChars::quit. New coordinates
   are only saved when writeOutChunks is called. */
void showAndChangeCoorinates
(const yx viewPortSize, editingState & edState, yx & chunkCoord);
/* Writes contents of bgChunk.getChunk() and cRChunk.getChunk() to the files
   at bgChunkFileName and cRChunkFileName respectively. Prints error message and
   waits for user input if there is an error. */
bool writeOutChunks
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 const yx chunkCoord, const backgroundChunkCharInfo bgChunk[][xWidth],
 const char cRChunk[][xWidth], const yx chunkSize);
/* Once entered this function will not exit untill one of editChars::quit or
   editChars::toggleHelpMsg is pressed.  */
void printEditHelp(const yx viewPortSize, editingState & edState);
void printBgCharAtPos(backgroundChunkCharType ch, const yx pos);
void printBgChunk
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx viewPortSize);
void printRandomColorAtCursoPos(editingState & edState);
void printCursorForCREditMode
(const yx viewPortSize, editingState & edState);
void printCursorForBgEditMode
(const yx viewPortSize, editingState & edState);
/* Prints a crosshair for the cursor if edState.crosshairCursortoggle is
   true. */
void printCursorCrossHair
(const yx viewPortSize, editingState & edState);


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
  backgroundChunkCharType rawChunk[yHeight][xWidth];
  readInBgChunkFile(fileName, rawChunk, chunkSize, chunkCoord, foundCoord);

  for(int yIter {}; yIter < chunkSize.y; yIter++)
    {
      for(int xIter {}; xIter < chunkSize.x; xIter++)
	{
	  chunk[yIter][xIter].ch = rawChunk[yIter][xIter];
	  if(foundCoord)
	    {
	      chunk[yIter][xIter].set = true;
	    }
	  else
	    {
	      chunk[yIter][xIter].set = false;
	    }
	}
    }
}


void editMode
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 const std::string refBgChunkFileName, const std::string refCRChunkFileName,
 const yx chunkSize, const bool usingReferences)
{
  chunk<backgroundChunkCharInfo, yHeight, xWidth> bgChunk
    {backgroundChunkCharInfo{editingSettings::invalidCharColorPair, false},
     editingSettings::showLastChunkAfterUpdateIndexFor};
  chunk<char, yHeight, xWidth> cRChunk
    {emptyCharChar, editingSettings::showLastChunkAfterUpdateIndexFor};
  
  bool foundBgChunkCoord {false}, foundCRChunkCoord {false},
    foundRefBgChunkCoord {false}, foundRefCRChunkCoord {false};
  yx bgChunkCoord {}, cRChunkCoord {}, dummyCoord;

  // Read files into bgChunk and cRChunk (if they exist.)
  readInBgChunkFile(bgChunkFileName, bgChunk.advanceBeforeModify().data,
		    chunkSize, bgChunkCoord, foundBgChunkCoord);
  readInCRChunkFile(cRChunkFileName, cRChunk.advanceBeforeModify().data,
		    chunkSize, cRChunkCoord, foundCRChunkCoord);
  if(!foundBgChunkCoord)
    {
      // Chunk coord not found i.e. file not found. So initialise chunk.
      fillChunk(bgChunk.advanceBeforeModify().data,
		chunkSize, (int)editingSettings::invalidCharColorPair,
		[](auto & element, const int filler)
		{
		  element.ch = filler;
		});
    }
  if(!foundCRChunkCoord)
    {
      // Chunk coord not found i.e. file not found. So initialise chunk.
      fillChunk(cRChunk.advanceBeforeModify().data, chunkSize,
		editingSettings::rulesChars::nullRule,
		[](auto & element, const char filler)
		{
		  element = filler;
		});
    }

  backgroundChunkCharType 	refBgChunk[yHeight][xWidth];
  char				refCRChunk[yHeight][xWidth];
  if(usingReferences)
    {
      // Read in reference bgChunk and cRChunk files.
      readInBgChunkFile(refBgChunkFileName, refBgChunk, chunkSize,
			dummyCoord, foundRefBgChunkCoord);
      readInCRChunkFile(refCRChunkFileName, refCRChunk, chunkSize,
			dummyCoord, foundRefCRChunkCoord);
      if(!foundRefBgChunkCoord || !foundRefCRChunkCoord)
	{
	  endwin();
	  printMessageNoWin("Error: unable to find one or more reference chunk"
			    " files (see help for more info.) Is the path "
			    "wrong?", printCharSpeed, afterPrintSleep);
	  exit(ERROR_INVALID_CMD_ARGS);
	}
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
      editModeProper(bgChunkFileName, cRChunkFileName, bgChunkCoord,
		     bgChunk, cRChunk, refBgChunk, refCRChunk,
		     chunkSize, usingReferences);
    }
  else
    {
      // Both or just one coord could be valid, so we choose one that is.
      editModeProper(bgChunkFileName, cRChunkFileName,
		     foundBgChunkCoord? bgChunkCoord: cRChunkCoord,
		     bgChunk, cRChunk,  refBgChunk, refCRChunk,
		     chunkSize, usingReferences);
    }
}


void editModeProper
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk,
 const backgroundChunkCharType refBgChunk[yHeight][xWidth],
 const char refCRChunk[yHeight][xWidth], const yx chunkSize,
 const bool usingReferences)
{
  editingState edState
    {
      false,
      true,
      editingSettings::crosshairTransitionTimeMs,
      editingSettings::dispSelectionSelectingTimeMs,
      ERR, 	        // This is returned by getch() when there is no input.
      yx{chunkSize.y / 2, chunkSize.x / 2},
      editingSettings::initialCursorChar,
      editingSettings::rulesChars::boarder
    };
  
  while(!confirmQuit(chunkSize, edState.input,
		     editingSettings::editChars::quit))
    {
      edState.input = getch();
      
      // Perform some action based on input.
      actOnInputLayer1(bgChunkFileName, cRChunkFileName, chunkCoord, bgChunk,
		       cRChunk, chunkSize, usingReferences, edState);

      // We always print the background chunk.
      if(!edState.refrenceChunkToggle)
	{
	  printBgChunk(bgChunk.getChunk().data, chunkSize);
	  if(edState.cRChunkToggle)
	    {
	      printCRChunk(cRChunk.getChunk().data, chunkSize);
	    }
	}
      else
	{
	  printBgChunk(refBgChunk, chunkSize);
	  if(edState.cRChunkToggle)
	    {
	      printCRChunk(refCRChunk, chunkSize);
	    }
	}

	// The cursor should generally always be visible, so print last.
      if(edState.cRChunkToggle)
	{
	  printCursorForCREditMode(chunkSize, edState);
	  cRChunk.printIndexIfChanged(chunkSize.y -1, 0);
	}
      else
	{
	  printCursorForBgEditMode(chunkSize, edState);
	  bgChunk.printIndexIfChanged(chunkSize.y -1, 0);
	}

      edState.setCursorVisibility();
      
      sleep(editingSettings::loopSleepTimeMs);
    }
}


bool getConfirmation(const yx viewPortSize, editingState & edState,
		     const std::string & question)
{
  return getConfirmation(viewPortSize, editingSettings::helpColor,
			 edState.input,
			 editingSettings::editSubMenuSleepTimeMs,
			 question);
}


void actOnInputLayer1
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx & chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 const bool usingReferences, editingState & edState)
{ 
  if(!updateCursorPos(bgChunk, cRChunk, chunkSize, edState))
    {
      switch(edState.input)
	{
	case editingSettings::editChars::toggleBetweenCRandBg:
	  edState.cRChunkToggle = !edState.cRChunkToggle;
	  break;
	case editingSettings::editChars::toggleReferenceChunkView:
	  // Only try and show reference chunks if they actually exist.
	  if(usingReferences)
	    {
	      edState.refrenceChunkToggle = !edState.refrenceChunkToggle;
	    }
	  break;
	case editingSettings::editChars::toggleHelpMsg:
	  printEditHelp(chunkSize, edState);
	  break;
	default:
	  if(!edState.refrenceChunkToggle)
	    {
	      actOnInputLayer2(bgChunkFileName, cRChunkFileName, chunkCoord, bgChunk,
			       cRChunk, chunkSize, edState);
	    }
	};
    }
}


bool updateCursorPos
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 editingState & edState)
{
  using namespace editingSettings::editChars;

  bool ret {false};

  auto sharedCursorMovementAction = [& bgChunk, & cRChunk, & edState, & ret]()
  {
    move(edState.cursorPos);
    if(edState.lineDrawModeToggle)
      {
	performActionAtPosFunc(bgChunk, cRChunk, edState);
      }
    ret = true;
  };
  
  switch(edState.input)
    {
    case cursorUp:
      if(edState.cursorPos.y > 0)
	{
	  edState.cursorPos.y--;
	  sharedCursorMovementAction();
	}
      break;
    case cursorDown:
      if(edState.cursorPos.y < chunkSize.y -1)
	{
	  edState.cursorPos.y++;
	  sharedCursorMovementAction();
	}
      break;
    case cursorLeft:
      if(edState.cursorPos.x > 0)
	{
	  edState.cursorPos.x--;
	  sharedCursorMovementAction();
	}
      break;
    case cursorRight:
      if(edState.cursorPos.x < chunkSize.x -1)
	{
	  edState.cursorPos.x++;
	  sharedCursorMovementAction();
	}
      break;
    }

  return ret;
}


bool updateCursorPos
(const yx chunkSize, editingState & edState)
{
  return updateCursorPos
    (chunkSize, edState, yx{0, 0}, {chunkSize.y, chunkSize.x});
}


bool updateCursorPos
(const yx chunkSize, editingState & edState, yx minPos, yx maxPos)
{
  using namespace editingSettings::editChars;

  bool ret {false};

  auto sharedCursorMovementAction = [& edState, & ret]()
  {
    move(edState.cursorPos);
    ret = true;
  };
  
  switch(edState.input)
    {
    case cursorUp:
      if(edState.cursorPos.y > minPos.y)
	{
	  edState.cursorPos.y--;
	  sharedCursorMovementAction();
	}
      break;
    case cursorDown:
      if(edState.cursorPos.y < maxPos.y -1)
	{
	  edState.cursorPos.y++;
	  sharedCursorMovementAction();
	}
      break;
    case cursorLeft:
      if(edState.cursorPos.x > minPos.x)
	{
	  edState.cursorPos.x--;
	  sharedCursorMovementAction();
	}
      break;
    case cursorRight:
      if(edState.cursorPos.x < maxPos.x -1)
	{
	  edState.cursorPos.x++;
	  sharedCursorMovementAction();
	}
      break;
    }

  return ret;
}


void performActionAtPosFunc
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, editingState & edState)
{
  if(edState.cRChunkToggle)
    {
      cRChunk.advanceBeforeModify().
	data[edState.cursorPos.y][edState.cursorPos.x] =
	edState.currentCRChar;
    }
  else
    {
      bgChunk.advanceBeforeModify().
	data[edState.cursorPos.y][edState.cursorPos.x].ch =
	edState.getCurrentBgChar();
      bgChunk.getChunk().
	data[edState.cursorPos.y][edState.cursorPos.x].set = true;
    }
}


void actOnInputLayer2
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 yx & chunkCoord, chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk,
 chunk<char, yHeight, xWidth> & cRChunk, const yx chunkSize,
 editingState & edState)
{
  using namespace editingSettings::editChars;
  
  switch(edState.input)
    {
    case performActionAtPos:
      performActionAtPosFunc(bgChunk, cRChunk, edState);
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
    case undo:
      if(!edState.cRChunkToggle)
	{
	  bgChunk.backward();
	}
      else
	{
	  cRChunk.backward();
	}
      break;
    case redo:
      if(!edState.cRChunkToggle)
	{
	  bgChunk.forward();
	}
      else
	{
	  cRChunk.forward();
	}
      break;
    case bgGetCharAtPos:
      if(!edState.cRChunkToggle && bgChunk.getChunk().data
	     [edState.cursorPos.y][edState.cursorPos.x].set)
	{
	  edState.setCurrentBgChar
	    (bgChunk.getChunk().data
	     [edState.cursorPos.y][edState.cursorPos.x].ch);
	}
      break;
    case toggleLineDrawMode:
      {
	edState.lineDrawModeToggle = !edState.lineDrawModeToggle;
	if(edState.lineDrawModeToggle)
	  {
	    performActionAtPosFunc(bgChunk, cRChunk, edState);
	  }
      }
      break;
    case floodFill:
      if(!edState.cRChunkToggle)
	{
	  floodFillBg
	    (bgChunk.advanceBeforeModify().data, chunkSize, edState);
	}
      break;
    case selectSelection:
      selectAndCopySelection
	(bgChunk.getChunk().data, chunkSize, edState);
      break;
    case pasteSelection:
      pasteSelectionIntoBg(bgChunk, chunkSize, edState);
      break;
    case bgShowUnsetChars:
      if(!edState.cRChunkToggle)
	{
	  showUnsetBgChars(bgChunk.getChunk().data, chunkSize, edState);
	}
      break;
    case toggleCrosshairCursor:
      edState.crosshairCursorToggle = !edState.crosshairCursorToggle;
      break;
    case changeCoordinates:
      showAndChangeCoorinates(chunkSize, edState, chunkCoord);
      break;
    case saveChunks:
      if(getConfirmation(chunkSize, edState,
			 "\tdo you really want to save y/n?\t"))
	{
	  progressivePrintMessage
	    (concat("\tSaving chunks...\t"),
	     chunkSize, 0, 0, false, false);
	  if(writeOutChunks(bgChunkFileName, cRChunkFileName,
			    chunkCoord, bgChunk.getChunk().data,
			    cRChunk.getChunk().data, chunkSize))
	    {
	      progressivePrintMessage
		(concat("\tChunks saved...\t"),
		 chunkSize, 0, afterPrintSleep, false, false);
	    }
	}
      break;
    }
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
		     character on in the view port is an ASCII character or an
		     ACS character. We use the cursor position to determine
		     which character set we are dealing with. */
		  if(edState.cursorPos.y == printACSAtY)
		    {
		      bool foundACS {false};

		      for(int aCSIter {}; (size_t)aCSIter < aCSChars.size();
			  ++aCSIter)
			{
			  if(acs_map[charAtPos & A_CHARTEXT] ==
			     aCSChars[aCSIter])
			    {
			      /* Note here that aSCIIChMaxEffective has
				 lowerUnusedASCIIChNum removed from it as
				 characters below this point aren't really
				 printable (this allows us to use unsigned
				 shorts for character rather than ints.) */
			      edState.setCurrentBgChar
				(charColorOffset +
				 aSCIIChMaxEffective + aCSIter + 1);
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
		      /* Note here that we remove lowerUnusedASCIIChNum the
			 characters below this point aren't really printable
			 (this allows us to use unsigned shorts for character
			 rather than ints.) */
		      edState.setCurrentBgChar
			(charColorOffset + (charAtPos & A_CHARTEXT) -
			 lowerUnusedASCIIChNum);
		    }
		  foundChar = true;
		  break;
		}
	    }

	  edState.setCursorVisibility();
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
		       won't to get the bg color (as that is the visible
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
      
      edState.setCursorVisibility();
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
  safeScreenExit(edState.input, editingSettings::editSubMenuSleepTimeMs);
}



void floodFillBg
(backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState)
{
  /*
    Sudo code:
        function floodFillBg(image, x, y, targetColor, replacementColor):
          if pixel at (x, y) is not the targetColor:
              return

          create a stack data structure
          push (x, y) onto the stack

          while stack is not empty:
              currentPixel = pop from stack
              if pixel at currentPixel is the targetColor:
                  set pixel at currentPixel to replacementColor

                  push neighboring pixels that are the targetColor onto the stack
                  (x, y) = currentPixel
                  push (x+1, y), (x-1, y), (x, y+1), (x, y-1) onto the stack

          return
  */
  const int targetCharToReplace
    {bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch};
		   
  if(edState.getCurrentBgChar() != targetCharToReplace)
    {
      std::stack<yx> locationsToSet {};
      locationsToSet.push(edState.cursorPos);

      while(locationsToSet.size() > 0)
	{
	  yx currentPos = locationsToSet.top();
	  // Pop doesn't return anything, so we use top as seen above.
	  locationsToSet.pop();
	  if(bgChunk[currentPos.y][currentPos.x].ch == targetCharToReplace)
	    {
	      /* Current location is equal to targetCharToReplace. Set current
		 location to edState.getCurrentBgChar(). */
	      bgChunk[currentPos.y][currentPos.x].ch =
		edState.getCurrentBgChar();
	      bgChunk[currentPos.y][currentPos.x].set = true;

	      /* Add adjoining locations (if they are equal to
		 targetCharToReplace) */
	      if(currentPos.y - 1 >= 0 &&
		 bgChunk[currentPos.y - 1][currentPos.x].ch ==
		 targetCharToReplace)
		{
		  locationsToSet.push(yx{currentPos.y - 1, currentPos.x});
		}
	      if(currentPos.y + 1 < chunkSize.y &&
		 bgChunk[currentPos.y + 1][currentPos.x].ch ==
		 targetCharToReplace)
		{
		  locationsToSet.push(yx{currentPos.y + 1, currentPos.x});
		}
	      if(currentPos.x - 1 >= 0 &&
		 bgChunk[currentPos.y][currentPos.x - 1].ch ==
		 targetCharToReplace)
		{
		  locationsToSet.push(yx{currentPos.y, currentPos.x - 1});
		}
	      if(currentPos.x + 1 < chunkSize.x &&
		 bgChunk[currentPos.y][currentPos.x + 1].ch ==
		 targetCharToReplace)
		{
		  locationsToSet.push(yx{currentPos.y, currentPos.x + 1});
		}
	    }

	  if(locationsToSet.size() < 1024)
	    {
	      printBgChunk(bgChunk, chunkSize);
	      editingSettings::colorMode.setColor(editingSettings::helpColor);
	      mvprintw(0, 0, "%d", locationsToSet.size());
	      refresh();
	    }
	}
    }
}


void selectAndCopySelection
(backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState)
{
  std::vector<editingState::selectionBufferElement> newSelection;

  /* Add addee to newSelection as if it were a set (we don't want duplicate
     coordinates.) */
  auto setAdd = [& newSelection]
    (const backgroundChunkCharType ch, const yx coord, bool deleteIfDuplicate)
  {
    const editingState::selectionBufferElement addee {ch, coord};
    bool found {false};
    for(int iter {}; iter < newSelection.size(); iter++)
      {
	if(addee.coord.y == newSelection[iter].coord.y &&
	   addee.coord.x == newSelection[iter].coord.x)
	  {
	    if(deleteIfDuplicate)
	      {
		/* This way the user and add / remove elements from the
		   selection. */
		newSelection.erase(newSelection.begin() + iter);
	      }
	    else
	      {
		newSelection[iter] = addee;
	      }
	    found = true;
	    break;
	  }
      }
    if(!found)
      {
	newSelection.push_back(addee);
      }
  };

  auto printSelection = [& edState, & newSelection]()
  {
    if(edState.shouldDisplaySelectionColorsForSelecting())
      {
	for(auto element: newSelection)
	  {
	    editingSettings::colorMode.setRandomColor();
	    mvprintw(element.coord.y, element.coord.x, " ");
	  }
      }
  };
  
  do
    {
      edState.input = getch();

      if(!updateCursorPos(chunkSize, edState))
	{
	  switch(edState.input)
	    {
	    case editingSettings::editChars::toggleHelpMsg:
	      // TODO: change to context specific help message!
	      exit(-1); //printEditHelp(chunkSize, edState);
	      break;
	    case editingSettings::editChars::performActionAtPos:
	      setAdd(bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch,
		     edState.cursorPos, true);
	      break;
	    case editingSettings::editChars::toggleCrosshairCursor:
	      edState.crosshairCursorToggle = !edState.crosshairCursorToggle;
	      break;
	    case editingSettings::editChars::toggleLineDrawMode:
	      {
		edState.lineDrawModeToggle = !edState.lineDrawModeToggle;
		if(edState.lineDrawModeToggle)
		  {
		    setAdd(bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch,
		     edState.cursorPos, true);
		  }
	      }
	      break;
	    case editingSettings::editChars::floodFill:
	      break;
	    case editingSettings::editChars::selectSelection:
	      // Save selection and exit function!
	      edState.setCurrentSelection(newSelection);
	      progressivePrintMessage
		(concat("New selection added!"),
		 chunkSize, printCharSpeed, 0);
	      edState.input = editingSettings::editChars::quit;
	      break;
	    }
	}
      else
	{
	  // Cursor moved. Update newSelection if in line draw mode.
	  if(edState.lineDrawModeToggle)
	    {
	      setAdd(bgChunk[edState.cursorPos.y][edState.cursorPos.x].ch,
		     edState.cursorPos, true);
	    }
	}

      printBgChunk(bgChunk, chunkSize);
      printSelection();
      printCursorForBgEditMode(chunkSize, edState);
      edState.setCursorVisibility();

      sleep(editingSettings::loopSleepTimeMs);
    }
  while(edState.input != editingSettings::editChars::quit);

  safeScreenExit(edState);
}


void pasteSelectionIntoBg
(chunk<backgroundChunkCharInfo, yHeight, xWidth> & bgChunk, const yx chunkSize,
 editingState & edState)
{
  // Minimum offset of selection from origin.
  yx selectionMinOffsetFromOrigin {}, selectionMaxOffsetFromOrigin {};
  std::vector<editingState::selectionBufferElement> currentSelection
    {edState.getCurrentSelection()};

  /* If min is true getSelectionOffsetFromOrigin will return minimum offset
     from the origin and if it is false the function will return the maximum
     offset. */
  auto getSelectionOffsetFromOrigin =
    [chunkSize, & currentSelection]
    (yx & selectionOffsetFromOrigin, const bool min)
    {
      selectionOffsetFromOrigin = min ? chunkSize: yx{0, 0};
      for(auto element: currentSelection)
	{
	  if(min)
	    {
	      if(element.coord.y < selectionOffsetFromOrigin.y)
		{
		  selectionOffsetFromOrigin.y = element.coord.y;
		}
	      if(element.coord.x < selectionOffsetFromOrigin.x)
		{
		  selectionOffsetFromOrigin.x = element.coord.x;
		}
	    }
	  else
	    {
	      if(element.coord.y > selectionOffsetFromOrigin.y)
		{
		  selectionOffsetFromOrigin.y = element.coord.y;
		}
	      if(element.coord.x > selectionOffsetFromOrigin.x)
		{
		  selectionOffsetFromOrigin.x = element.coord.x;
		}
	    }
	}
    };

  auto printSelection =
    [chunkSize, & edState, & selectionMinOffsetFromOrigin, & currentSelection]()
    {
      for(auto element: currentSelection)
	{
	  // Print from origin + cursorPos.
	  const yx printPos
	    {element.coord.y - selectionMinOffsetFromOrigin.y +
	     edState.cursorPos.y,
	     element.coord.x - selectionMinOffsetFromOrigin.x +
	     edState.cursorPos.x};
	  if(printPos.y >= 0 && printPos.y < chunkSize.y &&
	     printPos.x >= 0 && printPos.x < chunkSize.x)
	    {
	      printBgCharAtPos
		(element.ch, printPos);
	    }
	}
  };

  auto pasteSelection =
    [& bgChunk, chunkSize, & edState, & selectionMinOffsetFromOrigin, & currentSelection]()
    {
      yx printPos {};
      backgroundChunkCharType lastCh {};
      
      for(auto element: currentSelection)
	{
	  // Paste from origin + cursorPos.
	  printPos = 
	    yx{element.coord.y - selectionMinOffsetFromOrigin.y +
	       edState.cursorPos.y,
	       element.coord.x - selectionMinOffsetFromOrigin.x +
	     edState.cursorPos.x};
	  /* Save last ch for use with bgChunk.advanceBeforeModify() */
	  lastCh = element.ch;	
	  
	  if(printPos.y >= 0 && printPos.y < chunkSize.y &&
	     printPos.x >= 0 && printPos.x < chunkSize.x)
	    {
	      /* Normally we wouldn't call getChunk(), however this is a special
		 excption and we call advanceBeforeModify() after this loop. */
	      bgChunk.getChunk().data[printPos.y][printPos.x].ch = element.ch;
	      bgChunk.getChunk().data[printPos.y][printPos.x].set = true;
	    }
	  // Make sure this is counted as an undo-able action!
	  bgChunk.advanceBeforeModify().data[printPos.y][printPos.x].ch = lastCh;
	}
    };
  
  getSelectionOffsetFromOrigin(selectionMinOffsetFromOrigin, true);
  getSelectionOffsetFromOrigin(selectionMaxOffsetFromOrigin, false);
  
  do
    {
      edState.input = getch();

      /* We have to be able to move the cursor in the range [-sizeof(selection),
	 chunkSize) */
      if(!updateCursorPos
	 (chunkSize, edState,
	  yx
	  {-(selectionMaxOffsetFromOrigin.y - selectionMinOffsetFromOrigin.y),
	   -(selectionMaxOffsetFromOrigin.x - selectionMinOffsetFromOrigin.x)},
	  chunkSize))
	{
	  switch(edState.input)
	    {
	    case editingSettings::editChars::toggleHelpMsg:
	      // TODO: change to context specific help message!
	      exit(-1); //printEditHelp(chunkSize, edState);
	      break;
	    case editingSettings::editChars::performActionAtPos:
	      // Paste selection into bg.
	      pasteSelection();
	      edState.input = editingSettings::editChars::quit;
	      break;
	    case editingSettings::editChars::toggleCrosshairCursor:
	      edState.crosshairCursorToggle = !edState.crosshairCursorToggle;
	      break;
	    }
	}

      printBgChunk(bgChunk.getChunk().data, chunkSize);
      printCursorForBgEditMode(chunkSize, edState);
      printSelection();
      edState.setCursorVisibility();

      sleep(editingSettings::loopSleepTimeMs);
    }
  while(edState.input != editingSettings::editChars::quit);

  safeScreenExit(edState);
}


void showUnsetBgChars
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx chunkSize,
 editingState & edState)
{
  using namespace editingSettings;

  safeScreenExit(edState);
	
  while(edState.input != editChars::quit &&
	edState.input != editChars::bgShowUnsetChars)
    {
      if(edState.input == editChars::toggleHelpMsg)
	{
	  printEditHelp(chunkSize, edState);
	}
      for(int yIter {}; yIter < chunkSize.y; yIter++)
	{
	  for(int xIter {}; xIter < chunkSize.x; xIter++)
	    {
	      if(!bgChunk[yIter][xIter].set)
		{
		  int color = rand() % 2 ? blackBgColorPair: validColorNumber;
		  colorMode.setColor(color);
		  mvprintw(yIter, xIter, "%c", emptyCharChar);
		}
	    }
	}
      
      refresh();
      edState.input = getch();
      sleep(editSubMenuSleepTimeMs);
    }  
  safeScreenExit(edState);
}


void showAndChangeCoorinates
(const yx viewPortSize, editingState & edState, yx & chunkCoord)
{
  using namespace editingSettings::editChars;

  std::string msg {concat
      ("\tThe current chunk coordinates are ", chunkCoord, ". Please enter new "
       "chunk coordinates (note that these coordinates will not be saved until "
       "the chunks are saved.): ")};
  
  editingSettings::colorMode.setColor(editingSettings::helpColor);

  progressivePrintMessage
    (msg, viewPortSize, 0, 200, false, false);

  enableBlockingUserInput();
  while(scanw("%d %d", &chunkCoord.y, &chunkCoord.x) != 2)
    {
      progressivePrintMessage
	(concat("Error: you must enter a chunk coordinate in the form of a "
		"y and x coordinate seperated by a space. Please enter "
		"coordinates: "), viewPortSize, printCharSpeed,
	 editingSettings::afterIncorrectCoordInputSleep);
      progressivePrintMessage
	(msg, viewPortSize, 0, 200, false, false);
    }
  disableBlockingUserInput();
  safeScreenExit(edState);
}


bool writeOutChunks
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 const yx chunkCoord, const backgroundChunkCharInfo bgChunk[][xWidth],
 const char cRChunk[][xWidth], const yx chunkSize)
{
  bool saved {false};
  bool incompleateBgChunk {false};
  
  for(int yIter {}; yIter < chunkSize.y; yIter++)
    {
      for(int xIter {}; xIter < chunkSize.x; xIter++)
	{
	  if(!bgChunk[yIter][xIter].set)
	    {
	      incompleateBgChunk = true;
	      break;
	    }
	}
    }

  if(incompleateBgChunk)
    {
      progressivePrintMessage
	("\tError: unable to save chunks as the background chunk is not "
	 "complete! See help for more info.\t",
	 chunkSize, 0, editingSettings::afterFileErrorPrintSleep, false, false);
    }
  else
    {
      std::ofstream file (bgChunkFileName, std::ios::binary);
      if (!file.is_open())
	{
	  progressivePrintMessage
	    (concat("\tError: unable to save background chunk to file \"",
		    bgChunkFileName, "\"\t"),
	     chunkSize, 0,
	     editingSettings::editSubMenuSleepTimeMs, false, false);
	}
      compressAndWriteOutBgChunk
	(bgChunkFileName, file, chunkCoord, bgChunk, chunkSize);
      file.close();

      file.open(cRChunkFileName, std::ios::binary);
      if (!file.is_open())
	{
	  progressivePrintMessage
	    (concat("\tError: unable to save coord rules chunk to file \"",
		    cRChunkFileName, "\"\t"),
	     chunkSize, 0,
	     editingSettings::editSubMenuSleepTimeMs, false, false);
	}
      compressAndWriteOutCRChunk
	(cRChunkFileName, file, chunkCoord, cRChunk, chunkSize);
      file.close();
      saved = true;
    }
  
  return saved;
}


void printEditHelp(const yx viewPortSize, editingState & edState)
{
  using namespace editingSettings::editChars;
  
  editingSettings::colorMode.setColor(editingSettings::helpColor);
  progressivePrintMessage
    (concat
     ("\t~H~E~L~P~!~\t\t\t\t\t\t",
      toggleHelpMsg,	": to toggle this message.\t\t\t",
      cursorUp,		": to move the cursor up.\t\t\t",
      cursorDown,	": to move the cursor down.\t\t\t",
      cursorLeft,	": to move the cursor left.\t\t\t",
      cursorRight,	": to move the cursor right.\t\t\t",
      "\"", performActionAtPos, "\": to perform an action at the current "
      "cursor position. The specific action performed depends on the context. "
      "The most important action is to change the character under the cursor "
      "to the same character as the cursor.\t\t\t",
      toggleBetweenCRandBg, ": to toggle between background and character "
      "rules chunks.\t\t\t",
      toggleReferenceChunkView, ": to toggle the reference chunk view (this is "
      "only relevant when in mode 2, see -h for more info.)\t\t\t",
      cREraseCRChar,	": to set the cursor character to the rules erase "
      "character when editing a choord rules chunk. This allows for the "
      "erasure of rules characters from the chunk.\t\t\t",
      cRSetCRCharToBorder, ": to set the cursor character to the boarder "
      "character when editing a choord rules chunk.\t\t\t",
      cRSetCRCharToPlatform, ": to set the cursor character to the platform "
      "character when editing a choord rules chunk.\t\t\t",
      bGToggleCharacterSelection, ": to select a new character color and "
      "character for the cursor character when editing a background chunk\t\t\t",
      bgNextCurrentChar,	": to cycle forward (next) through the recent "
      "cursor characters when editing a background chunk\t\t\t",
      bgLastCurrentChar, ": to cycle backwards (last) through the recent "
      "cursor characters when editing a background chunk\t\t\t",
      redo,		": to cycle the chunk buffer position forward (redo). ",
      "This cycles forward through the chunk buffer. Every time a chunk is "
      "altered the chunk at the current position in the buffer is unchanged "
      "and a copy of that chunk is saved in the next buffer position where the "
      "change is made. The current position is updated to the position of this "
      "new altered copy.\t\t\t",
      undo,		": to cycle the chunk buffer position back (undo). "
      "This cycles back through the chunk buffer. Every time a chunk is "
      "altered the chunk at the current position in the buffer is unchanged "
      "and a copy of that chunk is saved in the next buffer position where the "
      "change is made. The current position is updated to the position of this "
      "new altered copy.\t\t\t",
      bgGetCharAtPos,	": to set the cursor character to the background chunk "
      "character below it. This is only applicable if editing a background "
      "chunk and the character at the cursor pos has already been set.\t\t\t",
      toggleLineDrawMode, ": to toggle line drawing. When line drawing is "
      "turned on any character the user moves the cursor over will be set to "
      "the current cursor character. This way lines can be drawn without "
      "having to constantly press space.\t\t\t",
      floodFill,	": to do a flood fill using the cursor character and "
      "starting at the current cursor pos. This is only applicable if editing "
      "a background chunk\t\t\t",
      bgShowUnsetChars,	": to print noise in locations where characters aren't "
      "set. This helps when trying to find unset background chunk characters. "
      "Note that the chunks cannot be saved if any of the background chunk "
      "characters are unset. Also note that when editing a new background "
      "chunk file all characters are initially unset.\t\t\t",
      toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
      changeCoordinates, ": to change coordinates. This changes the "
      "coordinates of both the background chunk and rules character chunk as "
      "they are a set.\t\t\t",
      saveChunks,	": to save (output) both chunks.\t\t\t"),
     viewPortSize, printCharSpeed, afterPrintSleepMedium, false, false);

  edState.input = getch();  
  while(edState.input != quit &&
	edState.input != toggleHelpMsg)
    {
      edState.input = getch();
      sleep(editingSettings::loopSleepTimeMs);
    }

  safeScreenExit(edState);
}


void printBgCharAtPos(backgroundChunkCharType ch, const yx pos)
{
  bool foundACSCode;
	      
  setColorFromChar(ch);
  ch = getChar(ch, foundACSCode);

  if(foundACSCode)
    {
      printACS(pos, ch);
    }
  else
    {
      mvprintw(pos, concat("", (char)ch).c_str());
    }
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
	      printBgCharAtPos(bgChunk[yIter][xIter].ch, yx{yIter, xIter});
	    }
	  else
	    {
	      editingSettings::colorMode.setColor
		(editingSettings::blackBgColorPair);
	      mvprintw(yIter, xIter, " ");
	    }
	}
    }
}


void printRandomColorAtCursoPos(editingState & edState)
{
  editingSettings::colorMode.setRandomColor();
  mvprintw(edState.cursorPos, concat(" ").c_str());
  move(edState.cursorPos);
};


void printCursorForCREditMode
(const yx viewPortSize, editingState & edState)
{
  printCursorCrossHair(viewPortSize, edState);
  editingSettings::colorMode.setColor(editingSettings::blackBgColorPair);
  mvprintw(edState.cursorPos, concat("", edState.currentCRChar).c_str());
  move(edState.cursorPos);
}


void printCursorForBgEditMode
(const yx viewPortSize, editingState & edState)
{
  bool aCSChar {false};
  const int bgChar {getChar(edState.getCurrentBgChar(), aCSChar)};

  printCursorCrossHair(viewPortSize, edState);
  setColorFromChar(edState.getCurrentBgChar());
  if(aCSChar)
    {
      printACS(edState.cursorPos, bgChar);
    }
  else
    {
      mvprintw(edState.cursorPos, concat("", (char)bgChar));
    }
  move(edState.cursorPos);
}


void printCursorCrossHair
(const yx viewPortSize, editingState & edState)
{ 
  if(edState.crosshairCursorToggle)
    {
      editingSettings::colorMode.setColor(edState.getCrosshairColor());
      
      for(int xIter {}; xIter < edState.cursorPos.x; xIter++)
	{
	  mvprintw(edState.cursorPos.y, xIter, " ");
	}
      for(int xIter {edState.cursorPos.x + 1}; xIter < viewPortSize.x; xIter++)
	{
	  mvprintw(edState.cursorPos.y, xIter, " ");
	}

      for(int yIter {}; yIter < edState.cursorPos.y; yIter++)
	{
	  mvprintw(yIter, edState.cursorPos.x, " ");
	}
      for(int yIter {edState.cursorPos.y + 1}; yIter < viewPortSize.y; yIter++)
	{
	  mvprintw(yIter, edState.cursorPos.x, " ");
	}
    }
}
