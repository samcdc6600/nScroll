#include <fstream>
#include "include/mapView.hpp"


namespace mapViewSettings
{
  setColorMode colorMode {};
}


/* Shows either bgMap or cRMap on the screen (depending on the value of
   viewingBgMap) as a character for each chunk. If the displayed map is larger
   than the view port the user can pan with WASD. If the user moves the cursor
   over a chunk the chunk coord will be displayed in a corner of the view
   port. If the user pressed space when the cursor is over a chunk the chunk
   will be displayed until the user presses space again. */
void viewMap
(const std::string & multiChunkFileName, bool viewingBgMap,
 const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize);
/* Updates cursorPos as long as at least one chunk representing character is in
   the view port. Returns true if cursorPos has been updated. */
bool updateCursorPos
(const int input, yx & cursorPos, const yx minMaxY, const yx minMaxX,
 const yx viewPortSize);
void printMapViewHelp(const yx viewPortSize);
void viewChunk
(bool viewingBgMap, yx & cursorPos, const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize);
/* Lists all coordinates in the map. Prompts the user first if they would like
   to as this function will exit the program after listing the coordinates. */
void listMapCoordsFunc
(const std::vector<yx> & mapCoords, const std::string & fileName,
 const yx viewPortSize);
/* Returns the coord at the index after chunkIndex in mapCoord. Wrapps
   around if chunkIndex is the last index in mapCoords. ChunkIndex is updated to
   the index of the coord returned. */
yx getNextChunkPos(const std::vector<yx> & mapCoords, int & chunkIndex);
void displayMap
(const std::vector<yx> & mapCoords, const yx cursorPos, const yx minMaxY,
 const yx minMaxX, const yx viewPortSize);


void mapView(const std::string multiChunkFileName, const yx viewPortSize)
{
  std::fstream mapFile(multiChunkFileName, std::ios::in | std::ios::binary);

  if(!mapFile)
    {
      exit(concat("Error: failed to open source file \"", multiChunkFileName,
		  "\" when attempting to view in map mode."),
	   ERROR_OPENING_FILE);
    }
  else if(mapFile.peek() == std::ifstream::traits_type::eof())
    {
      exit(concat("Error: file \"", multiChunkFileName,
		  "\" found to be empty when attempting to view it in map "
		  "mode."), ERROR_MALFORMED_FILE);
    }

  bool viewingBgMap {false};		// Controls chunk display function.
  // Only one of these is populated.
  std::vector<bgChunkStrt>	bgMap;
  std::vector<cRChunkStrt>	cRMap;
  std::vector<yx>		mapCoords;
  const std::string noChunksEMsg
    {concat("Error: unable to read any chunks from file \"", multiChunkFileName,
	    "\" when attempting to view it in map mode (note that the file "
	    "isn't empty!) ")};
  
  if(checkForPostfix(multiChunkFileName.c_str(), BACKGROUND_FILE_EXTENSION))
    {
      // We're dealing with a background file.
      viewingBgMap = true;
      bgChunk mapChunk;
      yx mapChunkCoord;
	
      while(getBgChunk(mapFile, mapChunk, viewPortSize, mapChunkCoord,
		       multiChunkFileName, true))
	{
	  mapCoords.push_back(mapChunkCoord);
	  bgMap.push_back(mapChunk);
	}
      if(bgMap.size() == 0)
	{
	  exit(noChunksEMsg, ERROR_MALFORMED_FILE);
	}
    }
  else
    {
      // We're dealing with a character rules file.
      cRChunk mapChunk;
      yx mapChunkCoord;

      while(getCRChunk(mapFile, mapChunk, viewPortSize, mapChunkCoord,
		       multiChunkFileName, true))
	{
	  mapCoords.push_back(mapChunkCoord);
	  cRMap.push_back(cRChunkStrt{mapChunk});
	}
      if(cRMap.size() == 0)
	{
	  exit(noChunksEMsg, ERROR_MALFORMED_FILE);
	}
    }

  mapFile.close();
  viewMap
    (multiChunkFileName, viewingBgMap, mapCoords, bgMap, cRMap, viewPortSize);
}


void viewMap
(const std::string & multiChunkFileName, bool viewingBgMap,
 const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize)
{
  auto dispCurrentChunkPos =
    [](const yx cursorPos)
    {
      mapViewSettings::colorMode.setColor(mapViewSettings::cursorColor);
      mvprintw(0,0, concat("Current chunk: ", cursorPos).c_str());
    };

  // Min values are .y and max are .x.
  yx minMaxY {}, minMaxX {};
  getMinMaxYAndX(mapCoords, minMaxY, minMaxX);
  yx cursorPos {(minMaxY.y + minMaxY.x) / 2, (minMaxX.y + minMaxX.x) / 2};
  int input {ERR};	   // ERR is returned by getch() when there is no input.
  /* Used for cycling through chunks. (They annoying be hard to locate if they
     aren't connected.) MapCoords should always be > 0 by this point. */
  int lastChunkIndex {int(mapCoords.size() -1)};
  
  while(!confirmQuit(viewPortSize, input, mapViewSettings::mapViewChars::quit))
    {
      input = getch();

      if(!updateCursorPos(input, cursorPos, minMaxY, minMaxX, viewPortSize))
	{
	  using namespace mapViewSettings::mapViewChars;
	  switch(input)
	    {
	    case toggleHelpMsg:
	      printMapViewHelp(viewPortSize);
	      break;
	    case actionAtPos:
	      curs_set(0);	// Turn off cursor visibility.
	      /* If the cursor is over a chunk representing character enter
		 display chunk mode (which can be exited by pressing space
		 again.) */
	      viewChunk(viewingBgMap, cursorPos, mapCoords, bgMap, cRMap,
			viewPortSize);
	      curs_set(1);	// Turn cursor visibility back on.
	    break;
	    case listMapCoords:
	      listMapCoordsFunc(mapCoords, multiChunkFileName, viewPortSize);
	      break;
	    case nextChunk:
	      cursorPos = getNextChunkPos(mapCoords, lastChunkIndex);
	      break;
	    }
	}

      clear();			// Clear the screen.
      dispCurrentChunkPos(cursorPos);
      displayMap(mapCoords, cursorPos, minMaxY, minMaxX, viewPortSize);
      /* The cursor is always displayed in the centre of the screen as the map
	 moves around it. */
      move(viewPortSize.y / 2, viewPortSize.x / 2);
      
      sleep(mapViewSettings::loopSleepTimeMs);
    }
}


bool updateCursorPos
(const int input, yx & cursorPos, const yx minMaxY, const yx minMaxX,
 const yx viewPortSize)
{
  using namespace mapViewSettings::mapViewChars;
  
  bool ret {false};
  yx newPos {cursorPos};

  switch(input)
    {
    case cursorUp:
      newPos.y--;
      if(newPos.y > (minMaxY.y - viewPortSize.y /2))
	{
	  /* Only update the cursor if it isn't outside of the minimum and
	     maximum y and x ranges. */
	  ret = true;
	}
      break;
    case cursorDown:
      newPos.y++;
      if(newPos.y <= (minMaxY.x + viewPortSize.y / 2))
	{
	  ret = true;
	}
      break;
    case cursorLeft:
      newPos.x--;
      if(newPos.x > (minMaxX.y - viewPortSize.x / 2))
	{
	  ret = true;
	}
      break;
    case cursorRight:
      newPos.x++;
      if(newPos.x <= (minMaxX.x + viewPortSize.x / 2))
	{
	  ret = true;
	}
      break;
    }

  if(ret)
    {
      cursorPos = newPos;
    }
  return ret;
}


void printMapViewHelp(const yx viewPortSize)
{
  using namespace mapViewSettings::mapViewChars;
  mapViewSettings::colorMode.setColor(mapViewSettings::helpColor);
  progressivePrintMessage
    (concat
     ("\t~H~E~L~P~!~\t\t\t\t\t\t",
      toggleHelpMsg,	": to toggle this message.\t\t\t",
      quit,		": to exit the program.\t\t\t",
      cursorUp,		": to move the cursor up.\t\t\t",
      cursorDown,	": to move the cursor down.\t\t\t",
      cursorLeft,	": to move the cursor left.\t\t\t",
      cursorRight,	": to move the cursor right.\t\t\t"
      "Note that the cursor always stays in the middle of the screen and as "
      "such moving the cursor has the effect of moving the map around the "
      "cursor. ",
      "\"", actionAtPos, "\": to toggle chunk view mode. If the cursor is over "
      "a map chunk the chunk will be displayed (in a read only mode.)\t\t\t",
      listMapCoords,	": to display the coordinates of all chunks in the "
      "map file (note that this will also exit the program.)\t\t\t",
      nextChunk,	": to move the cursor to the next map chunk. "
      "Essentially cycles through chunk positions (this makes it easy to visit "
      "any disconnected chunks and to visit all chunks in general in an "
      "ordered sequence (the order they are stored in the map file.))\t\t\t"),
     viewPortSize, printCharSpeed, afterPrintSleepMedium, false, false);

  int input {getch()};  
  while(input != quit &&
	input != toggleHelpMsg)
    {
      input = getch();
      sleep(mapViewSettings::loopSleepTimeMs);
    }

  safeScreenExit(input, mapViewSettings::mapViewSubMenuSleepTimeMs);
}


void viewChunk
(bool viewingBgMap, yx & cursorPos, const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize)
{
  auto printMapViewHelpLocal = [viewPortSize](const int input)
  {
    if(input == mapViewSettings::mapViewChars::toggleHelpMsg)
      {
	printMapViewHelp(viewPortSize);
      }
  };
  
  bool foundCoord {false};

  // for(const yx & coord: mapCoords)
  int mapCoordIndex {};
  for( ; (size_t)mapCoordIndex < mapCoords.size(); mapCoordIndex++)
    {
      if(cursorPos == mapCoords[mapCoordIndex])
	{
	  foundCoord = true;
	  break;
	}
    }

  if(foundCoord)
    {
      int input {};
      if(viewingBgMap)
	{
	  while((input = getch()) != mapViewSettings::mapViewChars::actionAtPos
		&& input != mapViewSettings::mapViewChars::quit)
	    {
	      // MapCoords and (bgMap or cRMap) should always be the same size.
	      printBgChunk(bgMap[mapCoordIndex].chunk, viewPortSize);
	      printMapViewHelpLocal(input);
	      sleep(mapViewSettings::loopSleepTimeMs);
	    }
	}
      else
	{
	  while((input = getch()) != mapViewSettings::mapViewChars::actionAtPos
		&& input != mapViewSettings::mapViewChars::quit)
	    {
	      // MapCoords and (bgMap or cRMap) should always be the same size.
	      printCRChunk(cRMap[mapCoordIndex].chunk, viewPortSize);
	      printMapViewHelpLocal(input);
	      sleep(mapViewSettings::loopSleepTimeMs);
	    }
	}
    }
  else
    {
      mapViewSettings::colorMode.setColor(warningColor);
      progressivePrintMessage
	(concat("\tError no chunk at current coordinate ", cursorPos, ".\t"),
	 viewPortSize, printCharSpeed, afterPrintSleep);
    }
}


void listMapCoordsFunc
(const std::vector<yx> & mapCoords, const std::string & fileName,
 const yx viewPortSize)
{
  int input;
  if(getConfirmation
     (viewPortSize, helpColorPair, input,
      mapViewSettings::mapViewSubMenuSleepTimeMs,
      concat("\tDo you really want to display all chunk coordinates in this map "
	     "file (\"", fileName, "\") (note that the program will exit after "
	     "displaying the list) y/n?\t")))
    {
      endwin();
      for(const yx & coord: mapCoords)
	{
	  printMessageNoWin(concat("", coord), afterPrintSleepFast,
			    afterPrintSleep);
	}
      printMessageNoWin("\nGoodbye :)\n", printCharSpeed,
			afterPrintSleep);
      exit(-1);
    }
}


yx getNextChunkPos(const std::vector<yx> & mapCoords, int & chunkIndex)
{
  if((size_t)(++chunkIndex) > (mapCoords.size() -1))
    {
      chunkIndex = 0;
    }

    return mapCoords[chunkIndex];
}


void displayMap
(const std::vector<yx> & mapCoords, const yx cursorPos, const yx minMaxY,
 const yx minMaxX, const yx viewPortSize)
{
  mapViewSettings::colorMode.setColor(mapViewSettings::chunkRepCharColor);
  
  for(const yx & chunkCoord: mapCoords)
    {
      if(abs(minMaxY.y) + chunkCoord.y >=
	 abs(minMaxY.y) + cursorPos.y - (viewPortSize.y / 2) &&
	 abs(minMaxY.y) + chunkCoord.y <=
	 abs(minMaxY.y) + cursorPos.y + (viewPortSize.y / 2) &&
	 abs(minMaxX.y) + chunkCoord.x >=
	 abs(minMaxX.y) + cursorPos.x - (viewPortSize.x / 2) &&
	 abs(minMaxX.y) + chunkCoord.x <=
	 abs(minMaxX.y) + cursorPos.x + (viewPortSize.x / 2))
	{
	  /* CursorPos is always at the centre of the screen. */
	  mvprintw
	    (chunkCoord.y - (cursorPos.y - (viewPortSize.y / 2)),
	     chunkCoord.x - (cursorPos.x - (viewPortSize.x / 2)),
	     concat("", mapViewSettings::chunkRepChar).c_str());
	}
    }
}
