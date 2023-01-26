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
  typedef std::vector<unsigned short> backgroundChunk;
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx maxyx;
  
private:
  /* Concatenate (y / maxyx.y) and (x / maxyx.y) and use as index into map.
     Then (y % (maxyx.y * 3)) * maxyx.x + (x % (maxyx.x * 3)) can be used to
     index into the backgroundChunk returned from the map (as the stage 1 draw
     buffer will be 3 by 3 chunks.) */
  typedef std::map<std::string, backgroundChunk> backgroundType;
  const backgroundType  background;

  class firstStageDrawBufferType
  {
  public:
    // Dimensions of firstStageDrawBuffer in chunks.
    static const int fSDBYChunks {3}, fSDBXChunks {3};

    /* Holds the position of the view port (in characters) relative to the level
       origin (0,0). Note that there does not need to be a chunk at
       (0,0). However all calculations take (0,0) as the origin. Position should
       be updated indirectly by rules.physics() or a function it calls and it
       along with lastUpdatedPosition is used to calculate which chunks should
       be drawn into buffer and when. Position should also be used to draw from
       the first stage draw buffer into the second stage draw buffer if the
       first stage draw buffer has changed (which of course can be checked by
       checking if position has changed relative to last position since the last
       draw was done.) */
    yx position {};
    yx lastUpdatedPosition {};
    /* Holds a 3Y by 3X buffer of the currently visible or "close to visible"
       chunks. Where Y is the height of the view port and X it's width. */
    unsigned short * buffer;

    firstStageDrawBufferType(const ssize_t bufferSize) :
      buffer(new unsigned short [bufferSize])
    {
    }

    ~firstStageDrawBufferType()
    {
      delete [] buffer;
    }
  };
  
  firstStageDrawBufferType firstStageDrawBuffer;
  
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
  // /* First checks if buffPos is in range. Returns false if it is not. Otherwise
  //    attempts to read the coordinates into chunkCoord. If this succeeds returns
  //    true (with chunkCoord being set to the coordinates read.) If there is a
  //    failure in reading the coordinates then the program will be aborted with eMsg
  //    being displayed. */
  // bool getChunkCoordinate(const std::string & bgData,
  // 			  std::string::const_iterator & buffPos,
  // 			  const std::string & eMsg, yx & chunkCoord) const;
  /* Get's chunk from bgData and stores in chunk. Prints eMsg and exits if chunk
     cannot successfully be read in. This function is used during object
     initialisation. */
  // void getChunk(const std::string & bgData,
  // 		std::string::const_iterator & buffPos, const std::string & eMsg,
  // 		std::string & chunk) const;
  /* Prints eMsg and terminates program if rawChunk is the wrong size. */
  void verifyCollapsedChunkSize(const backgroundChunk & rawChunk,
				const ssize_t chunksReadIn,
				const bool attemptedCompression) const;
  
public:
  const char * fileName;
  
  backgroundData(const yx maxyxIn, const char bgFileName []):
    maxyx (maxyxIn.y, maxyxIn.x),
    background(loadAndInitialiseBackground(bgFileName)),
    fileName(bgFileName), firstStageDrawBuffer
    (firstStageDrawBufferType::fSDBYChunks *
     firstStageDrawBufferType::fSDBXChunks * maxyxIn.y * maxyxIn.x)
  {
  }

  ssize_t numberOfChunks() const
  {
    return background.size();
  }


  bool keyExists(const std::string & key) const
  {
    return !(background.find(key) == background.end());
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
  //

  /* Updates the first stage draw buffer if firstStageDrawBuffer.position and
     firstStageDrawBuffer.lastUpdatedPosition have diverged by a sufficient
     delta. If an update is performed lastUpdatedPosition is set to the same
     values as position. */
  void updateFirstStageDrawBuffer();
};


#endif
