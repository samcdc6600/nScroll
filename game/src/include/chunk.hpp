#ifndef CHUNK_HPP_
#define CHUNK_HPP_


#include <map>
#include "utils.hpp"


template <typename chunkElementType>
class chunk
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  typedef chunkElementType chunkElementBaseType;
  typedef std::vector<chunkElementType> chunkType;
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx chunkSize;
  
protected:
  static const int minFSBSizeInChunks {5};
  /* Concatenate (y / chunkSize.y) and (x / chunkSize.x) and use as index into
     map. */
  typedef std::map<std::string, chunkType> chunkMapType;
  chunkMapType  chunkMap;
  class firstStageBufferType
  {
  public:
    /* Dimensions of first stage buffer in chunks and the offsets when updating
       the first stage buffer. */
    const int fSBYChunks, fSBXChunks, fSBYUpdateOffset, fSBXUpdateOffset;
    /* Holds the current position of the view port (Note that buffer may not yet
       be updated as a result this new position.) */
    yx viewPortPosition {};
    // Holds the position of the view port the last time buffer was updated.
    yx lastUpdatedPosition {};
    /* Holds a fSBYChunks by fSBXChunks buffer of the currently visible or
       "close to visible" chunks. */
    chunkElementType * buffer;
    
    firstStageBufferType(const yx chunkSizeIn, const int fSBSizeInChunks,
			 const int minFSBSizeInChunks) :
      fSBYChunks(fSBSizeInChunks), fSBXChunks(fSBSizeInChunks),
      fSBYUpdateOffset(fSBSizeInChunks / 2),
      fSBXUpdateOffset(fSBSizeInChunks / 2),
      buffer(new chunkElementType [fSBYChunks * chunkSizeIn.y *
				   fSBXChunks * chunkSizeIn.x])
    {
      if(fSBSizeInChunks < minFSBSizeInChunks || fSBSizeInChunks % 2 != 1)
	{
	  exit(concat
	       ("Error: first stage buffer size (", fSBSizeInChunks, ") (in "
		"chunks) passed to firstStageBufferType constructor is invalid."
		" It must be greater than ", minFSBSizeInChunks, " and odd.\n"),
	       ERROR_GENERIC_RANGE_ERROR);
	}
    }

    ~firstStageBufferType()
    {
      delete [] buffer;
    }
  };
  firstStageBufferType firstStageBuffer;

public:
  const char * fileName;
  
private:
  /* This is used to fill target chunks in the FSB when no chunk is found for a
     given key. */
  const chunkElementType MISSING_CHUNK_FILLER;

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
    /* To avoid visual artefacts the whole 1st stage buffer needs to be
     initialised.
     Should be called once initial player position is known, but before the main
     game loop. */
  void initFirstStageBuffer(const yx initialViewPortPos)
  {
    /* We need an initial position that is negative two chunks in the x
       dimension to ensure updating the chunk that initialViewPortPos maps to
       (this is assuming that lastUpdatedPosition is set to one chunk less than
       this.) We do this because if the view port position is two
       chunks to the left of the chunk mapped to by initialViewPortPos and
       the lastUpdatedPosition indicates a rightward movement the chunk
       two chunks ahead of initialCenterPos (view port position) will be updated
       (the chunk initialViewPortPos maps to in x.) However in the loop below
       we add a further offset of chunkIter chunks as we want to map from -|chunkIter|
       to |chunkIter|  with respect to initialViewPortPos. We also align the
       initialCenterPos to the closest chunk as this resolves some issues. */
    const yx initialCenterPos
      {initialViewPortPos.y - (initialViewPortPos.y % chunkSize.y),
       initialViewPortPos.x - (initialViewPortPos.x % chunkSize.x)
       - chunkSize.x * 2};
    int chunkIter {-(firstStageBuffer.fSBXChunks / 2)};
    /* We add -1 to chunkIter here because lastUpdatedPosition.x should be one
       chunk to the left of viewPortPosition. */
    firstStageBuffer.lastUpdatedPosition =
      yx{initialCenterPos.y, initialCenterPos.x + chunkSize.x * (chunkIter - 1)};

    /* Note that FSBXChunks should be odd. Fill columns (of height fSBYChunks)
       of chunks from left to right. */
    for( ; chunkIter <= (firstStageBuffer.fSBXChunks / 2);
	++chunkIter)
      {
	firstStageBuffer.viewPortPosition =
	  yx{initialCenterPos.y, initialCenterPos.x + chunkSize.x * chunkIter};	
	updateFirstStageBuffer();
      }
    
    firstStageBuffer.lastUpdatedPosition = initialViewPortPos;
    firstStageBuffer.viewPortPosition = initialViewPortPos;
  }

  
  /* UpdateFirstStageBuffer() will call this overloaded function (to do the
     actual update if it needs to be done in both dimensions at once.) */
  void updateFirstStageBuffer(const yx fSBSize)
  {
    /* Update chunks with a vertical offset. Iterate over chunks to be updated
       in the FSB. */
    for(int chunkUpdateDimensionIter {};
	chunkUpdateDimensionIter < firstStageBuffer.fSBXChunks;
	++chunkUpdateDimensionIter)
      {
	/* The first argument to calculateFSBTargetChunkWithVerticalChange()
	   must be true to ensure that the current view port position is used
	   as part of the x offset. */
	const yx fSBTargetChunk
	  {calculateFSBTargetChunkWithVerticalChange
	   (true, fSBSize, chunkUpdateDimensionIter)};
	/* Calculate index of potential chunk to be copied into FSB. Again we
	   use true here to ensure that the current view port position is
	   used. The second argument specifies that we want the calculation
	   done with regards to the vertical dimension. */
	const std::string chunkKey
	  {calculatePotentialChunkKeyForChunkToGoInFSB
	   (true, false, chunkUpdateDimensionIter)};
	getChunkAndCopyIntoFSB(fSBSize, fSBTargetChunk, chunkKey);
      }
    
    // Update chunks with a horizontal offset.
    /* Used to control which chunk not to update (because it has already been
       updated by the vertical updating code). */
    const bool verticalUpdateDirectionUP
      {firstStageBuffer.lastUpdatedPosition.y >
       firstStageBuffer.viewPortPosition.y};

    /* Update chunks with a horizontal offset. Iterate over chunks to be updated
       in the FSB. */
    for(int chunkUpdateDimensionIter {verticalUpdateDirectionUP ? 1: 0};
	chunkUpdateDimensionIter <
	  firstStageBuffer.fSBYChunks - (verticalUpdateDirectionUP ? 0: 1);
	++chunkUpdateDimensionIter)
      {
	/* The first argument to calculateFSBTargetChunkWithHorizontalChange()
	   must be true to ensure that the current view port position is used
	   as part of the x offset. */
	const yx fSBTargetChunk
	  {calculateFSBTargetChunkWithHorizontalChange
	   (true, fSBSize, chunkUpdateDimensionIter)};
	/* Calculate index of potential chunk to be copied into FSB. Again we
	   use true here to ensure that the current view port position is
	   used. The second argument specifies that we want the calculation
	   done with regards to the horizontal dimension. */
	const std::string chunkKey
	  {calculatePotentialChunkKeyForChunkToGoInFSB
	   (true, true, chunkUpdateDimensionIter)};
	  // 		const std::string chunkKey
	  // {calculatePotentialChunkKeyForChunkToGoInFSB
	  //  (true, chunkUpdateDimensionIter)};
	getChunkAndCopyIntoFSB(fSBSize, fSBTargetChunk, chunkKey);
      }

    // Update both axis of the last view port position.
    updateLastViewPortPosition(false);
    updateLastViewPortPosition(true);
  }

  
  /* UpdateFirstStageBuffer() will call this overloaded function (to do the
     actual update if it only needs to be done in one dimension) If horizontal
     is true an update will be done assuming that the triggering change in view
     port position was in the x dimension. Otherwise it will be done assuming
     that the trigger change in the view port position was in the y
     dimension. */
  void updateFirstStageBuffer(const yx fSBSize, const bool horizontal)
  {
    /* NOTE THAT THE CODE HERE WILL ONLY WORK CORRECTLY WHEN fSBYChunks AND
       fSBXChunks ARE UNEVEN. HOWEVER THEY SHOULD BE. SO THIS SHOULDN'T BE A
       PROBLEM. */
  
    // Iterate over chunks to be updated in the FSB.
    for(int chunkUpdateDimensionIter {};
	chunkUpdateDimensionIter < (horizontal ? firstStageBuffer.fSBYChunks:
				    firstStageBuffer.fSBXChunks);
	++chunkUpdateDimensionIter)
      {
	/* The first argument to calculateFSBTargetChunkWithVerticalChange()
	   must be false to ensure that the previous view port position is used
	   as part of the x offset. */
	const yx fSBTargetChunk
	  {horizontal ?
	   calculateFSBTargetChunkWithHorizontalChange
	   (false, fSBSize, chunkUpdateDimensionIter):
	   calculateFSBTargetChunkWithVerticalChange
	   (false, fSBSize, chunkUpdateDimensionIter)};
	/* Calculate index of potential chunk to be copied into FSB. Again we
	   use false here to ensure that the previous view port position is
	   used.*/
	const std::string chunkKey
	  {calculatePotentialChunkKeyForChunkToGoInFSB
	   (false, horizontal, chunkUpdateDimensionIter)};
	getChunkAndCopyIntoFSB(fSBSize, fSBTargetChunk, chunkKey);
      }

    updateLastViewPortPosition(horizontal);
  }

  
  /* Calculates the coordinates in the FSB that a chunk should be copied to
     when the view port has moved chunkSize.x in the x dimension. Where
     yChunkOffset is used as an offset (in chunks) in the y dimension. Handles
     wrap around. I.e. if the view port position is (0,0) and the old position
     was (0, 170) (where 170 is the size of a chunk in the x dimension in this
     example) then the set of chunks to be updated given 5 calls with
     yChunkOffset being set to (0, 1, 2, 3, 4) for each respective call, should
     be (3, 3), (4, 3), (0, 3), (1, 3), (2, 3).
     DoubleAxisUpdate should be set to true if an update needs to be done for
     both axis.  It will ensure that the current view port position is used as
     an offset (with regards to the x axis) as opposed to the last updated
     position. */
  yx calculateFSBTargetChunkWithHorizontalChange
  (const bool doubleAxisUpdate, const yx fSBSize, const int yChunkOffset) const
  {
    /* Note that the code in this function assumes that a mod can produce a
       negative result. */
    /* Y should change each iteration of the loop in the calling function, but x
       shouldn't. We have to wrap around in y if y >=
       firstStageBuffer.fSBYChunks. */
    /* Here if viewPortPosition.x < 0 we must sub the x view port dimension
       (minus 1) from viewPortPosition.x before the modulous (maybe?) and
       division operations because -50 / 170 and 50 / 170 are both 0. However we
       need -50 / 170 to be -1, because otherwise we would have two chunks
       mapping to one (actually more than that when taking into account the y
       dimension.) */ 
    yx targetChunk
      {0, ((firstStageBuffer.viewPortPosition.x -
	    (firstStageBuffer.viewPortPosition.x < 0 ? xWidth -1: 0))
	   % fSBSize.x) / chunkSize.x};
    // Account for negative coordinate
    targetChunk.x = targetChunk.x < 0 ?
      firstStageBuffer.fSBXChunks + targetChunk.x: targetChunk.x;
    /* Add offset in the x dimension based on the direction the view port has
       moved. */  
    targetChunk.x += firstStageBuffer.viewPortPosition.x >
      firstStageBuffer.lastUpdatedPosition.x ?
      firstStageBuffer.fSBXUpdateOffset:
      - firstStageBuffer.fSBXUpdateOffset;
    /* Maybe fix negative and wrap around in x... */
    targetChunk.x %= firstStageBuffer.fSBXChunks;
    targetChunk.x = targetChunk.x < 0 ?
      firstStageBuffer.fSBXChunks + targetChunk.x: targetChunk.x;
    /* FirstStageBuffer.fSBYChunks & 1 returns 1 if fSBYChunks is even.
       Y must be relative to the lastUpdatedPosition, because there will be a
       chunk in the fSB that is updated with the wrong chunk if we use the
       viewPortPosition.*/ 
    const int targetYPreWrap
      {((((doubleAxisUpdate ? firstStageBuffer.viewPortPosition.y :
	   firstStageBuffer.lastUpdatedPosition.y) -
	  ((doubleAxisUpdate ? firstStageBuffer.viewPortPosition.y :
	    firstStageBuffer.lastUpdatedPosition.y) < 0 ? yHeight -1: 0)) %
	 fSBSize.y) / chunkSize.y +
	(firstStageBuffer.fSBYChunks / 2 +
	 (firstStageBuffer.fSBYChunks & 1)) +
	yChunkOffset)};
    targetChunk.y = targetYPreWrap % firstStageBuffer.fSBYChunks;
    /* Maybe fix negative and wrap around in y... */
    targetChunk.y = targetChunk.y < 0 ?
      firstStageBuffer.fSBYChunks + targetChunk.y: targetChunk.y;

    return targetChunk;
  }

  
  /* DoubleAxisUpdate should be set to true if an update needs to be done for
     both axis.  It will ensure that the current view port position is used as
     an offset (with regards to the x axis) as opposed to the last updated
     position. */
  yx calculateFSBTargetChunkWithVerticalChange
  (const bool doubleAxisUpdate, const yx fSBSize, const int xChunkOffset) const
  {
    /* Note that the code in this function assumes that a mod can produce a
       negative result. */
    /* X should change each iteration of the loop in the calling function, but y
       shouldn't. We have to wrap around in x if x >=
       firstStageBuffer.fSBXChunks. */
    /* Here if viewPortPosition.y < 0 we must sub the y view port dimension
       (minus 1) from viewPortPosition.x before the modulous (maybe?) and
       division operations because -50 / 170 and 50 / 170 are both 0. However we
       need -50 / 170 to be -1, because otherwise we would have two chunks
       mapping to one (actually more than that when taking into account the x
       dimension.) */ 
    yx targetChunk
      {((firstStageBuffer.viewPortPosition.y -
	 (firstStageBuffer.viewPortPosition.y < 0 ? yHeight -1: 0))
	% fSBSize.y) / chunkSize.y, 0};
    // Account for negative coordinate
    targetChunk.y = targetChunk.y < 0 ?
      firstStageBuffer.fSBYChunks + targetChunk.y: targetChunk.y;
    /* Add offset in the y dimension based on the direction the view port has
       moved. */
    targetChunk.y += firstStageBuffer.viewPortPosition.y >
      firstStageBuffer.lastUpdatedPosition.y ?
      firstStageBuffer.fSBYUpdateOffset:
      - firstStageBuffer.fSBYUpdateOffset;
    /* Maybe wrap around in y... NOTE THAT THE PLAYER SHOULD NOT BE ABLE TO HAVE
       A NEGATIVE COORDINATE AND AS SUCH THE FOLLOWING CODE SHOULDN'T BE AN
       ISSUE BECAUSE WHEN targetChunk.y IS NEGATIVE |targetChunk.y| SHOULD NEVER
       BE MORE THAN firstStageBuffer.fSBYChunks. */
    targetChunk.y %= firstStageBuffer.fSBYChunks;
    targetChunk.y = targetChunk.y < 0 ?
      firstStageBuffer.fSBYChunks + targetChunk.y: targetChunk.y;
    /* FirstStageBuffer.fSBXChunks & 1 returns 1 if fSBXChunks is even.
       X must be relative to the lastUpdatedPosition, because there will be a
       chunk in the fSB that is updated with the wrong chunk if we use the
       viewPortPosition.*/ 
    const int targetXPreWrap
      {((((doubleAxisUpdate ? firstStageBuffer.viewPortPosition.x:
	   firstStageBuffer.lastUpdatedPosition.x) -
	  ((doubleAxisUpdate ? firstStageBuffer.viewPortPosition.x:
	   firstStageBuffer.lastUpdatedPosition.x) < 0 ? xWidth -1: 0)) %
	 fSBSize.x) / chunkSize.x +
	(firstStageBuffer.fSBXChunks / 2 +
	 (firstStageBuffer.fSBXChunks & 1)) +
	xChunkOffset)};
    targetChunk.x = targetXPreWrap % firstStageBuffer.fSBXChunks;
    /* Maybe fix negative and wrap around in x... */
    targetChunk.x = targetChunk.x < 0 ?
      firstStageBuffer.fSBXChunks + targetChunk.x: targetChunk.x;

    return targetChunk;
  }
  
  
  /* DoubleAxisUpdate should be set to true if an update needs to be done for
     both axis. Where horizontal determines whether or not to use
     chunkUpdateDimensionIter as a component of y or x when creating the
     key. Horizontal should be set to true when the triggering change in view
     port position was in the x dimension. */
  std::string calculatePotentialChunkKeyForChunkToGoInFSB
  (const bool doubleAxisUpdate, const bool horizontal,
   const int chunkUpdateDimensionIter) const
  {
    /* Note here that we account for the direction of movement when calculating
       the y or x coordinate based on the value of horizontal. */
    yx chunkCoord {};

    if(horizontal)
      {
	/* Here Y must be relative to the lastUpdatedPosition, bceause there
	   will be a chunk in the fSB that is updated with the wrong chunk if
	   we use the viewPortPosition.*/
	chunkCoord =
	  yx{(doubleAxisUpdate ? firstStageBuffer.viewPortPosition.y:
	      firstStageBuffer.lastUpdatedPosition.y) +
	     ((chunkUpdateDimensionIter -
	       (firstStageBuffer.fSBYChunks / 2)) * chunkSize.y),
	     firstStageBuffer.viewPortPosition.x +
	     (firstStageBuffer.viewPortPosition.x >
	      firstStageBuffer.lastUpdatedPosition.x ?
	      firstStageBuffer.fSBXUpdateOffset:
	      -firstStageBuffer.fSBXUpdateOffset) * chunkSize.x};
      }
    else
      {
	/* Here X must be relative to the lastUpdatedPosition, bceause there
	   will be a chunk in the fSB that is updated with the wrong chunk if
	   we use the viewPortPosition.*/
	chunkCoord =
	  yx{(firstStageBuffer.viewPortPosition.y +
	      (firstStageBuffer.viewPortPosition.y >
	       firstStageBuffer.lastUpdatedPosition.y ?
	       firstStageBuffer.fSBYUpdateOffset:
	       -firstStageBuffer.fSBYUpdateOffset) * chunkSize.y),
	     (doubleAxisUpdate ? firstStageBuffer.viewPortPosition.x:
	      firstStageBuffer.lastUpdatedPosition.x) +
	     ((chunkUpdateDimensionIter -
	       (firstStageBuffer.fSBXChunks  / 2)) * chunkSize.x)};
      }
  
    return createChunkCoordKeyFromCharCoord(chunkCoord);
  }


  /* Tries to get chunk associated with chunkKey and copy it into
     fSBTargetChunk in the first stage buffer. If no chunk is found to be
     associated chunkKey then fills target chunk with MISSING_CHUNK_FILLER.
     Where chunk key and fSBTargetchunk are in chunks and fSBSize is the size of
     the first stage buffer. */
  void getChunkAndCopyIntoFSB(const yx fSBSize, const yx fSBTargetChunk,
			      const std::string chunkKey)
  {
    // Try to get chunk and then perform the copy to the FSB.
    try
      {
	// At() will throw an exception if the key isn't found.
	const chunkType * chunk {&chunkMap.at(chunkKey)};
	// Iterate over lines of target chunk in the FSB.
	for(int yIter {}; yIter < chunkSize.y; ++yIter)
	  {
	    for(int xIter {}; xIter < chunkSize.x; ++xIter)
	      {
		firstStageBuffer.buffer
		  [(fSBTargetChunk.y * fSBSize.x * chunkSize.y) +
		   yIter * fSBSize.x +
			     
		   ((fSBTargetChunk.x * chunkSize.x)
		    + xIter)] 
		  = (*chunk)[(yIter * chunkSize.x) + xIter];
	      }
	  }
      }
    catch(const std::out_of_range& err)
      {
	/* Key not found... Fill target chunk in first stage buffer with
	   some stuff. */
	for(int yIter {}; yIter < chunkSize.y; ++yIter)
	  {
	    for(int xIter {}; xIter < chunkSize.x; ++xIter)
	      {
		firstStageBuffer.buffer
		  [(fSBTargetChunk.y * fSBSize.x * chunkSize.y) +
		   yIter * fSBSize.x +			     
		   ((fSBTargetChunk.x * chunkSize.x)
		    + xIter)] = MISSING_CHUNK_FILLER;;
	      }
	  }
      }
  }


  /* Updates one coordinate of the last view port position to the position of
     the chunk that was just updated (actual position not the FSB pos.) Where
     the coordinate to be updated is based on horizontal. */
  void updateLastViewPortPosition(bool horizontal)
  {
    if(horizontal)
      {
	firstStageBuffer.lastUpdatedPosition.x =	 
	  (firstStageBuffer.viewPortPosition.x
	  / chunkSize.x) * chunkSize.x;
      }
    else
      {
	firstStageBuffer.lastUpdatedPosition.y =
	  (firstStageBuffer.viewPortPosition.y
	  / chunkSize.y) * chunkSize.y;
      }
  }
  
  
protected:
  /* This must be called to properly initialise the buffers before use. */
  void initBuffers(const yx initialViewPortPos,
		   chunkElementType *  secondStageBuffer)
  {
    initFirstStageBuffer(initialViewPortPos);
    updateSecondStageBuffer(secondStageBuffer);
  }

  
    /* Updates the first stage buffer if
     firstStageBuffer.viewPortPosition and firstStageBuffer.lastUpdatedPosition
     have diverged by a sufficient delta. If an update is performed
     lastUpdatedPosition is set to the same values as viewPortPosition. */
  void updateFirstStageBuffer()
  {
    /*
      The first stage buffer may be a 5 by 5 array of chunks. When the delta
      between the last updated position and the new position (viewPortPosition)
      has diverged by the dimension of one chunk (note that this differs
      depending on the axis.) The chunks two chunks ahead of the
      viewPortPosition (in y or x or both) are updated depending on the
      dimension/s in which there is a change in position of more than one chunk.
      Note here that it is only two chunks ahead for the case when the array is
      5x5. For the general case it will be half of the array size rounded down
      and the array size should always be uneven.
      The diagram below shows the position of the last updated position and the
      current position upon entering this function and the chunks that will be
      updated because the current position is one chunk ahead of the last
      updated position (this is assuming an update is being done in the
      horizontal dimension only and because of a rightward movement, as opposed
      to a leftward one.)
      Where C is a chunk that will stay the same L is the last updated position,
      N is the new position and X is a chunk that needs to be updated because
      the delta between L and N is one chunks worth in terms of y or x (X in
      this case.)
    
      +---+---+---+---+---+
      | X | C | C | C | C |
      +---+---+---+---+---+
      | X | C | C | C | C |
      +---+---+---+---+---+
      | X | C | L | N | C |
      +---+---+---+---+---+
      | X | C | C | C | C |
      +---+---+---+---+---+
      | X | C | C | C | C |
      +---+---+---+---+---+

      This means that the view port can move up to y -1 or x -1 in the y or x
      directions before an update needs to be done.
      NOTE THAT WE ARE INITIALLY GOING TO IMPLEMENT THIS FUNCTION SO THAT IT
      WILL BE RUN IN THE SAME THREAD AS THE REST OF THE GAME. HOWEVER WE PLAN TO
      MOVE TO IT'S OWN THREAD. WHEN AN UPDATE NEEDS TO BE DONE IT CAN BE DONE IN
      PARALLEL WITH THE REST OF THE GAME LOGIC. THE THREAD DOING TO UPDATING
      WILL BE ABLE TO UPDATE FOR A FULL CHUNKS WORTH OF MOVEMENT IN THE MAIN
      THREAD (BECAUSE THERE IS A COLUMN (OR ROW) OF CHUNKS IN BETWEEN THE
      CURRENT POSITION AND THE COLUMN (OR ROW) BEING UPDATED. IF THERE IS MORE
      MOVEMENT THAN THAT THEN THE MAIN THREAD WILL NEED TO BE PAUSED TO WAIT FOR
      THE UPDATING THREAD. HOWEVER WE THINK THAT THIS SHOULD NOT HAPPEN UNDER
      NORMAL CIRCUMSTANCES AS ONE CHUNKS WORTH OF MOVEMENT IN THE WORSE CASE (IN
      THE Y AXIS) IS 40 CHUNKS WORTH OF FRAMES AND THE ENGINE SLEEPS FOR SOME
      TIME BETWEEN EACH FRAME. CONSIDERING THAT 5 CHUNKS ARE UPDATED FOR EACH
      UPDATE (IF THE BUFFER IS 5 X 5) THIS MEANS THAT THE MAIN THREAD WILL BE
      COPYING 35 CHUNKS WORTH OF DATA MORE THAN THE UPDATING THREAD. HOWEVER THE
      UPDATING THREAD WILL NEED TO DO 5 LOOKUPS TO FIND THE THREADS AND THESE
      LOOKUPS WILL PROBABLY BE THE SLOWEST OPERATIONS PERFORMED BY THE UPDATING
      THREAD. THIS DOESN'T TAKE INTO ACCOUNT DIAGONAL MOVEMENT PATTERNS.
    */
    /* Returns true if an update is needed in the x dimension as a result of
       firstStageBuffer.viewPortPosition.x and
       firstStageBuffer.lastUpdatedPosition.x having diverged by a sufficient
       delta. */
    std::function<bool()> updatedNeededInXDimension = [&]()
    {
      // x > y ? x - y : y - x. Get distance between two numbers on number line.
      return ((firstStageBuffer.lastUpdatedPosition.x >
	       firstStageBuffer.viewPortPosition.x) ?
	      ((firstStageBuffer.lastUpdatedPosition.x -
		firstStageBuffer.viewPortPosition.x) > chunkSize.x -1) :
	      ((firstStageBuffer.viewPortPosition.x -
		firstStageBuffer.lastUpdatedPosition.x) > chunkSize.x -1));
    };
    /* Returns true if an update is needed in the y dimension as a result of
       firstStageBuffer.viewPortPosition.y and
       firstStageBuffer.lastUpdatedPosition.y having diverged by a sufficient
       delta. */
    std::function<bool()> updatedNeededInYDimension = [&]()
    {
      // x > y ? x - y : y - x. Get distance between two numbers on number line.
      return ((firstStageBuffer.lastUpdatedPosition.y >
	       firstStageBuffer.viewPortPosition.y) ?
	      ((firstStageBuffer.lastUpdatedPosition.y -
		firstStageBuffer.viewPortPosition.y) > chunkSize.y -1) :
	      ((firstStageBuffer.viewPortPosition.y -
		firstStageBuffer.lastUpdatedPosition.y) > chunkSize.y -1));
    };
    

    
    // x > y ? x - y : y - x. Get distance between two numbers on number line.
    if(updatedNeededInXDimension() && !updatedNeededInYDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks}, true);
      }
    else if(updatedNeededInYDimension() && !updatedNeededInXDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks}, false);
      }
    else if(updatedNeededInXDimension() && updatedNeededInYDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks});
      }
  }

  
  /* Copies one chunk (relative to viewPortPosition) from firstStageBuffer
     to secondStageBuffer. */
  void updateSecondStageBuffer
  (chunkElementType *  secondStageBuffer)
  { 
    // Size of the first stage buffer in characters.
    const yx fSBSize {chunkSize.y * firstStageBuffer.fSBYChunks,
		      chunkSize.x * firstStageBuffer.fSBXChunks};  
  
    for(int yIter {}; yIter < chunkSize.y; ++yIter)
      {
	yx yAndXComponents
	  {((firstStageBuffer.viewPortPosition.y + yIter) % fSBSize.y), 0};
	// Maybe fix negative and wrap around in y...
	yAndXComponents.y = yAndXComponents.y < 0 ?
	  fSBSize.y + yAndXComponents.y:
	  yAndXComponents.y;
	// Convert to linear position in array with respect to y.
	yAndXComponents.y *= fSBSize.x;
      
	for(int xIter {}; xIter < chunkSize.x; ++xIter)
	  {
	    yAndXComponents.x =
	      ((firstStageBuffer.viewPortPosition.x + xIter) % fSBSize.x);
	    // Maybe fix negative and wrap around in x...
	    yAndXComponents.x = yAndXComponents.x < 0 ?
	      fSBSize.x + yAndXComponents.x:
	      yAndXComponents.x;

	    secondStageBuffer[(yIter * chunkSize.x) + xIter] =
	      firstStageBuffer.buffer[yAndXComponents.y + yAndXComponents.x];
	  }  
      }
  }
       
  
public:
  chunk(const yx chunkSizeIn, const int fSBSizeInChunks,
	const chunkElementType missingChunkFiller, const char fileName []):
    chunkSize(chunkSizeIn.y, chunkSizeIn.x),
    firstStageBuffer(chunkSizeIn, fSBSizeInChunks, minFSBSizeInChunks),
    fileName(fileName),
    MISSING_CHUNK_FILLER(missingChunkFiller)
  {
  }


  /* This function should be called by any code that allocates memory for a
     second stage buffer. Where firstStageBufferSizeInChunks is the size of the
     first stage buffer (in chunks) that will be used by the object the second
     stage buffer in question will be used with. For example a first stage
     buffer for the game background might be 5x5, it's second stage buffer
     should then be 1x1  or 5 - (minFSBSizeInChunks -1) (assuming
     minFSBSizeInChunks is 5), but a first stage buffer for character rules
     may be 7x7 and in this case it's second stage buffer should be 3x3 or 7 -
     (minFSBSizeInChunks -1) (assuming minFSBSizeInChunks is 5). This would
     mean that physics could be calculated outside of the view port (to some
     extent.) */
  static ssize_t getSecondStageBufferSizeInChunks
  (const ssize_t firstStageBufferSizeInChunks, const std::string eMsg)
  {
    if(firstStageBufferSizeInChunks < minFSBSizeInChunks ||
       firstStageBufferSizeInChunks % 2 != 1)
      {
	exit(concat("Error calculating second stage buffer dimension size in "
		    "chunks when ", eMsg, "."), ERROR_BAD_INTERNAL_VALUE);
      }
    
    return (firstStageBufferSizeInChunks - (minFSBSizeInChunks -1));
  }
  
  
  ssize_t numberOfChunks() const
  {
    return chunkMap.size();
  }

  
  bool keyExists(const std::string & key) const
  {
    return !(chunkMap.find(key) == chunkMap.end());
  }


  /* Updates the view port pos if the relative player position has changed
     enough since the last time the view port pos was updates. */
  void updateViewPortPosition
  (const yx playerMovementAreaPadding, const yx playerPosVPRel,
   const yx playerMaxBottomRightOffset,
   bool (* testIntersectionWithPaddingInTopOrLeft)
   (const yx playerMovementAreaPadding, const yx playerPosVPRel,
    const yx playerMaxBottomRightOffset, const bool yDimension),
   bool (* testIntersectionWithPaddingInBottomOrRight)
   (const yx playerMovementAreaPadding, const yx playerPosVPRel,
    const yx playerMaxBottomRightOffset, const bool yDimension,
    const yx viewPortSize))
  {
    if(testIntersectionWithPaddingInTopOrLeft
       (playerMovementAreaPadding, playerPosVPRel, playerMaxBottomRightOffset,
	true))
      {
      // The top of player is in or above the top padding region.
       firstStageBuffer.viewPortPosition.y -=
	 playerMovementAreaPadding.y - playerPosVPRel.y;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, playerPosVPRel,
	   playerMaxBottomRightOffset, true, chunkSize))
    {
      // The bottom of player is in or below the bottom padding region.
       firstStageBuffer.viewPortPosition.y +=
	 (playerPosVPRel.y - (chunkSize.y -playerMovementAreaPadding.y
			      -(playerMaxBottomRightOffset.y + 1)));
    }
  else if(testIntersectionWithPaddingInTopOrLeft
	  (playerMovementAreaPadding, playerPosVPRel,
	   playerMaxBottomRightOffset, false))
    {
      // The left of the player is in or to the left of the left padding region.
      firstStageBuffer.viewPortPosition.x -=
	playerMovementAreaPadding.x - playerPosVPRel.x;
    }
  else if(testIntersectionWithPaddingInBottomOrRight
	  (playerMovementAreaPadding, playerPosVPRel,
	   playerMaxBottomRightOffset, false, chunkSize))
    {
      /* The right of the player is in or to the right of the right padding
         region. */
      firstStageBuffer.viewPortPosition.x +=
	(playerPosVPRel.x - (chunkSize.x -playerMovementAreaPadding.x
			     -(playerMaxBottomRightOffset.x + 1)));
    }
    

    /*    static bool firstCall {true};
    if(firstCall)
      {
	firstCall = false;
	if(std::remove("posUpdateSequence.cpp") != 0)
	  {
	    exit("Failed to remove file!", 0);
	  }
      }
    
    std::ofstream file("posUpdateSequence.cpp", std::ios::app);
    if (!file.is_open())
      {
	exit("Error failed to open file!", 0);
      }

    static int a {};
    static bool incA {true};
    //     static int b {};
    static bool incB {true};
    
    static bool dimension {true};


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.8, 1.5);
    std::uniform_real_distribution<> dis2(0.5, 1.5);
    
        static std::chrono::time_point<std::chrono::high_resolution_clock> start;
    static bool first {true};
    if(first)
      {
	first = false;
	  start = std::chrono::high_resolution_clock::now();
      }
    
    static std::chrono::time_point<std::chrono::high_resolution_clock> end;
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
							

    static double dist3;
    static double dist4;
    
    if(elapsed.count() > 2.4)
      {
	start = end;
	dist3 = dis2(gen);
	dist4 = dis2(gen);
      }
    
    
	if(firstStageBuffer.viewPortPosition.x > 780)
	  {
	    firstStageBuffer.viewPortPosition.x -= 1;
	    file<<"-1,\n";
	  }
	if(firstStageBuffer.viewPortPosition.x < -270)
	  {
	    firstStageBuffer.viewPortPosition.x += 1;
	    file<<"1,\n";
	  }
	else
	  {
	    int a ((int)dist3 ? 1 : -1);
	    firstStageBuffer.viewPortPosition.x += a;
	      // (int)dist3 ? dis(gen) : -dis(gen);
	    file<<a<<",\n";
	  }

	if(firstStageBuffer.viewPortPosition.y > 220)
	  {
	    firstStageBuffer.viewPortPosition.y -= 1;
	    file<<"-1,\n";
	  }
	else if(firstStageBuffer.viewPortPosition.y < - 96)
	  {
	    firstStageBuffer.viewPortPosition.y += 1;
	    file<<"1,\n";
	  }
	else
	  {
	    int a ((int)dist4 ? -1: 1);
	    firstStageBuffer.viewPortPosition.y += a;
	      // (int)dist4 ? -dis(gen): dis(gen);
	    file<<a<<",\n";
	  }

	  file.close();*/
  }


  void updateViewPortPosition(const yx newPosition)
  {
    firstStageBuffer.viewPortPosition = newPosition;
  }


  yx getViewPortPosition() const {return firstStageBuffer.viewPortPosition;}
};


#endif
