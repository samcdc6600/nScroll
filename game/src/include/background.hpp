#ifndef BACKGROUND_H_
#define BACKGROUND_H_


#include <vector>
#include <string>
#include <map>
#include "utils.hpp"
  

class backgroundData
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  typedef std::vector<int> backgroundChunk;
  /* Concatenate (y / maxyx.y) and (x / maxyx.y) and use as index into map.
     Then (y % (maxyx.y * 3)) * maxyx.x + (x % (maxyx.x * 3)) can be used to
     index into the backgroundChunk returned from the map (as the stage 1 draw
     buffer will be 3 by 3 chunks.) */
  typedef std::map<std::string, backgroundChunk> backgroundType;
  // const int linearChunkSize;	// Number of characters in a chunk.
  
private:
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx maxyx;
  bool backgroundModifiable {true};
  const backgroundType background;
  /* Code for value of transparent space is 1 higher then 159 * 64 which is
     above the space of all ACS and ASCII characters whether coloured or not. */
  static constexpr int TRANS_SP {10176};


  /* First checks if buffPos is in range. Returns false if it is not. Otherwise
   attempts to read the coordinates into chunkCoord. If this succeeds returns
   true (with chunkCoord being set to the coordinates read.) If there is a
   failure in reading the coordinates then the program will be aborted with eMsg
   being displayed. */
  bool getChunkCoordinate(const std::string & bgData,
			  std::string::const_iterator & buffPos,
			  const std::string & eMsg, yx & chunkCoord);

  void getChunk(const std::string & bgData,
		std::string::const_iterator & buffPos, const std::string & eMsg,
		std::string & chunk);

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
  
public:
  backgroundData(const yx maxyxIn): maxyx(maxyxIn.y, maxyxIn.x)/*,
								 linearChunkSize(maxyx.y * maxyx.x)*/
  {
  }
  
  void initialiseBackgroundData(const bool rawData, const char bgFileName [],
				const std::string & bgData);

  // std::vector<int> & getChunkFromCoord(const yx coord)
  // {
  //   return getChunkFromCoord(coord.y, coord.x);
  // }

  // std::vector<int> & getChunkFromCoord(const int y, const int x)
  // {
  //   // Return backgroundChunks[y / maxyx, x / maxyx] (or something like this?)
  //   if(background.size)
  //     {
  //     }
  // }
};


#endif