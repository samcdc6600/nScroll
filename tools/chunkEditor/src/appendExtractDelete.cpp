#include <fstream>
#include "include/appendExtractDelete.hpp"

/* Checks for a chunk with coordinates searchCoord in file multiChunkFile. If
   one is found set's duplicatChunkCoordFound to true (sets it to false
   otherwise.) DestChunkSeekPos is set to the start position of the last chunk
   read. So if a chunk is found it will be set to the start of that chunk. */
void checkForDuplicateChunk
(const std::string multiChunkFileName, std::fstream & multiChunkFile,
 const yx searchCoord, bool & duplicatChunkCoordFound,
 std::streampos & destChunkSeekPos, const yx viewPortSize);
/* Essentially does the same thing as checkForDuplicateChunk(), however only
   for multi Bg chunk files. */
bool duplicateCoordInBgDest
  (const std::string multiChunkFileName, std::fstream & multiChunkFile,
   const yx searchCoord, std::streampos & destChunkSeekPos,
   const yx viewPortSize);
bool duplicateCoordInCRDest
(const std::string multiChunkFileName, std::fstream & multiChunkFile,
 const yx searchCoord, std::streampos & destChunkSeekPos,
 const yx viewPortSize);
/* Searches for a background chunk with coord as it's coordinate in file. If the
   coordinate is found in file then the corresponding chunk is stored in
   chunk, chunkSeekPos is set to the start of the chunk (start of coordinates)
   and the function returns true. Otherwise the function returns false.
   Note that file should already be open. And this function won't close it.
   Also note that if a read was attempted past the end of the file (this will be
   the case if no chunk was found or the chunk found was the last chunk in the
   file) file.fail() (this can be reset with file.clear(), which may be needed
   to do further operations on the file) will be true and file.tellg() will be
   -1. */
bool findBgChunk
(const std::string fileName, std::fstream & file,
 backgroundChunkCharType chunk[][xWidth], const yx targetCoord,
 std::streampos & chunkSeekPos, const yx viewPortSize);
bool findCRChunk
(const std::string fileName, std::fstream & file,
 char chunk[][xWidth], const yx targetCoord, std::streampos & chunkSeekPos,
 const yx viewPortSize);
/* Compactant should be open (using std::ios::in | std::ios::out |
   std::ios::binary). */
void compactDestFileInPlace
(const std::string & compactantFileName, std::fstream & compactant,
 std::streampos & chunkStartSeekPos, std::streampos chunkEndSeekPos,
 const yx viewPortSize, const std::string eMsg);
/* Extracts chunk with coordinates coord from multiChunkFileName and saves it
   in singleChunkFile. Exits with error message if no chunk with coord can be
   found in multiChunkFileName or there is some file writing / reading related
   error. If singleChunkFileName already exists it will be overwritten.
   Otherwise it will be created. Note that the file extensions should already
   have been validated to ensure that they are the right type of files and in
   the right order and combination. */
void extract(const std::string multiChunkFileName,
             const std::string singleChunkFileName, const yx coord,
             const yx viewPortSize);


void append(const std::string singleChunkFileName,
	    const std::string multiChunkFileName, const yx viewPortSize)
{
  auto checkForDuplicateChunkLocal =
    [& singleChunkFileName, & multiChunkFileName, viewPortSize]
    (std::fstream & source, std::fstream & dest,
     std::streampos & destChunkSeekPos, yx & sourceCoordFound)
    {
      bool retDuplicatChunkCoordFound {};

      readInChunkCoordFromFile
	(sourceCoordFound, singleChunkFileName, source, true);

      // Seek back to the start of the source file.
      source.seekg(0, std::ios::beg);
      checkForDuplicateChunk
	(multiChunkFileName, dest, sourceCoordFound, retDuplicatChunkCoordFound,
	 destChunkSeekPos, viewPortSize);
      
      return retDuplicatChunkCoordFound;
    };

  // ======================== LOCAL LAMBDA FUNCTIONS END =======================
  // ===========================================================================
  
    std::fstream source(singleChunkFileName, std::ios::in | std::ios::binary);
    std::fstream dest;
    createFileIfNoneExists(multiChunkFileName,
			   concat("trying to append file \"",
				  singleChunkFileName,
				  "\" to file \"", multiChunkFileName, "\""));
    dest.open(multiChunkFileName,
	      std::ios::in | std::ios::out | std::ios::binary);

    if(!source)
      {
	exit(concat("Error: failed to open source file \"", singleChunkFileName,
		    "\" when for appending to file \"", multiChunkFileName,
		    "\"."), ERROR_OPENING_FILE);
      }
    else if(!dest)
      {
	exit(concat("Error: failed to open destination file \"",
		    multiChunkFileName,
		    "\" when trying to append file \"", singleChunkFileName,
		    "\" to it."), ERROR_OPENING_FILE);
      }

    bool duplicatChunkCoordFound {false}, addDuplicateChunk {false};
    std::streampos destChunkSeekPos;
    yx sourceCoord {};

    duplicatChunkCoordFound =
      checkForDuplicateChunkLocal(source, dest, destChunkSeekPos, sourceCoord);

    if(duplicatChunkCoordFound)
      {
	/* We've found a duplicate chunk (ask the user if they wish to
	   overwrite it. */
	int input {};
	addDuplicateChunk = getConfirmation
	  (viewPortSize, helpColorPair, input, subMenuSleepTimeMs,
	   concat
	   ("\t", sourceCoord, " already exists in the file \"",
	    multiChunkFileName,
	    "\". Appending the chunk from file \"", singleChunkFileName, "\" "
	    "will override the existing chunk with the same coordinate in \"",
	    multiChunkFileName, "\". Do you still want to append the chunk "
	    "from \"", singleChunkFileName, "\", y/n?"));
	if(addDuplicateChunk)
	  {
	    // CompactBgDestFileInPlace() closes dest.
	    compactDestFileInPlace
	      (multiChunkFileName, dest, destChunkSeekPos, dest.tellg(),
	       viewPortSize,
	       concat("trying to replace chunk with matching coordinates in \"",
		      multiChunkFileName, "\" with chunk in \"",
		      singleChunkFileName, "\""));
	  }
	    
      }
    else
      {
	dest.close();
      }

    if(!duplicatChunkCoordFound ||
       duplicatChunkCoordFound && addDuplicateChunk)
      {
	/* All writes are to the end of the file in ios::app mode. */
	dest.open(multiChunkFileName, std::ios::in | std::ios::out |
		  std::ios::binary | std::ios::app);
	if(!dest)
	  {
	    exit(concat("Error: failed to open destination file \"",
			multiChunkFileName, "\" (after compaction and "
			"truncation) when trying to append file \"",
			singleChunkFileName, "\" to it."), ERROR_OPENING_FILE);
	  }
	    
	char readBuff[4096];
	while (source.read(readBuff, sizeof(readBuff)))
	  {
	    dest.write(readBuff, sizeof(readBuff));
	  }
	if(source.gcount() > 0)
	  {
	    // Handle end of file (if the file isn't some multiple of readBuff.)
	    dest.write(readBuff, source.gcount());
	  }
      }

    source.close();
    dest.close();
}


void checkForDuplicateChunk
(const std::string multiChunkFileName, std::fstream & multiChunkFile,
 const yx searchCoord, bool & duplicatChunkCoordFound,
 std::streampos & destChunkSeekPos, const yx viewPortSize)
{
  if(checkForPostfix(multiChunkFileName.c_str(), BACKGROUND_FILE_EXTENSION))
    {
      // We're appending to a background file.
      duplicatChunkCoordFound = duplicateCoordInBgDest
	(multiChunkFileName, multiChunkFile, searchCoord, destChunkSeekPos,
	 viewPortSize);
    }
  else
    {
      // We're appending to a character rules file.
      duplicatChunkCoordFound = duplicateCoordInCRDest
	(multiChunkFileName, multiChunkFile, searchCoord, destChunkSeekPos,
	 viewPortSize);
    }
}


bool duplicateCoordInBgDest
(const std::string multiChunkFileName, std::fstream & multiChunkFile,
 const yx searchCoord, std::streampos & destChunkSeekPos,
 const yx viewPortSize)
{
  backgroundChunkCharType chunk[yHeight][xWidth];

  return findBgChunk
    (multiChunkFileName, multiChunkFile, chunk, searchCoord,
     destChunkSeekPos, viewPortSize);
}


bool duplicateCoordInCRDest
(const std::string multiChunkFileName, std::fstream & multiChunkFile,
 const yx searchCoord, std::streampos & destChunkSeekPos,
 const yx viewPortSize)
{
  char chunk[yHeight][xWidth];

  return findCRChunk
    (multiChunkFileName, multiChunkFile, chunk, searchCoord, destChunkSeekPos,
     viewPortSize);
};


bool findBgChunk
(const std::string fileName, std::fstream & file,
 backgroundChunkCharType chunk[][xWidth], const yx targetCoord,
 std::streampos & chunkSeekPos, const yx viewPortSize)
{
  bool foundChunk {false};
  yx currentCoord;

  while(true)
    {
      // Store position in file before attempting to read next chunk.
      chunkSeekPos = file.tellg();      
      if(!getBgChunk(file, chunk, viewPortSize, currentCoord,
		     fileName, true))
	{
	  break;
	}
      else if(currentCoord == targetCoord)
	{
	  foundChunk = true;
	  break;
	}
    }

  return foundChunk;
}


bool findCRChunk
(const std::string fileName, std::fstream & file,
 char chunk[][xWidth], const yx targetCoord, std::streampos & chunkSeekPos,
 const yx viewPortSize)
{
  bool foundChunk {false};
  yx currentCoord;
  
  while(true)
    {
      // Store position in file before attempting to read next chunk.
      chunkSeekPos = file.tellg();
      if(!getCRChunk(file, chunk, viewPortSize, currentCoord,
		     fileName, true))
	{
	  break;
	}
      else if(currentCoord == targetCoord)
	{
	  foundChunk = true;
	  break;
	}
    }
  
  return foundChunk;
}


/* Compactant should be open (using std::ios::in | std::ios::out |
   std::ios::binary). */
void compactDestFileInPlace
(const std::string & compactantFileName, std::fstream & compactant,
 std::streampos & chunkStartSeekPos, std::streampos chunkEndSeekPos,
 const yx viewPortSize, const std::string eMsg)
{
  if(chunkEndSeekPos == -1)
    {
      /* The matching chunk was the last chunk in the file and thus tellg
	 returned -1 after the chunk was read. So all we need to do is truncate
	 the file from the start of this last chunk (chunkStartSeekPos.) */
      /* Trying to read past the end of the file can set compactant.fail to
	 true. */
      if(compactant.fail())
	{
	  compactant.clear();
	}
    }
  else
    {
      /* The matching chunk has other chunks after it. So we must move those
	 other chunks down by the size of the chunk at chunkStartSeekPos (The
	 chunk to be removed.) */
      const std::streamsize sizeOfChunkToRemove
	{chunkEndSeekPos - chunkStartSeekPos};
      std::vector<char> buffer(sizeOfChunkToRemove);
      
      while(!compactant.eof())
	{
	  /* Read data from the position after the deleted record. */
	  compactant.seekg(chunkEndSeekPos);
	  compactant.read(buffer.data(), sizeOfChunkToRemove);

	  /* If we've reached the end of the file, resize the buffer. */
	  if(compactant.gcount() < sizeOfChunkToRemove)
	    {
	      buffer.resize(compactant.gcount());
	      /* We must reset compactants error flags (which are almost
		 certainly set because we tried to read past the end of the
		 file.) to do the final write. */
	      if(compactant.fail())
		{
		  compactant.clear();
		}
	      // Seek to next position to start writing to.
	      compactant.seekp(chunkStartSeekPos);
	      compactant.write(buffer.data(), buffer.size());
	      chunkStartSeekPos += compactant.gcount();
	      break;
	    }
	  // Seek to next position to start writing to.
	  compactant.seekp(chunkStartSeekPos);
	  compactant.write(buffer.data(), buffer.size());
	  
	  // Update the start and end positions.
	  chunkStartSeekPos += compactant.gcount();
	  chunkEndSeekPos += compactant.gcount();
	}
    }
  compactant.close();
  
  // truncate the file at the new end position
  if(truncate(compactantFileName.c_str(), chunkStartSeekPos) != 0)
    {
      exit(concat("Error: failed to open file \"", compactantFileName, "\""
		  " for truncation after first step of compaction when ",
		  eMsg, "."), ERROR_OPENING_FILE);
    }
};


void extract(const std::string multiChunkFileName,
	     const std::string singleChunkFileName,
	     const std::string yCoord, const std::string xCoord,
	     const yx viewPortSize)
{
	auto yCoordIter {yCoord.begin()}, xCoordIter {xCoord.begin()};
	/* This function should have already been called for both yCoord and
	   xCoord. So the error message shouldn't ever be printed if we get to
	   this point, but we supply one none the less. */
	const yx coord
	  {readSingleNum(yCoord, yCoordIter, "trying to read 3rd argument "
			  "(chunk y position)", true),
	    readSingleNum(xCoord, xCoordIter, "trying to read 4th argument "
			  "(chunk x position)", true)};
	extract(multiChunkFileName, singleChunkFileName, coord,
		viewPortSize);
}


void extract(const std::string multiChunkFileName,
	     const std::string singleChunkFileName, const yx coord,
	     const yx viewPortSize)
{
  auto convertBgChunkTypeToChunkInfo =
    [viewPortSize](backgroundChunkCharType inputChunk[][xWidth],
		   backgroundChunkCharInfo outputChunk[][xWidth])
    {
      for(int yIter {}; yIter < viewPortSize.y; yIter++)
	    {
	      for(int xIter {}; xIter < viewPortSize.x; xIter++)
		{
		  outputChunk[yIter][xIter].ch = inputChunk[yIter][xIter];
		  outputChunk[yIter][xIter].set = true;
		}
	    }
    };
  
  /* Open files for writing / reading.
   Note that std::ios::binary is not a file opening mode in and of itself and
   so when not using ofstream or ifstream we must also supply
   ios::in and or ios::out if we are also supplying std::ios::binary
   (presumably just supplying std::ios::binary overrides the default of
   std::ios::in | std::ios::out when using fstream.) If we just use
   std::ios::binary with fstream by itself the file opening operation will
   fail. */
  std::fstream source(multiChunkFileName,
		      std::ios::in | std::ios::binary);
  // Open dest in append mode.
  std::ofstream dest(singleChunkFileName, std::ios::binary);

  if(!source)
    {
      exit(concat("Error: failed to open source file \"", multiChunkFileName,
		  "\" when attempting to extract chunk ", coord, " from it "
		  "and store said chunk in \"", singleChunkFileName,
		  "\"."), ERROR_OPENING_FILE);
    }
  else if(source.peek() == std::ifstream::traits_type::eof())
    {
      exit(concat("Error: source file \"", multiChunkFileName,
		  "\" found to be empty when attempting to extract chunk ",
		  coord, " from it and store said chunk in \"",
		  singleChunkFileName, "\"."), ERROR_OPENING_FILE);
    }
  else if(!dest)
    {
      exit(concat("Error: failed to open destination file \"",
		  singleChunkFileName, "\" when trying to write chunk ",
		  coord, " from file \"", multiChunkFileName, "\" to it."),
	   ERROR_OPENING_FILE);
    }

  
  
  yx currentCoord {};
  bool foundChunk {false};
  const std::string chunkNotFoundMsg
  {concat("Error: failed to find chunk ", coord, " in source file "
	  "\"", multiChunkFileName, "\" when trying to extract "
	  "chunk.")};
  if(checkForPostfix(multiChunkFileName.c_str(), BACKGROUND_FILE_EXTENSION))
    {
      backgroundChunkCharType chunk[yHeight][xWidth];
      std::streampos destChunkSeekPos;
      
      if(findBgChunk(multiChunkFileName, source, chunk, coord, destChunkSeekPos,
		     viewPortSize))
	{
	  // Convert chunk to suitable format.
	  backgroundChunkCharInfo outputChunk[yHeight][xWidth] {};
	  convertBgChunkTypeToChunkInfo(chunk, outputChunk);
	  // Copy chunk to dest file (singleChunkFileName).
	  compressAndWriteOutBgChunk(singleChunkFileName, dest, coord,
				     outputChunk, viewPortSize);
	}
      else
	{
	  exit(chunkNotFoundMsg, ERROR_INVALID_CMD_ARGS);
	}
    }
  else
    {
      char chunk[yHeight][xWidth];
      std::streampos destChunkSeekPos;

      if(findCRChunk(multiChunkFileName, source, chunk, coord, destChunkSeekPos,
		     viewPortSize))
	{
	  // Copy chunk to dest file (singleChunkFileName).
	  compressAndWriteOutCRChunk(singleChunkFileName, dest, coord, chunk,
				     viewPortSize);
	}
      else
	{
	  exit(chunkNotFoundMsg, ERROR_INVALID_CMD_ARGS);
	}
    }
}


void deleteChunk
(const std::string multiChunkFileName, const std::string yCoord,
 const std::string xCoord, const yx viewPortSize)
{
  std::fstream multiChunkFile
    {multiChunkFileName, std::ios::in | std::ios::out | std::ios::binary};
  if(!multiChunkFile)
    {
      exit(concat("Error: failed to open file \"", multiChunkFileName,
		  "\" when attempting to delete chunk (", yCoord, ", ",
		  xCoord, ") from file."), ERROR_OPENING_FILE);
    }
  auto yCoordIter {yCoord.begin()}, xCoordIter {xCoord.begin()};
  /* This function should have already been called for both yCoord and
     xCoord. So the error message shouldn't ever be printed if we get to
     this point, but we supply one none the less. */
  const yx searchCoord
    {readSingleNum(yCoord, yCoordIter, "trying to read 2nd argument "
		   "(chunk y position)", true),
     readSingleNum(xCoord, xCoordIter, "trying to read 3rd argument "
		   "(chunk x position)", true)};


      bool duplicatChunkCoordFound {false};
      std::streampos destChunkSeekPos;

      checkForDuplicateChunk
	(multiChunkFileName, multiChunkFile, searchCoord, duplicatChunkCoordFound,
	 destChunkSeekPos, viewPortSize);

      if(!duplicatChunkCoordFound)
	{
	  // Remove duplicate chunk.
	}
      else
	{
	  // Print error message.
	}
}
