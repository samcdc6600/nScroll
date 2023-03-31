#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_


#include <vector>
#include <string>
#include <map>
#include "chunk.hpp"


class backgroundData: public chunk<unsigned short>
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  chunkElementBaseType * secondStageDrawBuffer;
  
private:

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
  void loadAndInitialiseBackground();
  void initialiseBackground
  (const bool rawData, const std::string & bgData,
   chunkMapType & background);
  /* Get's chunk from bgData and stores in chunk. Prints eMsg and exits if chunk
     cannot successfully be read in. This function is used during object
     initialisation. */
  /* Prints eMsg and terminates program if rawChunk is the wrong size. */
  void verifyCollapsedChunkSize(const chunkType & rawChunk,
				const ssize_t chunksReadIn,
				const bool attemptedCompression) const;
  
public:
  
  backgroundData(const yx chunkSizeIn, const char bgFileName []):
    chunk(chunkSizeIn, bgFileName),
    secondStageDrawBuffer
    (new chunkElementBaseType [chunkSizeIn.y * chunkSizeIn.x])
  {
    loadAndInitialiseBackground();
  }

  ~backgroundData()
  {
    delete [] secondStageDrawBuffer;
  }

  void updateBuffers(const yx playerPos)
  {
       updateFirstStageBuffer(playerPos);
       updateSecondStageBuffer(playerPos, secondStageDrawBuffer);
  }
};


#endif
