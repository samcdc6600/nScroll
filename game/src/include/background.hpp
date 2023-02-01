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
  
private:
  /* Concatenate (y / chunkSize.y) and (x / chunkSize.y) and use as index into
     map. Then (y % (chunkSize.y * fSDBYChunks)) * chunkSize.x + (x %
     (chunkSize.x * fSDBXChunks)) can be used to index into the backgroundChunk
     returned from the map (as the stage 1 draw buffer will be fSDBYChunks by
     fSDBXChunks chunks.) */
  typedef std::map<std::string, backgroundChunk> backgroundType;
  const backgroundType  background;

  class firstStageDrawBufferType
  {
  public:
    // Dimensions of firstStageDrawBuffer in chunks.
    static const int fSDBYChunks {5}, fSDBXChunks {5};
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
  /* Updates the first stage draw buffer if viewPortPosition and
     firstStageDrawBuffer.lastUpdatedPosition have diverged by a sufficient
     delta. If an update is performed lastUpdatedPosition is set to the same
     values as position. Where viewPortPosition is position of the view port
     that is calculated from the player position (the player can move around
     some amount in the centre of the view port without moving the view port if
     the player doesn't go to close to any edge of the view port.) */
  void updateFirstStageDrawBuffer(const yx viewPortPosition);
};


#endif
