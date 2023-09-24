#ifndef MAP_VIEW_HPP__
#define MAP_VIEW_HPP__


#include "utils.hpp"


namespace mapViewSettings
{
  constexpr int loopSleepTimeMs {5};
  
  namespace mapViewChars
  {
    constexpr char quit			{'q'};
  }
}


void mapView(const std::string multiChunkFileName, const yx viewPortSize);
/* Shows either bgMap or cRMap on the screen (depending on the value of
   viewingBgMap) as a character for each chunk. If the displayed map is larger
   than the view port the user can pan with WASD. If the user moves the cursor
   over a chunk the chunk coord will be displayed in a corner of the view
   port. If the user pressed space when the cursor is over a chunk the chunk
   will be displayed until the user presses space again. */
void viewMap
(const std::string multiChunkFileName, bool viewingBgMap,
 std::vector<yx> mapCoords,
 std::vector<bgChunkStrt> bgMap, std::vector<cRChunkStrt> cRMap,
 const yx viewPortSize);


#endif
