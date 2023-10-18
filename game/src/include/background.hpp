#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_


#include <vector>
#include <string>
#include <map>
#include "chunk.hpp"
#include "colorS.hpp"


class backgroundData: public chunk<bgSecondStageDrawBufferType>
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  chunkElementBaseType * secondStageDrawBuffer;
  
private:
  /* Chunk elements are stored in this format on disk. We will use
     chunkFileElementBaseType for reading in chunks. After this we will convert
     the chunks to chunkElementBaseType (this requires a small calculation for
     each character (chunk element).) */
  typedef unsigned short chunkFileElementBaseType;
  static const int FIRST_STAGE_BUFFER_DIMENSIONS_SIZE {5};
  const chunkFileElementBaseType bgRunLengthSequenceSignifier
  {std::numeric_limits<chunkFileElementBaseType>::max()};

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
  void loadAndInitialiseBackground();
  void initialiseBackground
  (std::fstream & bgFile, chunkMapType & background);
  /* Reads in and decompress a chunk from a file. The file should already be
   open. If it is a multi chunk file, multiChunkFile should be set to true. In
   this case if there is an error the function will return false. However if
   multiChunkFile is false an error message will be printed and the program
   exited upon error. File should be an already opened chunk file, chunk is
   where the chunk read in will be stored and chunkCoord is the chunk
   coordinates read from the file.
   NOTE THAT IF THE LAST CHUNK IN THE FILE IS READ FILE.FAIL() WILL BE TRUE
   AND WILL NEED TO BE RESET IF MORE OPERATIONS (AT LEAST FOR SOME POSSIBLE
   OPERATIONS) NEED TO BE DONE ON FSTREAM. */
bool getBgChunk
(std::fstream & file, std::vector<chunkFileElementBaseType> & chunk,
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile = false);
  
public:

  // Third argument to chunk is used to fill missing chunks with black spaces.
  backgroundData(const yx chunkSizeIn, const char bgFileName []):
    chunk(chunkSizeIn, FIRST_STAGE_BUFFER_DIMENSIONS_SIZE,
	  colorParams::blackSpaceChar, bgFileName, "calling chunk constructor"
	  "before backgroundData constructor body"),
    secondStageDrawBuffer
    (new chunkElementBaseType
     [chunk::secondStageBufferSizeInChunks * chunkSizeIn.y *
      chunk::secondStageBufferSizeInChunks * chunkSizeIn.x])
  {
    loadAndInitialiseBackground();
  }

  
  ~backgroundData()
  {
    delete [] secondStageDrawBuffer;
  }


  void initBuffers(const yx initialViewPortPos)
  {
    chunk::initBuffers(initialViewPortPos, secondStageDrawBuffer);
  }
  

  void updateBuffers()
  {
    updateFirstStageBuffer();
    updateSecondStageBuffer(secondStageDrawBuffer);
  }
};


#endif
