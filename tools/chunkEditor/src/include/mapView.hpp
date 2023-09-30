#ifndef MAP_VIEW_HPP__
#define MAP_VIEW_HPP__


#include "utils.hpp"
#include "colorS.hpp"


namespace mapViewSettings
{
  constexpr int loopSleepTimeMs		{5};
  extern setColorMode colorMode;
  constexpr int cursorColor		{helpColorPair};
  constexpr char chunkRepChar   	{'M'};
  constexpr int chunkRepCharColor	{86}; // 72?
  
  namespace mapViewChars
  {
    constexpr char quit			{'q'};
    constexpr char cursorUp		{'w'};
    constexpr char cursorDown		{'s'};
    constexpr char cursorLeft		{'a'};
    constexpr char cursorRight		{'d'};
    constexpr char actionAtPos		{' '};
    constexpr char listMapCoords	{'l'};
  }
}


void mapView(const std::string multiChunkFileName, const yx viewPortSize);


#endif
