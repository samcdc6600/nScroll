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
(const std::string multiChunkFileName, bool viewingBgMap,
 const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize);
/* Updates cursorPos as long as at least one chunk representing character is in
   the view port. Returns true if cursorPos has been updated. */
bool updateCursorPos
(const int input, yx & cursorPos, const yx minMaxY, const yx minMaxX,
 const yx viewPortSize);
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
	  bgMap.push_back(bgChunkStrt{mapChunk});
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

  viewMap
    (multiChunkFileName, viewingBgMap, mapCoords, bgMap, cRMap, viewPortSize);
}


void viewMap
(const std::string multiChunkFileName, bool viewingBgMap,
 const std::vector<yx> & mapCoords,
 const std::vector<bgChunkStrt> & bgMap, const std::vector<cRChunkStrt> & cRMap,
 const yx viewPortSize)
{
  auto dispCurrentChunkPos =
    [](const yx cursorPos, const yx viewPortSize)
    {
      mapViewSettings::colorMode.setColor(mapViewSettings::cursorColor);
      mvprintw(0,0, concat("Current chunk: ", cursorPos).c_str());
    };

  // Min values are .y and max are .x.
  yx minMaxY {}, minMaxX {};
  getMinMaxYAndX(mapCoords, minMaxY, minMaxX);
  yx cursorPos {(minMaxY.y + minMaxY.x) / 2, (minMaxX.y + minMaxX.x) / 2};
  int input {ERR};	   // ERR is returned by getch() when there is no input.
  
  while(!confirmQuit(viewPortSize, input, mapViewSettings::mapViewChars::quit))
    {
      input = getch();

      if(!updateCursorPos(input, cursorPos, minMaxY, minMaxX, viewPortSize))
	{
	  switch(input)
	    {
	      /* CASE SPACE:
		 If the cursor is over a chunk representing character enter display
		 chunk mode (which can be exited by pressing space again.) */
	    }
	}

      clear();			// Clear the screen.
      dispCurrentChunkPos(cursorPos, viewPortSize);
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


void displayMap
(const std::vector<yx> & mapCoords, const yx cursorPos, const yx minMaxY,
 const yx minMaxX, const yx viewPortSize)
{
  mapViewSettings::colorMode.setColor(mapViewSettings::chunkRepCharColor);
  
  for(const yx chunkCoord: mapCoords)
    {
      if(abs(minMaxY.y) + chunkCoord.y >= abs(minMaxY.y) + cursorPos.y - (viewPortSize.y / 2) &&
	 abs(minMaxY.y) + chunkCoord.y <= abs(minMaxY.y) + cursorPos.y + (viewPortSize.y / 2) &&
	 abs(minMaxX.y) + chunkCoord.x >= abs(minMaxX.y) + cursorPos.x - (viewPortSize.x / 2) &&
	 abs(minMaxX.y) + chunkCoord.x <= abs(minMaxX.y) + cursorPos.x + (viewPortSize.x / 2))
	{
	  /* CursorPos is always at the centre of the screen. */
	  // yx printPos {chunkCoord.y + abs(cursorPos.y - (viewPortSize.y / 2)),
	  // 	       chunkCoord.x + abs(cursorPos.x - (viewPortSize.x / 2))};
	  /* If X in abs(X) is  */
	  mvprintw(chunkCoord.y - (cursorPos.y - (viewPortSize.y / 2)), chunkCoord.x - (cursorPos.x - (viewPortSize.x / 2)),
		   concat("", mapViewSettings::chunkRepChar).c_str());

	  if(chunkCoord.y == 10 && chunkCoord.x == 10)
	    {
	      	    mvprintw(yx{1, 0},
		     concat
			     ("", yx{cursorPos.y, cursorPos.x}));
	    mvprintw(yx{2, 0},
		     concat
		     ("", yx{abs(cursorPos.y - (viewPortSize.y / 2)), abs(cursorPos.x - (viewPortSize.x / 2))}));
	  mvprintw(yx{3, 0},
		   concat
		   ("", yx{chunkCoord.y + abs(cursorPos.y - (viewPortSize.y / 2)), chunkCoord.x + abs(cursorPos.x - (viewPortSize.x / 2))}));
		   }
	}
    }
}
