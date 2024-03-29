#ifndef MAP_VIEW_HPP__
#define MAP_VIEW_HPP__


#include "utils.hpp"
#include "colorS.hpp"


namespace mapViewSettings
{
  constexpr int loopSleepTimeMs		{5};
  constexpr int mapViewSubMenuSleepTimeMs {subMenuSleepTimeMs};
  extern setColorMode colorMode;
  constexpr int cursorColor		{helpColorPair};
  constexpr int helpColor		{helpColorPair};
  constexpr char chunkRepChar   	{'M'};
  constexpr int chunkRepCharColor	{86}; // 72?
  
  namespace mapViewChars
  {
    constexpr char quit			{'q'};
    constexpr char toggleHelpMsg       	{'h'};
    constexpr char cursorUp		{'w'};
    constexpr char cursorDown		{'s'};
    constexpr char cursorLeft		{'a'};
    constexpr char cursorRight		{'d'};
    constexpr char actionAtPos		{' '};
    constexpr char listMapCoords	{'l'};
    /* Cycles through map chunks, setting the cursor position to successive
       chunks. */
    constexpr char nextChunk		{'n'};
  }
}


void mapView(const std::string multiChunkFileName, const yx viewPortSize);


#endif
