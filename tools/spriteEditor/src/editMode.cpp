#include <filesystem>
#include <stack>
#include <curses.h>
#include <fstream>
#include "include/editMode.hpp"
#include "include/utils.hpp"


namespace editingSettings
{    
  setColorMode colorMode {};
}


struct editingState
{
  int input;
  yx cursorPos;
  /* For characters in the view port that have not been set we periodically
     change the color they are printed in so that the user can see what
     characters have been set (as if we just printed these unset characters as
     one color the user could set a character with this color. This could cause
     confusion.) */
  int randomColorPairForSlicePrint {};
  std::chrono::steady_clock::time_point randomColorPairForSlicePrintTimeLast;
  
private:
  bool cursorOn;
  std::chrono::steady_clock::time_point cursorVisibilityChangeTimeLast;
  // Buffer to remember x of the last bg chars.
  static constexpr int privSpriteCharsBufferSize {24};
  spriteCharType privSpritCharsRingBuffer [privSpriteCharsBufferSize] {};
  int currentPriveSpriteCharsRingBufferIndex {};

public:
  editingState
  (const bool cursorOn,
   const std::chrono::steady_clock::time_point cursorVisibilityChangeTimeLast,
   const int input, const yx cursorPos, const spriteCharType currentSpriteChar)
  {
    this->cursorOn = cursorOn;
    this->cursorVisibilityChangeTimeLast = cursorVisibilityChangeTimeLast;
    this->input = input;
    this->cursorPos = cursorPos;
    for(int iter {}; iter < privSpriteCharsBufferSize; ++iter)
      {
	privSpritCharsRingBuffer[iter] = currentSpriteChar;
      }
  }
  
  int getCurrentSpriteChar()
  {
    return privSpritCharsRingBuffer[currentPriveSpriteCharsRingBufferIndex];
  }

  void setCurrentSpriteChar(const int newChar)
  {
    // Cycle back to zero if index goes out of range.
    currentPriveSpriteCharsRingBufferIndex < privSpriteCharsBufferSize -1 ?
      currentPriveSpriteCharsRingBufferIndex++:
      currentPriveSpriteCharsRingBufferIndex = 0;
    privSpritCharsRingBuffer[currentPriveSpriteCharsRingBufferIndex] = newChar;
  }

  void recallNextSpriteChar()
  {
    currentPriveSpriteCharsRingBufferIndex < privSpriteCharsBufferSize -1 ?
      currentPriveSpriteCharsRingBufferIndex++:
      currentPriveSpriteCharsRingBufferIndex = 0;
  }

  void recallLastSpriteChar()
  {
    currentPriveSpriteCharsRingBufferIndex == 0 ?
      currentPriveSpriteCharsRingBufferIndex = privSpriteCharsBufferSize - 1:
      currentPriveSpriteCharsRingBufferIndex--;
  }

  void printCursor()
  {
    if(cursorOn)
      {
	bool aCSChar {false};
	const int bgChar {getChar(getCurrentSpriteChar(), aCSChar)};
    
	setColorFromChar(getCurrentSpriteChar());
	if(aCSChar)
	  {
	    printACS(cursorPos, bgChar);
	  }
	else
	  {
	    mvprintw(cursorPos, concat("", (char)bgChar));
	  }
      }
    move(cursorPos);
    cursorVisibilityChangeTimeLast =
      setCursorVisibility(cursorOn, cursorVisibilityChangeTimeLast);
  };

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
};  


// void readInSpriteFile
// (const std::string fileName, backgroundChunkCharInfo chunk[][xWidth],
//  const yx chunkSize, yx & chunkCoord, bool & foundCoord);
void editModeProper
(const std::string & spriteFileName,
 sprite<spriteCharData, yHeight, xWidth> & spriteObj,
 const yx chunkSize);
void actOnInputLayer1
(const std::string & spriteFileName,
 sprite<spriteCharData, yHeight, xWidth> & spriteObj,
 const yx chunkSize, editingState & edState);
/* Updates the cursors position in edState if edState.input is one of cursorUp,
    cursorDown, cursorLeft or cursorRight and if the cursor will not go outside
    of the view port. */
bool updateCursorPos
(const yx chunkSize, editingState & edState);
/* Once entered this function will not exit untill one of editChars::quit or
   editChars::toggleHelpMsg is pressed.  */
void printEditHelp(const yx viewPortSize, editingState & edState);
void printCurrentSlice
(sprite<spriteCharData, yHeight, xWidth> & spriteObj, const yx viewPortSize,
 editingState & edState);


void editMode
(const std::string spriteFileName, const yx chunkSize)
{
    sprite<spriteCharData, yHeight, xWidth> spriteObj
      {editingSettings::showLastChunkAfterUpdateIndexFor};

    bool foundSpriteFile {false};
    
    // readInSpriteFile(spriteFileName, spriteObj.advanceBeforeModify().data,
    // 		     chunkSize, foundSpriteFile);
    if(!foundSpriteFile)
      {
      // 	      // Chunk coord not found i.e. file not found. So initialise chunk.
      // fillChunk(bgChunk.advanceBeforeModify().data,
      // 		chunkSize, (int)editingSettings::invalidCharColorPair,
      // 		[](auto & element, const int filler)
      // 		{
      // 		  element.ch = filler;
      // 		});
      }

    editModeProper(spriteFileName, spriteObj, chunkSize);
}


// void readInSpriteFile
// (const std::string fileName, sprite<int, yHeight, xWidth> & spriteObj,
//  const yx chunkSize, bool & foundSpriteFile)
// {
// }


void editModeProper
(const std::string & spriteFileName,
 sprite<spriteCharData, yHeight, xWidth> & spriteObj,
 const yx chunkSize)
{  
  editingState edState
    {
      true,
      std::chrono::steady_clock::now(),
      ERR, 	        // This is returned by getch() when there is no input.
      yx{chunkSize.y / 2, chunkSize.x / 2},
      ' ',
    };

    while(!confirmQuit(chunkSize, edState.input,
		     editingSettings::editChars::quit))
      {
	edState.input = getch();

	// Act on input.
	actOnInputLayer1(spriteFileName, spriteObj, chunkSize, edState);

	printCurrentSlice(spriteObj, chunkSize, edState);

	// printCursor(edState.cursorPos, edState.getCurrentBgChar());
	spriteObj.printUndoRedoBufferIndexIfChanged(chunkSize.y -1, 0);

	edState.printCursor();

	refresh();
	sleep(editingSettings::loopSleepTimeMs);
      }
}


void actOnInputLayer1
(const std::string & spriteFileName,
 sprite<spriteCharData, yHeight, xWidth> & spriteObj,
 const yx chunkSize, editingState & edState)
{ 
  if(!updateCursorPos(chunkSize, edState))
    {
      switch(edState.input)
	{
	case editingSettings::editChars::toggleHelpMsg:
	  printEditHelp(chunkSize, edState);
	case editingSettings::editChars::performActionAtPos:
	  spriteObj.advanceBeforeModify
	    (edState.cursorPos.y, edState.cursorPos.x,
	     edState.getCurrentSpriteChar());
	    ;
	  break;
	// default:
	  // if(!edState.refrenceChunkToggle)
	  //   {
	  //     actOnInputLayer2(bgChunkFileName, cRChunkFileName, chunkCoord, bgChunk,
	  // 		       cRChunk, chunkSize, edState);
	  //   }
	};
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
      "\t\t\t"),
     viewPortSize, printCharSpeed, afterPrintSleepMedium, false, false);

  edState.input = getch();  
  while(edState.input != quit &&
	edState.input != toggleHelpMsg)
    {
      edState.input = getch();
      sleep(editingSettings::loopSleepTimeMs);
    }

  safeScreenExit(edState.input, editingSettings::editSubMenuSleepTimeMs);
}


void printCurrentSlice
(sprite<spriteCharData, yHeight, xWidth> & spriteObj, const yx viewPortSize,
 editingState & edState)
{
  auto getColorForUnsetChars = [viewPortSize, & edState]()
    {
      if(std::chrono::steady_clock::now() -
	 edState.randomColorPairForSlicePrintTimeLast >
	 editingSettings::colorPairForSlicePrintDisplayTime)
	{
	  edState.randomColorPairForSlicePrint = getRandomColor();
	  edState.randomColorPairForSlicePrintTimeLast =
	    std::chrono::steady_clock::now();
	}
    };

  getColorForUnsetChars();
  
  for(int yIter {}; yIter < viewPortSize.y; yIter++)
    {
      for(int xIter {}; xIter < viewPortSize.x; xIter++)
	{
          if (spriteObj(yIter, xIter).set == true)
	    {
	      int ch;
	      bool foundACSCode;
	      
	      setColorFromChar(spriteObj(yIter, xIter).ch);
	      ch = getChar(spriteObj(yIter, xIter).ch, foundACSCode);

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
		(edState.randomColorPairForSlicePrint);
	      mvprintw(yIter, xIter, " ");
	    }
	}
    }
}
