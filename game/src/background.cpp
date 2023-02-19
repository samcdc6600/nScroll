#include "include/background.hpp"
#include "include/colorS.hpp"
#include "include/collapse.hpp"
#include <cstdlib>


/* This is used to fill target chunks in the FSDB when no chunk is found for a
   given key. */
constexpr short COLOR_BLACK_BLACK_NO {1};


backgroundData::backgroundType backgroundData::loadAndInitialiseBackground
(const char bgFileName []) const
{
  std::string rawLevelBackground {};
  backgroundType background;
    
  loadFileIntoString
    (bgFileName, rawLevelBackground,
     concat("trying to read ", BACKGROUND_FILE_EXTENSION, " file"));
  initialiseBackground(false, bgFileName, rawLevelBackground, background);
  /* This will result in a deep copy when this function is called from the
     constructor. However this only happens once per level load. */
  return background;
}


void backgroundData::initialiseBackground
(const bool rawData, const char bgFileName [], const std::string & bgData,
 backgroundType & background) const
{
  
  if(rawData)
    {			/* This is probably a file generated by the
			   level editor. */
      // /* Read background header, which should contain a 2-tuple which
      //    specifies the size of the chunks in the file in their y and x
      //    dimensions respectively. We don't technically need this number
      //    since the yHeight and xWidth (which are defined in utils.hpp) are
      //    used for this purpose. However we might change this in the future
      //    and having these numbers in level background files make it more
      //    explicit when looking at those files. This should also allow for
      //    the possibility of levels with a smaller view port than that of
      //    the window. Although changes will almost certainly have to be made
      //    to the code to support that feature. */
      // const yx chunkSize {getChunkSize(bgData)};
      yx chunkCoord {};
      std::string chunk {};
      backgroundChunk rawChunk {};
      std::string::const_iterator buffPos {std::begin(bgData)};
	  
      while(true)
	{
	  ssize_t chunksReadIn {};
	  /* Each chunk should have a header that contains it's coordinates
	     in the level. The unit of the coordinates should be chunks. So
	     for a chunk that starts at (0, 170) in character coordinates
	     (assuming the size of chunks for this file are 170 in the x
	     dimension), the coordinate in the header would be (0, 1) and
	     for (0, 340) it would be (0, 2), etc... */
	  if(getChunkCoordinate
	     (bgData, buffPos,
	      concat("trying to read coord no. ", chunksReadIn, " from ",
		     BACKGROUND_FILE_EXTENSION, " file \"", bgFileName, "\""),
	      chunkCoord))
	    {
	      // Get chunk from background data read from file.
	      // Collapse chunk and return in rawChunk.
	      // collapse(chunk, rawChunk);

	      /* Store rawChunk in this.background with a key that should be
		 calculated according to the following:
		 Concatenate (y / chunkSize.y) and (x / chunkSize.y) and use as
		 index into map. Then
		 (y % (chunkSize.y * 3)) * chunkSize.x + (x % (chunkSize.x * 3))
		 can be used to index into the backgroundChunk returned from
		 the map (as the stage 1 draw buffer will be 3 by 3
		 chunks.)
	      */

	      chunksReadIn++;
	      // GetChunk() clears it's argument (chunk).
	      rawChunk.clear();
	    }
	  else
	    {
	      break;
	    }
	}
    }
  else
    {
      // const yx chunkSize {getChunkSize(bgData)};
      yx chunkCoord {};
      std::string chunk {};
      backgroundChunk rawChunk {};
      std::string::const_iterator buffPos {std::begin(bgData)};
      ssize_t chunksReadIn {};

      while(true)
	{
	  /* Each chunk should have a header that contains it's coordinates
	     in the level. The unit of the coordinates should be chunks. So
	     for a chunk that starts at (0, 170) in character coordinates
	     (assuming the size of chunks for this file are 170 in the x
	     dimension), the coordinate in the header would be (0, 1) and
	     for (0, 340) it would be (0, 2), etc... */
	  if(getChunkCoordinate
	     (bgData, buffPos,
	      concat("trying to read coord no. ", chunksReadIn, " from ",
		     BACKGROUND_FILE_EXTENSION, " file \"", bgFileName, "\""),
	      chunkCoord))
	    {
	      skipSpaceUpToNextLine
		(bgData, buffPos,
		 concat("Error: trying to read chunk no. ", chunksReadIn,
			". Expected '\\n' after reading chunk coordinate "
			"from ", BACKGROUND_FILE_EXTENSION, " file \"",
			bgFileName, "\". Encountered other character or EOF."));
	      // Get chunk from background data read from file.
	      getChunk(bgData, buffPos,
		       concat("trying to read in chunk no. ", chunksReadIn,
			      " from ", BACKGROUND_FILE_EXTENSION, " file \"",
			      bgFileName, "\"."), chunk, chunkSize);
	      // Collapse chunk and return in rawChunk.
	      collapse(chunk, rawChunk);
	      verifyCollapsedChunkSize(rawChunk, chunksReadIn, true);
	      insertChunk(chunkCoord, rawChunk, chunksReadIn, bgFileName,
			  background);

	      chunksReadIn++;
	      // GetChunk() clears it's argument (chunk).
	      rawChunk.clear();
	    }
	  else
	    {
	      break;
	    }
	}
    }
}


// bool backgroundData::getChunkCoordinate(const std::string & bgData,
// 			std::string::const_iterator & buffPos,
// 			const std::string & eMsg, yx & chunkCoord) const
// {
//   if(buffPos != std::end(bgData))
//     {
//       readSingleCoordSectionInNNumbersNoSkpSp
// 	(bgData, buffPos, eMsg, &chunkCoord);

//       return true;
//     }
//   else
//     {
//       return false;
//     }
// }


// void backgroundData::getChunk(const std::string & bgData,
// 	      std::string::const_iterator & buffPos, const std::string & eMsg,
// 	      std::string & chunk) const
// {
//   chunk.clear();

//   int lnCount {};
//   for( ; lnCount < chunkSize.y && buffPos != std::end(bgData); )
//     {
//       char newCh {};
//       newCh = *buffPos++;
//       chunk += newCh;
//       lnCount += (newCh == '\n' ? 1: 0);
//     }
//   if(lnCount != chunkSize.y)
//     {
//       exit(eMsg, ERROR_BACKGROUND);
//     }
// }


void backgroundData::verifyCollapsedChunkSize
(const backgroundChunk & rawChunk, const ssize_t chunksReadIn,
 const bool attemptedCompression) const
{
  if(rawChunk.size() != (size_t)(chunkSize.y * chunkSize.x))
    {      
      exit(concat
	   ("Error: chunk no. ", chunksReadIn, " is the wrong size (",
	    rawChunk.size(), ")",
	    (attemptedCompression ? " after being compressed.": "."),
	    "Expected size of ", chunkSize.y * chunkSize.x, " (",
	    chunkSize.y, " * ", chunkSize.x, ")."),
	   ERROR_BACKGROUND);
    }
}


/*void backgroundData::checkForChunk(const yx coord)
{
}*/


/*bool backgroundData::getChunk
    (const yx & coord, backgroundData::backgroundChunk * chunk) const
{
  const std::string key {createChunkCoordKey(coord)};
  backgroundChunk & ret {};

  // try
  //   {
  //     ret = background.at(key);
  //   }
  // catch()
  //   {

  //   }
  return ret;
  }*/


#include <curses.h>
#include <iostream>

void backgroundData::initFirstStageDrawBuffer(const yx playerPos)
{
  // CALCULATE VIEW PORT POSITION FROM PLAYER POS (MAYBE WE SHOULD PASS THE
  // BACKGROUND OBJECT INTO THE RULES OBJECT VIA THE PHYSICS FUNCTION AND THEN
  // THE RULES OBJECT CAN CALL A MEMBER FUNCTION ON THIS OBJECT TO CALCULATE
  // WHEN THE VIEW PORT SHOULD BE UPDATED (AS OPPOSED TO THE RULES OBJECT DOING
  // IT DIRECTLY.) THIS WAY WAS CAN PROBABLY DUPLICATE THE CODE NEEDED HERE FOR
  // CALCULATING HOW TO UPDATE THE VIEW PORT POSITION FROM THE PLAYER
  // POSITION. MAYBBE WE SHOULD ALSO JUST MAKE THIS FUNCTION CALL INITLK
  // SAJFLKJDSAF LKJ... WE WILL HAVE TO THINK ABOUT IT MORE!
  /* Concatenate (y / chunkSize.y) and (x / chunkSize.y) and use as index into
     map. Then (y % (chunkSize.y * fSDBYChunks)) * chunkSize.x + (x %
     (chunkSize.x * fSDBXChunks)) can be used to index into the backgroundChunk
     returned from the map (as the stage 1 draw buffer will be fSDBYChunks by
     fSDBXChunks chunks.) */

  const yx initialViewPortPosition {playerPos.y, playerPos.x + chunkSize.x * 4};
  firstStageDrawBuffer.lastUpdatedPosition = initialViewPortPosition;
  firstStageDrawBuffer.viewPortPosition =
    yx{playerPos.y, playerPos.x + chunkSize.x * 3};
  
  updateFirstStageDrawBuffer();

  firstStageDrawBuffer.viewPortPosition =
    yx{playerPos.y, playerPos.x + chunkSize.x * 2};

    updateFirstStageDrawBuffer();

  firstStageDrawBuffer.viewPortPosition = playerPos;


  // const yx initialViewPortPosition {playerPos.y + chunkSize.y * 4, playerPos.x};
  // firstStageDrawBuffer.lastUpdatedPosition = initialViewPortPosition;
  // firstStageDrawBuffer.viewPortPosition =
  //   yx{playerPos.y + chunkSize.y * 3, playerPos.x};
  
  // updateFirstStageDrawBuffer();

  // firstStageDrawBuffer.viewPortPosition =
  //   yx{playerPos.y + chunkSize.y * 2, playerPos.x};

  // updateFirstStageDrawBuffer();

  // firstStageDrawBuffer.viewPortPosition = playerPos;
}


void backgroundData::updateFirstStageDrawBuffer()
{ 
  if(abs(firstStageDrawBuffer.lastUpdatedPosition.x -
	 firstStageDrawBuffer.viewPortPosition.x) > chunkSize.x -1)
    {
      updateFirstStageDrawBuffer(true);
    }
  else if(abs(firstStageDrawBuffer.lastUpdatedPosition.y -
	      firstStageDrawBuffer.viewPortPosition.y) > chunkSize.y -1)
    {
      updateFirstStageDrawBuffer(false);
    }
}


void backgroundData::updateFirstStageDrawBuffer(const bool horizontal)
{
    /*
    The first stage draw buffer is a 5 by 5 array of chunks.
    When the delta between the last updated position and the new position
    (viewPortPosition) have diverged by the dimension of one chunk (note that
    this differs depending on the axis.) The chunks two chunks ahead of the
    viewPortPosition (in y or x) are updated. 

    The diagram below shows the position of the last updated position and the
    current position upon entering this function and the chunks that will be
    updated because the current position is one chunk ahead of the last updated
    position.
    Where c is a chunk that will stay the same L is the last updated position, N
    is the new position and X is a chunk that needs to be updated because the
    delta between L and N is one chunks worth in terms of y or x (X in this
    case.)
    
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
    NOTE THAT WE ARE INITIALLY GOING TO IMPLEMENT THIS FUNCTION SO THAT IT WILL
    BE RUN IN THE SAME THREAD AS THE REST OF THE GAME. HOWEVER WE PLAN TO MOVE
    TO IT'S OWN THREAD. WHEN AN UPDATE NEEDS TO BE DONE IT CAN BE DONE IN
    PARALLEL WITH THE REST OF THE GAME LOGIC. THE THREAD DOING TO UPDATING WILL
    BE ABLE TO UPDATE FOR A FULL CHUNKS WORTH OF MOVEMENT IN THE MAIN THREAD
    (BECAUSE THERE IS A COLUMN (OR ROW) OF CHUNKS IN BETWEEN THE CURRENT
    POSITION AND THE COLUMN (OR ROW) BEING UPDATED. IF THERE IS MORE MOVEMENT
    THAN THAT THEN THE MAIN THREAD WILL NEED TO BE PAUSED TO WAIT FOR THE
    UPDATING THREAD. HOWEVER WE THINK THAT THIS SHOULD NOT HAPPEN UNDER NORMAL
    CIRCUMSTANCES AS ONE CHUNKS WORTH OF MOVEMENT IN THE WORSE CASE (IN THE Y
    AXIS) IS 40 CHUNKS WORTH OF FRAMES AND THE ENGINE SLEEPS FOR SOME TIME
    BETWEEN EACH FRAME. CONSIDERING THAT 5 CHUNKS ARE UPDATED FOR EACH UPDATE
    THIS MEANS THAT THE MAIN THREAD WILL BE COPYING 35 CHUNKS WORTH OF DATA MORE
    THAN THE UPDATING THREAD. HOWEVER THE UPDATING THREAD WILL NEED TO DO 5
    LOOKUPS TO FIND THE THREADS AND THESE LOOKUPS WILL PROBABLY BE THE SLOWEST
    OPERATIONS PERFORMED BY THE UPDATING THREAD.
  */
  
  /* NOTE THAT THE CODE HERE WILL ONLY WORK CORRECTLY WHEN fSDBYChunks AND
     fSDBXChunks ARE UNEVEN. HOWEVER THEY SHOULD BE. SO THIS SHOULDN'T BE A
     PROBLEM. */
  // Size of the first stage draw buffer in characters.
  const yx fSDBSize {chunkSize.y * firstStageDrawBuffer.fSDBYChunks,
		     chunkSize.x * firstStageDrawBuffer.fSDBXChunks};
  
  // Iterate over chunks to be updated in the FSDB.
  for(int chunkUpdateDimensionIter {};
      chunkUpdateDimensionIter < firstStageDrawBuffer.fSDBYChunks;
      ++chunkUpdateDimensionIter)
    {
      const yx fSDBTargetChunk
	{horizontal ?
	 calculateFSDBTargetChunkWithHorizontalChange
	 (fSDBSize, chunkUpdateDimensionIter):
	 calculateFSDBTargetChunkWithVerticalChange
	 (fSDBSize, chunkUpdateDimensionIter)};
      // Calculate index of potential chunk to be copied into FSDB.
      const std::string chunkKey
	{calculatePotentialChunkKeyForChunkToGoInFSDB
	 (horizontal, chunkUpdateDimensionIter)};

      // Try to get chunk and then perform the copy to the FSDB.
      try
	{
	  // At() will throw an exception if the key isn't found.
	  const backgroundChunk * chunk {&background.at(chunkKey)};

	  // Iterate over lines of target chunk in the FSDB.
	  for(int yIter {}; yIter < chunkSize.y; ++yIter)
	    {
	      for(int xIter {}; xIter < chunkSize.x; ++xIter)
		{
		  firstStageDrawBuffer.buffer
		    [(fSDBTargetChunk.y * fSDBSize.x * chunkSize.y) +
		     yIter * fSDBSize.x +
			     
		     ((fSDBTargetChunk.x * chunkSize.x)
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
		  firstStageDrawBuffer.buffer
		    [(fSDBTargetChunk.y * fSDBSize.x * chunkSize.y) +
		     yIter * fSDBSize.x +			     
		     ((fSDBTargetChunk.x * chunkSize.x)
		      + xIter)] =
		    // Fill with black spaces.
		    (MONO_CH_MAX +1) * COLOR_BLACK_BLACK_NO + ' ';
		}
	    }
	}
    }

  firstStageDrawBuffer.lastUpdatedPosition =
    firstStageDrawBuffer.viewPortPosition;
}


yx backgroundData::calculateFSDBTargetChunkWithHorizontalChange
(const yx fSDBSize, const int yChunkOffset) const
{
  /* Y should change each iteration of the loop in the calling function, but x
     shouldn't. We have to wrap around in y if y >=
     firstStageDrawBuffer.fSDBYChunks. */
  yx targetChunk {0, (firstStageDrawBuffer.viewPortPosition.x % fSDBSize.x) /
		  chunkSize.x};
  /* Add offset in the x dimension based on the direction the view port has
     moved. */
  targetChunk.x += firstStageDrawBuffer.viewPortPosition.x >
    firstStageDrawBuffer.lastUpdatedPosition.x ?
    firstStageDrawBuffer.fSDBXUpdateOffset:
    - firstStageDrawBuffer.fSDBXUpdateOffset;
  /* Maybe wrap around in x... NOTE THAT THE PLAYER SHOULD NOT BE ABLE TO HAVE A
     NEGATIVE COORDINATE AND AS SUCH THE FOLLOWING CODE SHOULDN'T BE AN ISSUE
     BECAUSE WHEN targetChunk.x IS NEGATIVE |targetChunk.x| SHOULD NEVER BE MORE
     THAN firstStageDrawBuffer.fSDBXChunks. */
  targetChunk.x %= firstStageDrawBuffer.fSDBXChunks;
  targetChunk.x = targetChunk.x < 0 ?
    firstStageDrawBuffer.fSDBXChunks + targetChunk.x: targetChunk.x;
  // FirstStageDrawBuffer.fSDBYChunks & 1 returns 1 if fSDBYChunks is even.
  const int targetYPreWrap {((firstStageDrawBuffer.viewPortPosition.y %
			      fSDBSize.y) / chunkSize.y +
			     (firstStageDrawBuffer.fSDBYChunks / 2 +
			      (firstStageDrawBuffer.fSDBYChunks & 1)) +
			     yChunkOffset)};
  targetChunk.y = targetYPreWrap % firstStageDrawBuffer.fSDBYChunks;

  return targetChunk;
}


yx backgroundData::calculateFSDBTargetChunkWithVerticalChange
(const yx fSDBSize, const int xChunkOffset) const
{
  /* X should change each iteration of the loop in the calling function, but y
     shouldn't. We have to wrap around in x if x >=
     firstStageDrawBuffer.fSDBXChunks. */
  yx targetChunk {(firstStageDrawBuffer.viewPortPosition.y % fSDBSize.y) /
		  chunkSize.y, 0};
  /* Add offset in the y dimension based on the direction the view port has
     moved. */
  targetChunk.y += firstStageDrawBuffer.viewPortPosition.y >
    firstStageDrawBuffer.lastUpdatedPosition.y ?
    firstStageDrawBuffer.fSDBYUpdateOffset:
    - firstStageDrawBuffer.fSDBYUpdateOffset;
  /* Maybe wrap around in y... NOTE THAT THE PLAYER SHOULD NOT BE ABLE TO HAVE A
     NEGATIVE COORDINATE AND AS SUCH THE FOLLOWING CODE SHOULDN'T BE AN ISSUE
     BECAUSE WHEN targetChunk.y IS NEGATIVE |targetChunk.y| SHOULD NEVER BE MORE
     THAN firstStageDrawBuffer.fSDBYChunks. */
  targetChunk.y %= firstStageDrawBuffer.fSDBYChunks;
  targetChunk.y = targetChunk.y < 0 ?
    firstStageDrawBuffer.fSDBYChunks + targetChunk.y: targetChunk.y;
  // FirstStagedrawbuffer.fSDBYChunks & 1 returns 1 if fSDBYChunks is even.
  const int targetXPreWrap {((firstStageDrawBuffer.viewPortPosition.x %
			      fSDBSize.x) / chunkSize.x +
			     (firstStageDrawBuffer.fSDBXChunks / 2 +
			      (firstStageDrawBuffer.fSDBXChunks & 1)) +
			     xChunkOffset)};
  targetChunk.x = targetXPreWrap % firstStageDrawBuffer.fSDBXChunks;

  return targetChunk;
}


std::string backgroundData::calculatePotentialChunkKeyForChunkToGoInFSDB
(const bool horizontal, const int chunkUpdateDimensionIter) const
{
  /* Note here that we account for the direction of movement when calculating
     the y or x coordinate based on the value of horizontal. */

  yx chunkCoord {};

  if(horizontal)
    {
      chunkCoord =
	yx{firstStageDrawBuffer.viewPortPosition.y +
	   ((chunkUpdateDimensionIter -
	     (firstStageDrawBuffer.fSDBYChunks / 2)) * chunkSize.y),
	   firstStageDrawBuffer.viewPortPosition.x +
	   (firstStageDrawBuffer.viewPortPosition.x >
	     firstStageDrawBuffer.lastUpdatedPosition.x ?
	     firstStageDrawBuffer.fSDBXUpdateOffset:
	     -firstStageDrawBuffer.fSDBXUpdateOffset) * chunkSize.x};
    }
  else 
    {
      chunkCoord =
	yx{(firstStageDrawBuffer.viewPortPosition.y +
	    (firstStageDrawBuffer.viewPortPosition.y >
	     firstStageDrawBuffer.lastUpdatedPosition.y ?
	     firstStageDrawBuffer.fSDBYUpdateOffset:
	     -firstStageDrawBuffer.fSDBYUpdateOffset) * chunkSize.y),
	firstStageDrawBuffer.viewPortPosition.x +
	((chunkUpdateDimensionIter -
	  (firstStageDrawBuffer.fSDBXChunks  / 2)) * chunkSize.x)};
    }
  
  return createChunkCoordKeyFromCharCoord(chunkCoord);
}


void backgroundData::updateSecondStageDrawBuffer
(backgroundData::drawBufferType *  secondStageDrawBuffer)
{ 
  // Size of the first stage draw buffer in characters.
  const yx fSDBSize {chunkSize.y * firstStageDrawBuffer.fSDBYChunks,
		     chunkSize.x * firstStageDrawBuffer.fSDBXChunks};  
  
  for(int yIter {}; yIter < chunkSize.y; ++yIter)
    {
      for(int xIter {}; xIter < chunkSize.x; ++xIter)
	{
	  secondStageDrawBuffer[(yIter * chunkSize.x) + xIter] =
	    firstStageDrawBuffer.buffer
	    [((firstStageDrawBuffer.viewPortPosition.y + yIter) %
	       (chunkSize.y * firstStageDrawBuffer.fSDBYChunks)) *
	     fSDBSize.x +
	     
	     ((firstStageDrawBuffer.viewPortPosition.x + xIter) %
	       (chunkSize.x * firstStageDrawBuffer.fSDBXChunks))];
	}  
    }
}


void backgroundData::updateViewPortPosition
(const yx playerMovementAreaPadding, const yx playerPosTopLeft,
 const yx playerPosBottomRight)
{
  std::function<bool()> testPaddingInX = [&]()
  {
    return playerPosTopLeft.x <=
      (firstStageDrawBuffer.viewPortPosition.x +
       playerMovementAreaPadding.x) &&
      playerPosBottomRight.x >=
      (firstStageDrawBuffer.viewPortPosition.x + chunkSize.x
       - playerMovementAreaPadding.x);
  };
  
  
  if(playerPosTopLeft.y <
     (firstStageDrawBuffer.viewPortPosition.y + playerMovementAreaPadding.y) &&
     playerPosBottomRight.y >
     (firstStageDrawBuffer.viewPortPosition.y +
      chunkSize.y - playerMovementAreaPadding.y))
    {
      if(testPaddingInX())
	{
	  /* The left of the player is in or to the left of the left padding or
	     the right of the player is in or to the right of the right
	     padding.
	     UPDATE X DIMENSION OF THE VIEW PORT POSITION. */
	}
    }
  else
    {
      /* Top of player is in or above the top padding or the bottom of the
	 player is in or below the bottom padding.
	 Check x dimension... */
      if(testPaddingInX())
	{
	  /* The left of the player is in or to the left of the left padding or
	     the right of the player is in or to the right of the right
	     padding.
	     UPDATE X DIMENSION OF THE VIEW PORT POSITION. */
	}
      // UPDATE Y DIMENSION OF THE VIEW PORT POSITION.
    }
}
