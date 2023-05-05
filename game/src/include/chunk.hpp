#ifndef CHUNK_HPP_
#define CHUNK_HPP_


#include <map>
#include "utils.hpp"


template <typename chunkElementType>
class  chunk
{
public:
  // ========================== MEMBER VARIABLES START =========================
  // ===========================================================================
  typedef chunkElementType chunkElementBaseType;
  typedef std::vector<chunkElementType> chunkType;
  // (View port size) used to retrieve a chunk given a coordinate.
  const yx chunkSize;
  
protected:
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
    
    firstStageBufferType(const yx chunkSizeIn, const int fSBSizeInChunks) :
      fSBYChunks(fSBSizeInChunks), fSBXChunks(fSBSizeInChunks),
      fSBYUpdateOffset(fSBSizeInChunks / 2),
      fSBXUpdateOffset(fSBSizeInChunks / 2),
      buffer(new chunkElementType [fSBYChunks * chunkSizeIn.y *
				   fSBXChunks * chunkSizeIn.x])
    {
      if(fSBSizeInChunks < 5 || fSBSizeInChunks % 2 != 1)
	{
	  exit(concat
	       ("Error: first stage buffer size (", fSBSizeInChunks, ") (in "
		"chunks) passed to firstStageBufferType constructor is invalid."
		" It must be greater than 5 and odd.\n"),
	       ERROR_GENERIC_RANGE_ERROR);
	}
    }

    ~firstStageBufferType()
    {
      delete [] buffer;
    }
  };
  firstStageBufferType firstStageBuffer;
  
private:

  /* This is used to fill target chunks in the FSB when no chunk is found for a
     given key. */
  const chunkElementType MISSING_CHUNK_FILLER;

  // ========================== MEMBER FUNCTIONS START =========================
  // ===========================================================================
  /* Returns true if an update is needed in the x dimension as a result of
     firstStageBuffer.viewPortPosition.x and
     firstStageBuffer.lastUpdatedPosition.x having diverged by a sufficient
     delta. */
  bool updatedNeededInXDimension() const
  {
    // x > y ? x - y : y - x. Get distance between two numbers on number line.
    return ((firstStageBuffer.lastUpdatedPosition.x >
	     firstStageBuffer.viewPortPosition.x) ?
	    ((firstStageBuffer.lastUpdatedPosition.x -
	      firstStageBuffer.viewPortPosition.x) > chunkSize.x -1) :
	    ((firstStageBuffer.viewPortPosition.x -
	      firstStageBuffer.lastUpdatedPosition.x) > chunkSize.x -1));
  }
  /* Returns true if an update is needed in the y dimension as a result of
     firstStageBuffer.viewPortPosition.y and
     firstStageBuffer.lastUpdatedPosition.y having diverged by a sufficient
     delta. */
  bool updatedNeededInYDimension() const
  {
    // x > y ? x - y : y - x. Get distance between two numbers on number line.
    return ((firstStageBuffer.lastUpdatedPosition.y >
	     firstStageBuffer.viewPortPosition.y) ?
	    ((firstStageBuffer.lastUpdatedPosition.y -
	      firstStageBuffer.viewPortPosition.y) > chunkSize.y -1) :
	    ((firstStageBuffer.viewPortPosition.y -
	      firstStageBuffer.lastUpdatedPosition.y) > chunkSize.y -1));
  }


  /* UpdateFirstStageBuffer() will call this overloaded function (to do the
     actual update if it needs to be done in both dimensions at once.) */
  void updateFirstStageBuffer(const yx fSBSize)
  {
    // Update chunks with a vertical offset.
        // Iterate over chunks to be updated in the FSB.
    for(int chunkUpdateDimensionIter {};
	chunkUpdateDimensionIter < firstStageBuffer.fSBXChunks;
	++chunkUpdateDimensionIter)
      {
	/* Here use use true as an argument to
	   calculateFSBTargetChunkWithVerticalChange() because we want it to
	   use the current view port position as the offset for the start of
	   the chunks being iterated over (the thing we had to change to fix
	   the code, not that we will have to update the
	   calculateFSBTargetChunkWithVerticalChange() function to handle this
	   extra argument. It should work as it currently does for the other
	   version of updateFirstStageBuffer(). Note that we will also need to
	   add an analogous option to the horizontal change version of the
	   aforementioned function and also to the function to calculate the
	   chunk key. */
	const yx fSBTargetChunk
	  {calculateFSBTargetChunkWithVerticalChange
	   (true, fSBSize, chunkUpdateDimensionIter)};
      }
    
    // Update chunks with a horizontal offset.
    /* Used to control which chunk not to update (because it has already been
       updated by the vertical updating code) when updating horizontally. */
    const bool verticalUpdateDirectionUP
      {firstStageBuffer.lastUpdatedPosition.y <
       firstStageBuffer.viewPortPosition.y};
  }


  // std::string chunkKey;
  
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
	//tmp
      // 	    chunkKey = calculatePotentialChunkKeyForChunkToGoInFSB
      // (horizontal, chunkUpdateDimensionIter);
	    // ^^ tmp ^^


	    
	const yx fSBTargetChunk
	  {horizontal ?
	   calculateFSBTargetChunkWithHorizontalChange
	   (fSBSize, chunkUpdateDimensionIter):
	   calculateFSBTargetChunkWithVerticalChange
	   (fSBSize, chunkUpdateDimensionIter)};
	// Calculate index of potential chunk to be copied into FSB.
	const std::string chunkKey
	  {calculatePotentialChunkKeyForChunkToGoInFSB
	   (horizontal, chunkUpdateDimensionIter)};


        // int i = 0;
	// for(auto c: *chunk)
	//   {
	//     if(i % (firstStageBufferType::fSBXChunks * chunkSize.x) == 0)
	//       std::cout<<'\n';
	//     std::cout<<(char)(c % 159);
	//     i++;
	//   }
	//     exit(-1);
	//   }

	  

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

    // /* We must update these separately otherwise we will forget the correct
    //    last position for the axis not being updated. */
    // if(horizontal)
    //   {
    // 	firstStageBuffer.lastUpdatedPosition.x =
    // 	  firstStageBuffer.viewPortPosition.x;
    //   }
    // else
    //   {
    // 	firstStageBuffer.lastUpdatedPosition.y =
    // 	  firstStageBuffer.viewPortPosition.y;
    //   }



    if(horizontal)
      {
	firstStageBuffer.lastUpdatedPosition.x =	 
	  ((firstStageBuffer.viewPortPosition.x < 0 ?
	    (firstStageBuffer.viewPortPosition.x - chunkSize.x +1) :
	    firstStageBuffer.viewPortPosition.x)
	   / chunkSize.x) * chunkSize.x;
      }
    else
      {
	firstStageBuffer.lastUpdatedPosition.y =
	  ((firstStageBuffer.viewPortPosition.y < 0 ?
	    (firstStageBuffer.viewPortPosition.y - chunkSize.y +1) :
	    firstStageBuffer.viewPortPosition.y)
	   / chunkSize.y) * chunkSize.y;
      }

    //     /* We must update these separately otherwise we will forget the correct
    //    last position for the axis not being updated. */
    // if(horizontal)
    //   {
    // 	/* We have to make sure the last position is chunk aligned so we can't
    // 	   get out of sync. */
    // 	firstStageBuffer.lastUpdatedPosition.x =
    // 	  (firstStageBuffer.viewPortPosition.x % chunkSize.x) * chunkSize.x;
    //   }
    // else
    //   {
    // 	/* We have to make sure the last position is chunk aligned so we can't
    // 	   get out of sync. */
    // 	firstStageBuffer.lastUpdatedPosition.y =
    // 	  (firstStageBuffer.viewPortPosition.y % chunkSize.y) * chunkSize.y;
    //   }
  }

  
  /* Calculates the coordinates in the FSB that a chunk should be copied to
     when the view port has moved chunkSize.x in the x dimension. Where
     yChunkOffset is used as an offset (in chunks) in the y dimension. Handles
     wrap around. I.e. if the view port position is (0,0) and the old position
     was (0, 170) (where 170 is the size of a chunk in the x dimension in this
     example) then the set of chunks to be updated given 5 calls with
     yChunkOffset being set to (0, 1, 2, 3, 4) for each respective call, should
     be (3, 3), (4, 3), (0, 3), (1, 3), (2, 3). */
  yx calculateFSBTargetChunkWithHorizontalChange
  (const yx fSBSize, const int yChunkOffset) const
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
      {(((firstStageBuffer.lastUpdatedPosition.y -
	  (firstStageBuffer.lastUpdatedPosition.y < 0 ? yHeight -1: 0)) %
	 fSBSize.y) / chunkSize.y +
	(firstStageBuffer.fSBYChunks / 2 +
	 (firstStageBuffer.fSBYChunks & 1)) +
	yChunkOffset)};
    targetChunk.y = targetYPreWrap % firstStageBuffer.fSBYChunks;
    /* Maybe fix negative and wrap around in y... */
    targetChunk.y = targetChunk.y < 0 ?
      firstStageBuffer.fSBYChunks + targetChunk.y: targetChunk.y;

    // if(targetChunk.y == 0)
    //   {
    // 	 nodelay(stdscr, FALSE);
    // 		attron(COLOR_PAIR(56));
    // 		mvprintw(47, 0, concat("H chunkKey = ", chunkKey, "\ttargetChunk = ", targetChunk).c_str());
    // refresh();
    // sleep(70);
    // getch();
    //  nodelay(stdscr, TRUE);
    //   }

    return targetChunk;
  }


  yx calculateFSBTargetChunkWithVerticalChange
  (const yx fSBSize, const int xChunkOffset) const
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
      {(((firstStageBuffer.lastUpdatedPosition.x -
	  (firstStageBuffer.lastUpdatedPosition.x < 0 ? xWidth -1: 0)) %
	 fSBSize.x) / chunkSize.x +
	(firstStageBuffer.fSBXChunks / 2 +
	 (firstStageBuffer.fSBXChunks & 1)) +
	xChunkOffset)};
    targetChunk.x = targetXPreWrap % firstStageBuffer.fSBXChunks;
    /* Maybe fix negative and wrap around in x... */
    targetChunk.x = targetChunk.x < 0 ?
      firstStageBuffer.fSBXChunks + targetChunk.x: targetChunk.x;


    // if(targetChunk.y == 0)
    //   {
    // 	nodelay(stdscr, FALSE);
    // 		attron(COLOR_PAIR(20));
    // mvprintw(47, 0, concat("V chunkKey = ", chunkKey, "\ttargetChunk = ", targetChunk).c_str());
    //     refresh();
    // sleep(70);
    // getch();
    // nodelay(stdscr, TRUE);
    //   }
    

    return targetChunk;
  }

  
  /* Where horizontal determines whether or not to use chunkUpdateDimensionIter
     as a component of y or x when creating the key. Horizontal should be set
     to true when the triggering change in view port position was in the x
     dimension. */
  std::string calculatePotentialChunkKeyForChunkToGoInFSB
  (const bool horizontal, const int chunkUpdateDimensionIter) const
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
	  yx{firstStageBuffer.lastUpdatedPosition.y +
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
	     firstStageBuffer.lastUpdatedPosition.x +
	     ((chunkUpdateDimensionIter -
	       (firstStageBuffer.fSBXChunks  / 2)) * chunkSize.x)};
      }
  
    return createChunkCoordKeyFromCharCoord(chunkCoord);
  }
  
protected:
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
    
    // static long a {};
    // a++;
  //   endwin();
  // std::cout<<"updateFirstStageBuffer() called "<<a<<" times.!\n";

    
    // x > y ? x - y : y - x. Get distance between two numbers on number line.
    if(updatedNeededInXDimension() && updatedNeededInYDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks});
      }
    else if(updatedNeededInXDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks}, true);

 	// if(a > 8)
	//   {
	// attron(COLOR_PAIR(56));
	// for(int y {}; y < (chunkSize.y /3); y++)
	//   {
	//     for(int x {}; x < (chunkSize.x / 3); x++)
	//       {
	// 	mvprintw(y, x, "U");
	//       }
	//   }
	// sleep(2000);
	// refresh();
	  // }
      }
    else if(updatedNeededInYDimension())
      {
	updateFirstStageBuffer
	  (yx{chunkSize.y * firstStageBuffer.fSBYChunks,
	      chunkSize.x * firstStageBuffer.fSBXChunks}, false);

	// if(a > 8)
	//   {
	// attron(COLOR_PAIR(20));
	// for(int y {}; y < (chunkSize.y / 3); y++)
	//   {
	//     for(int x {}; x < (chunkSize.x / 3); x++)
	//       {
	// 	mvprintw(y, x, "U");
	//       }
	//   }
	// sleep(2000);
	// refresh();
	  // }
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
  const char * fileName;
  
  chunk(const yx chunkSizeIn, const int fSBSizeInChunks,
	const chunkElementType missingChunkFiller, const char fileName []):
    chunkSize(chunkSizeIn.y, chunkSizeIn.x),
    firstStageBuffer(chunkSizeIn, fSBSizeInChunks),
    fileName(fileName),
    MISSING_CHUNK_FILLER(missingChunkFiller)
  {
  }
  
  
  ssize_t numberOfChunks() const
  {
    return chunkMap.size();
  }

  
  bool keyExists(const std::string & key) const
  {
    return !(chunkMap.find(key) == chunkMap.end());
  }
  
  
  /* To avoid visual artefacts the whole 1st stage buffer needs to be
     initialised.
     Should be called once initial player position is known, but before the main
     game loop. */
  void initFirstStageBuffer(const yx initialViewPortPos)
  {
    /* We need to align the position we start filling the chunks from to the
       start of a chunk. Here a coordinate of -1 should snap to -48 (the start
       of the first negative chunk) and -49 should snap to -96. But 1 should
       snap to 0 and 48 should snap to 48 (the start of the second positive
       chunk.) */
    const yx initialCenterPos
      {((initialViewPortPos.y < 0 ?
	(initialViewPortPos.y - chunkSize.y +1) : initialViewPortPos.y)
	/ chunkSize.y) * chunkSize.y,
       ((initialViewPortPos.x < 0 ?
	 (initialViewPortPos.x - chunkSize.x +1) : initialViewPortPos.x)
	/ chunkSize.x) * chunkSize.x};

    // endwin();
    // std::cout<<"initialViewPortPos = "<<initialViewPortPos
    // 	     <<"initialCenterPos = "<<initialCenterPos<<'\n';
    // exit(-1);

    int chunkIter {-(firstStageBuffer.fSBXChunks / 2)};
    firstStageBuffer.lastUpdatedPosition =
      yx{initialCenterPos.y,
	 initialCenterPos.x + chunkSize.x * (chunkIter -1)};

    /* Note that FSBXChunks should be odd. Fill columns (of height fSBYChunks)
       of chunks from left to right. */
    for( ; chunkIter <= (firstStageBuffer.fSBXChunks / 2);
	++chunkIter)
      {
	firstStageBuffer.viewPortPosition =
	  yx{initialCenterPos.y, initialCenterPos.x + chunkSize.x * chunkIter};
	updateFirstStageBuffer();
      }

    //     std::ofstream out("out.txt");
    // for(int i {};
    //     i < (firstStageBufferType::fSBXChunks * chunkSize.x *
    // 	   firstStageBufferType::fSBYChunks * chunkSize.y);
    //     ++i)
    //   {
    //     if(i % (firstStageBufferType::fSBXChunks * chunkSize.x) == 0)
    // 	out<<'\n';
    //     out<<(char)(firstStageBuffer.buffer[i] % 159);
    //   }
    // out.close();
    // exit(-1);

    firstStageBuffer.viewPortPosition = initialViewPortPos;
  }


  /* Updates the view port pos if the relative player position has changed
     enough since the last time the view port pos was updates. */
  bool updateViewPortPosition
  (const yx playerMovementAreaPadding, const yx playerPos,
   const yx playerMaxBottomRightOffset)
  { 
    // // Tests for if player is above or to the left of padding.
    // std::function<bool(bool)> testPaddingDirectionInDimension =
    //   [&](bool yDimension)
    //   {
    // 	if(yDimension)
    // 	  {
    // 	    return playerPos.y - playerMovementAreaPadding.y < 0;
    // 	  }
    // 	else
    // 	  {
    // 	    return playerPos.x - playerMovementAreaPadding.x < 0;
    // 	  }
    //   };
    // /* Test if player is if player is outisde of zone inside of padding in the y
    //    or x dimension */
    // std::function<bool(bool)> testPaddingInDimension = [&](bool yDimension)
    // {
    //   if(yDimension)
    // 	{ 
    // 	  return (testPaddingDirectionInDimension(true) ||
    // 		  (playerPos.y + playerMaxBottomRightOffset.y + 1) >
    // 		  (chunkSize.y - playerMovementAreaPadding.y));
    // 	}
    //   else
    // 	{
    // 	  return (testPaddingDirectionInDimension(false) ||
    // 		  (playerPos.x + playerMaxBottomRightOffset.x + 1) >
    // 		  (chunkSize.x - playerMovementAreaPadding.x));
    // 	}
    // };

    // bool viewPortPosChanged {false};
    
    // // Test in Y dimension.
    // if(testPaddingInDimension(true))
    //   {
    // 	/* The top of the player is in or above the top padding
    // 	   or the bottom of the player player is in or below the bottom
    // 	   padding.
    // 	   UPDATE X DIMENSION OF THE VIEW PORT POSITION. */
    // 	if(testPaddingDirectionInDimension(true))
    // 	  {
    // 	    // The player is in or above the top padding.
    // 	    /* Here if the playerPos is positive we want to subtract it from
    // 	       the padding, but if it is negative we wan't to add it. Luckily
    // 	       simply subtracting it from the padding will do the job. */
    // 	    firstStageBuffer.viewPortPosition.y -=
    // 	      playerMovementAreaPadding.y - playerPos.y;
    // 	    viewPortPosChanged = true;
    // 	  }
    // 	else
    // 	  {
    // 	    // The player is in or below the bottom padding.
    // 	    firstStageBuffer.viewPortPosition.y +=
    // 	      abs(playerPos.y) - (chunkSize.y - playerMovementAreaPadding.y);
    // 	    viewPortPosChanged = true;
    // 	  }
    //   }
    // // Test in X dimension.
    // if(testPaddingInDimension(false))
    //   {
    // 	// exit(-1);
    // 	/* The left of the player is in or to the left of the left padding
    // 	   or the right of the player is in or to the right of the right
    // 	   padding.
    // 	   UPDATE X DIMENSION OF THE VIEW PORT POSITION. */
    // 	if(testPaddingDirectionInDimension(false))
    // 	  {
    // 	    // The player is in the left padding.
    // 	    // firstStageBuffer.viewPortPosition.x -=
    // 	    //   abs((firstStageBuffer.viewPortPosition.x +
    // 	    // 	   playerMovementAreaPadding.x) - playerPos.x);
    // 	    firstStageBuffer.viewPortPosition.x -=
    // 	      playerMovementAreaPadding.x - playerPos.x;
    // 	    viewPortPosChanged = true;
    // 	  }
    // 	else
    // 	  {
    // 	    // The player is in the right padding.
    // 	    // firstStageBuffer.viewPortPosition.x +=
    // 	    //   abs((firstStageBuffer.viewPortPosition.x + chunkSize.x
    // 	    // 	   - playerMovementAreaPadding.x) -
    // 	    // 	  (playerMaxBottomRightOffset.x + 1 + playerPos.x));
    // 	    firstStageBuffer.viewPortPosition.x +=
    // 	      abs(playerPos.x) - (chunkSize.x - playerMovementAreaPadding.x);
    // 	    viewPortPosChanged = true;
    // 	  }
    //   }

    static int a {};
    static bool incA {true};
        static int b {};
    static bool incB {true};
    
    static bool dimension {true};

    // if(dimension)
    //   {
	if(a > 680)
	  {
	    incA = false;
	  }
	if(a < 1)
	  {
	    incA = true;
	  }
	if(incA)
	  {
	    a++;
	    firstStageBuffer.viewPortPosition.x++;
	  }
	else
	  {
	    a--;
	    firstStageBuffer.viewPortPosition.x--;
	  }
    //   }
    // else
    //   {
	if(b > 192)
	  {
	    incB = false;
	  }
	if(b < 1)
	  {
	    incB = true;
	  }
	if(incB)
	  {
	    b++;
	    firstStageBuffer.viewPortPosition.y++;
	  }
	else
	  {
	    b--;
	    firstStageBuffer.viewPortPosition.y--;
	  }
    //   }
    // dimension = !dimension;
    
    
    // firstStageBuffer.viewPortPosition.y;
    
    // return viewPortPosChanged;
    return true;
  }


  void updateViewPortPosition(const yx newPosition)
  {
    firstStageBuffer.viewPortPosition = newPosition;
  }


  yx getViewPortPosition() const {return firstStageBuffer.viewPortPosition;}
};


#endif
