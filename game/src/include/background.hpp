#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_


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
  const yx chunkSize;
  typedef unsigned short drawBufferType;
  
private:
  /* Concatenate (y / chunkSize.y) and (x / chunkSize.x) and use as index into
     map. */
  typedef std::map<std::string, backgroundChunk> backgroundType;
  const backgroundType  background;

  class firstStageDrawBufferType
  {
  public:
    /* Dimensions of first stage draw buffer in chunks and the offsets when
       updating the first stage draw buffer. */
    static const int fSDBYChunks {5}, fSDBXChunks {5},
      fSDBYUpdateOffset {2}, fSDBXUpdateOffset {2};
    /* Holds the current position of the view port (Note that buffer may not yet
       be updated as a result this new position.) */
    yx viewPortPosition {};
    // Holds the position of the view port the last time buffer was updated.
    yx lastUpdatedPosition {};
    /* Holds a 3Y by 3X buffer of the currently visible or "close to visible"
       chunks. Where Y is the height of the view port and X it's width. */
    drawBufferType * buffer;

    firstStageDrawBufferType(const ssize_t bufferSize) :
      buffer(new drawBufferType [bufferSize])
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
  /* UpdateFirstStageDrawBuffer() will call this function (to do the actuall
     update) If horizontal is true an update will be done assuming that the
     triggering change in view port position was in the x dimension. Otherwise
     it will be done assuming that the trigger change in the view port
     position was in the y dimension. */
  void updateFirstStageDrawBuffer(const bool horizontal);
  /* Calculates the coordinates in the FSDB that a chunk should be copied to
     when the view port has moved chunkSize.x in the x dimension. Where
     yChunkOffset is used as an offset (in chunks) in the y dimension. Handles
     wrap around. I.e. if the view port position is (0,0) and the old position
     was (0, 170) (where 170 is the size of a chunk in the x dimension in this
     example) then the set of chunks to be updated given 5 calls with
     yChunkOffset being set to (0, 1, 2, 3, 4) for each respective call, should
     be (3, 3), (4, 3), (0, 3), (1, 3), (2, 3). */
  yx calculateFSDBTargetChunkWithHorizontalChange(const yx fSDBSize,
						  const int yChunkOffset) const;
  yx calculateFSDBTargetChunkWithVerticalChange(const yx fSDBSize,
						const int xChunkOffset) const;
  /* Where horizontal determines whether or not to use chunkUpdateDimensionIter
     as a component of y or x when creating the key. Horizontal should be set
     to true when the triggering change in view port position was in the x
     dimension. */
  std::string calculatePotentialChunkKeyForChunkToGoInFSDB
  (const bool horizontal, const int chunkUpdateDimensionIter) const;
  
public:
  const char * fileName;
  
  backgroundData(const yx chunkSizeIn, const char bgFileName []):
    chunkSize (chunkSizeIn.y, chunkSizeIn.x),
    background(loadAndInitialiseBackground(bgFileName)),
    firstStageDrawBuffer
    (firstStageDrawBufferType::fSDBYChunks *
     firstStageDrawBufferType::fSDBXChunks * chunkSizeIn.y * chunkSizeIn.x),
    fileName(bgFileName)
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
  //   // Return backgroundChunks[y / chunkSize, x / chunkSize] (or something like this?)
  //   if(background.size)
  //     {
  //     }
  // }
  //

  /* Should be called once initial player position is known, but before the main
     game loop. */
  void initFirstStageDrawBuffer(const yx playerPos);
  /* Updates the first stage draw buffer if
     firstStageDrawBuffer.viewPortPosition and
     firstStageDrawBuffer.lastUpdatedPosition have diverged by a sufficient
     delta. If an update is performed lastUpdatedPosition is set to the same 
     values as viewPortPosition. */
  void updateFirstStageDrawBuffer();
  /* Copies one chunk (relative to viewPortPosition) from firstStageDrawbuffer
     to secondStageDrawBuffer. */
  void updateSecondStageDrawBuffer
  (drawBufferType * secondStageDrawBuffer);
};


#endif
