#ifndef CHUNK_HPP_
#define CHUNK_HPP_


class <typename chunkElementType> chunk
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  typedef std::vector<chunkElementType> chunkType;
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx chunkSize;
  
priave:
  /* Concatenate (y / chunkSize.y) and (x / chunkSize.x) and use as index into
     map. */
  typedef std::map<std::string, chunkType> chunkMapType;
  const chunkMapType  chunkMap;

  class firstStageBufferType
  {
  public:
    /* Dimensions of first stage buffer in chunks and the offsets when updating
       the first stage buffer. */
    static const int fSBYChunks {5}, fSBXChunks {5},
      fSBYUpdateOffset {2}, fSBXUpdateOffset {2};
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



  
  
  chunk():
    chunkSize (chunkSizeIn.y, chunkSizeIn.x),
    background(loadAndInitialiseBackground(fileName)),
    firstStageDrawBuffer
    (firstStageBufferType::fSBYChunks *
     firstStageBufferType::fSBXChunks * chunkSizeIn.y * chunkSizeIn.x),
    fileName(chunkFileName)
  {
    
  }
};


#endif
