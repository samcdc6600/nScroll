#include <fstream>
#include "include/mapView.hpp"


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
 std::vector<yx> mapCoords,
 std::vector<bgChunkStrt> bgMap, std::vector<cRChunkStrt> cRMap,
 const yx viewPortSize)
{

  // Min values are .y and max are .x.
  yx minMaxY {}, minMaxX {};
  getMinMaxYAndX(mapCoords, minMaxY, minMaxX);
  yx cursorPos {(minMaxY.y + minMaxY.x) / 2, (minMaxX.y + minMaxX.x) / 2};
  int input {};

  
  while(confirmQuit(viewPortSize, input, mapViewSettings::mapViewChars::quit))
    {
      input = getch();

      switch(input)
	{
	  /* CASE WASD:
	     move cursor (unless cursor pos will go outside of minMaxY and
	     minMaxX values.) If cursor is going to go out of view port, move
	     view port. */

	  /* CASE SPACE:
	     If the cursor is over a chunk representing character enter display
	     chunk mode (which can be exited by pressing space again.) */
	}
      
      /* Display map (map displayed depends on viewingBgMap.) */
      if(viewingBgMap)
	{
      
	}
      else
	{
	}
      /* Display coordinate of chunk cursor is over in bottom left corner
	 (unless there cursor is in the bottom left corner, in which case
	 display the coordinate in a different position.) */

      sleep(mapViewSettings::loopSleepTimeMs);
    }
}
