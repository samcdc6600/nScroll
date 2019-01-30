#include "checkBoundsOfBounds.h"


/* should return an integer that represents the difference in "size" between the background file and
the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background)
{  
  return boarder.size() - background.size();
}
