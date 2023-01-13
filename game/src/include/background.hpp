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
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx maxyx;
  
private:
  /* Concatenate (y / maxyx.y) and (x / maxyx.y) and use as index into map.
     Then (y % (maxyx.y * 3)) * maxyx.x + (x % (maxyx.x * 3)) can be used to
     index into the backgroundChunk returned from the map (as the stage 1 draw
     buffer will be 3 by 3 chunks.) */
  typedef std::map<std::string, backgroundChunk> backgroundType;
  const backgroundType  background;
  /* Code for value of transparent space is 1 higher then 159 * 64 which is
     above the space of all ACS and ASCII characters whether coloured or not. */
  static constexpr int TRANS_SP {10176};

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
  backgroundType loadAndInitialiseBackground
  (const char bgFileName []) const;
  void initialiseBackground
  (const bool rawData, const char bgFileName [], const std::string & bgData,
   backgroundType & background) const;
  /* First checks if buffPos is in range. Returns false if it is not. Otherwise
     attempts to read the coordinates into chunkCoord. If this succeeds returns
     true (with chunkCoord being set to the coordinates read.) If there is a
     failure in reading the coordinates then the program will be aborted with eMsg
     being displayed. */
  bool getChunkCoordinate(const std::string & bgData,
			  std::string::const_iterator & buffPos,
			  const std::string & eMsg, yx & chunkCoord) const;
  /* Get's chunk from bgData and stores in chunk. Prints eMsg and exits if chunk
     cannot successfully be read in. This function is used during object
     initialisation. */
  void getChunk(const std::string & bgData,
		std::string::const_iterator & buffPos, const std::string & eMsg,
		std::string & chunk) const;
  /* Prints eMsg and terminates program if rawChunk is the wrong size. */
  void verifyCollapsedChunkSize(const backgroundChunk & rawChunk,
				const ssize_t chunksReadIn,
				const bool attemptedCompression) const;
  std::string inline createChunkCoordKey(const yx coord) const;
  /* Creates a key using coord and inserts rawChunk into background with that
     key. If a duplicate key is found prints error message and exit's. */
  void insertChunk
  (const yx coord, const backgroundChunk & rawChunk, const ssize_t chunksReadIn,
   const char bgFileName [], backgroundType & background) const;
  
public:
  backgroundData(const yx maxyxIn, const char bgFileName []):
    maxyx (maxyxIn.y, maxyxIn.x), background
    (loadAndInitialiseBackground(bgFileName))
  {
    /*    endwin();
    for(auto chunk: background)
      {
	std::cout<<"Chunk:\n";
	for(auto c: chunk.second)
	  {
	    std::cout<<(char)c;
	  }
	std::cout<<'\n';
	
      }
    exit(-1); */
  }
  
  /* Uses coord to create a key and searches background for that key. If it is
     found sets chunk to the address of the chunk that was found in the map
     (this should be safe since this function will print an error and exit if
     backgroundModifiable is set to true. It is set to false at the end of
     initialiseBackgroundData() and initialiseBackgroundData is the only
     function that should alter background (except for named lambda functions
     defined within it.) */
  //  bool getChunk(const yx & coord, backgroundChunk * chunk) const ;

  // bool insertChunkInto(const yx & coord, T1 & destination);
  
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
