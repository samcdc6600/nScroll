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
  char input;
  yx cursorPos;
  // The bg char that will be saved to bgChunk if we press space.
  int currentBgChar;
  char currentCRChar;
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
void printCharacterSelection();
/* Once entered this function will not exit untill one of editChars::quit or
   editChars::toggleHelpMsg is pressed.  */
void printEditHelp(const yx viewPortSize, editingState & edState);
void printBgChunk(const backgroundChunkCharInfo bgChunk[][xWidth], const yx viewPortSize);
void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize);
void printCursorForCREditMode
(const char cRChunk[][xWidth], const yx cursorPos, const char currentCRChar);
void printCursorForBgEditMode
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx cursorPos, const int currentBgChar);
void printACS(const yx position, const int aCSChar);
void printACS(const int aCSChar);
void setColorFromChar(const int ch);
bool charInColorCharRange(const int ch);
int getColor(const int ch);
/* Removes color info from ch. If ch is an aCS character sets aCS to true,
   otherwise sets aCS to false. Prints an error and exits if ch is out of
   range. */
int getChar(const int ch, bool &aCS);
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
	  printCursorForCREditMode(cRChunk, edState.cursorPos, edState.currentCRChar);
	}
      else
	{
	  printCursorForBgEditMode(bgChunk, edState.cursorPos, edState.currentBgChar);
	}

      edState.cursorVisibilityChangeTimeLast =
	setCursorVisibility(edState.cursorOn, edState.cursorVisibilityChangeTimeLast);
      
      sleep(editingSettings::loopSleepTimeMs);
    }
}


void actOnInput
(backgroundChunkCharInfo bgChunk[][xWidth], char cRChunk[][xWidth], const yx chunkSize,
 editingState & edState)
{
  using namespace editingSettings::editChars;
  
  switch(edState.input)
    {
    case cursorUp:
      if(edState.cursorPos.y > 0)
	{
	  edState.cursorPos.y--;
	  move(edState.cursorPos);
	}
      break;
    case cursorDown:
      if(edState.cursorPos.y < chunkSize.y -1)
	{
	  edState.cursorPos.y++;
	  move(edState.cursorPos);
	}
      break;
    case cursorLeft:
      if(edState.cursorPos.x > 0)
	{
	  edState.cursorPos.x--;
	  move(edState.cursorPos);
	}
      break;
    case cursorRight:
      if(edState.cursorPos.x < chunkSize.x -1)
	{
	  edState.cursorPos.x++;
	  move(edState.cursorPos);
	}
      break;
    case setCharacterAtPos:
      if(edState.cRChunkToggle)
	{
	  cRChunk[edState.cursorPos.y][edState.cursorPos.x] = edState.currentCRChar;
	}
      else
	{
	}
      break;
    case toggleBetweenCRandBg:
      edState.cRChunkToggle = !edState.cRChunkToggle;
      break;
    case eraseCRChar:
      edState.currentCRChar = editingSettings::rulesChars::nullRule;
      break;
    case setCRCharToBorder:
      edState.currentCRChar = editingSettings::rulesChars::boarder;
      break;
    case setCRCharToPlatform:
      edState.currentCRChar = editingSettings::rulesChars::platform;
      break;
    case toggleCharacterSelection:
      
      break;
    case toggleHelpMsg:
      printEditHelp(chunkSize, edState);
    }
}


void printEditHelp(const yx viewPortSize, editingState & edState)
{
  using namespace editingSettings::editChars;
  
  editingSettings::colorMode.setColor(1);
  progressivePrintMessage
    (concat
     ("~H~E~L~P~!~\t\t\t\t\t\t",
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
		  mvprintw(yIter, xIter, concat("", ch).c_str());
		}
	    }
	  else
	    {
	      editingSettings::colorMode.setColor(colorParams::noCharColorPair);
	      mvprintw(yIter, xIter, " ");
	    }
	  // foundAcsCode = getContiguouslyColordString
	  //   (bgChunk, yx{yIter, xIter}, viewPortSize,
	  //    contiguousColorChars, acsCode);

	  
	  // if(contiguousColorChars.size() != 0)
	  //   {
	  //     // Print ASCII chars.
	  //     printw(contiguousColorChars.c_str());
	  //   }
	  // if(foundAcsCode)
	  //   {
	  //     // Print ACS char.
	  //     printAcs(acsCode, false);
	  //   }

	  // contiguousColorChars.clear();
	}
    }
}


void printCRChunk(const char cRChunk[][xWidth], const yx viewPortSize)
{
  setColorMode setCuMode {};
  
  
  for(int yIter {}; yIter < viewPortSize.y; ++yIter)
    {
      for(int xIter {}; xIter < viewPortSize.x; ++xIter)
	{
	  if(cRChunk[yIter][xIter] != ' ')
	    {
	      setCuMode.setColor
		(abs(rand() % editingSettings::gameColorPairsNo) +1);
	      mvprintw(yIter, xIter, concat("", cRChunk[yIter][xIter]).c_str());
	    }
	}
    }
}


void printCursorForCREditMode
(const char cRChunk[][xWidth], const yx cursorPos, const char currentCRChar)
{
  // TMP (We want to set the cursor colour to the opposite of the bg
  // colour. No matter which chunk editing mode we are in.)
  editingSettings::colorMode.setColor(colorParams::noCharColorPair);
  mvprintw(cursorPos, concat("", currentCRChar).c_str());
  move(cursorPos);
}


void printCursorForBgEditMode
(const backgroundChunkCharInfo bgChunk[][xWidth], const yx cursorPos, const int currentBgChar)
{
  bool aCSChar {false};
  /* Set cursor colour to opposite of bg color (maybe, we aren't doing
     it properly here.) at cursorPos. */
  setColorFromChar
    ((bgChunk[cursorPos.y][cursorPos.x].ch +
	       (colorParams::gameColorPairsNo / 2)) %
	      colorParams::gameColorPairsNo);
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


int getColor(const int ch)
{
  /* A character is encoded as it's basic value + it's color value -1 times
     maxCharNum. Since integer division rounds down ch / maxCharNum should give
     the color code. less 1. */
  int color {(ch / maxCharNum) +1};
  if(color > colorParams::gameColorPairsNo)
    {
      exit(concat("Error (in getColor()): encountered colour (", color, ") "
		  "code that is out of range.\n"),
	   ERROR_COLOR_CODE_RANGE);
    }

  return color;
}



int getChar(const int ch, bool & aCS)
{
  int rawCh;
  aCS = false;
  
  // Remove colour information (first color starts at 1, so we remove 1.)
  rawCh -= ((getColor(rawCh) -1) * maxCharNum);

  if(rawCh > ASCII_CH_MAX && rawCh <= maxCharNum)
    {
      // We have an ACS char.
      aCS = true;
    }
  else if(rawCh < 0 || rawCh > ASCII_CH_MAX)
    {
      exit(concat("Error: encountered character (", (int)rawCh, ") that is lass"
		  "than 0 or greater than ", maxCharNum,". after having color "
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
