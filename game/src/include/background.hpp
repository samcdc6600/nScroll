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
  static const int FIRST_STAGE_BUFFER_DIMENSIONS_SIZE {5};

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

  // Third argument to chunk is used to fill missing chunks with black spaces.
  backgroundData(const yx chunkSizeIn, const char bgFileName []):
    chunk(chunkSizeIn, FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
	  (MONO_CH_MAX +1) * 1 + ' ', bgFileName),
    secondStageDrawBuffer
    (new chunkElementBaseType [chunk::getSecondStageBufferSizeInChunks
      (FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
       "creating second stage background buffer") * chunkSizeIn.y *
      chunk::getSecondStageBufferSizeInChunks
      (FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
       "creating second stage background buffer") * chunkSizeIn.x])
  {
    loadAndInitialiseBackground();
  }

  ~backgroundData()
  {
    delete [] secondStageDrawBuffer;
  }

  void updateBuffers()
  {
    updateFirstStageBuffer();
    updateSecondStageBuffer(secondStageDrawBuffer);
  }
};


#endif
