#ifndef BACKGROUND_H_
#define BACKGROUND_H_


#include <vector>


class backgroundData
{
private:
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx maxyx;
  
public:
  const int linearChunkSize;	// Number of characters in a chunk.
  std::vector<std::vector<std::array<int, linearChunkSize>>> backgroundChunks;

  backgroundData(const yx maxyx): linearChunkSize(yx.y * yx.x)
  {
    this->yx.y = yx.y;
    this->yx.x = yx.x;
  }

  std::array<int, linearChunkSize> & getChunkFromIndex(const yx yxIndex)
  {
    return getChunkFromIndex(yx.y, yx.x);
  }

  std::array<int, linearChunkSize> & getChunkFromIndex(const int y, const int x)
  {
    // Return backgroundChunks[y / maxyx, x / maxyx] (or something like this?)
  }

  // More functions to get chunks above, below and on either size of chunk
  // coordinate falls in...
} 


#endif
