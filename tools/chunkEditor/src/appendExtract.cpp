#include <fstream>
#include "include/appendExtract.hpp"


/* Searches for a background chunk with coord as it's coordinate in file. If the
   coordinate is found in file then the corresponding chunk is stored in
   chunk, chunkSeekPos is set to the start of the chunk (start of coordinates)
   and the function returns true. Otherwise the function returns false.
   Note that file should already be open. And this function won't close it. */
bool findBgChunk
(const std::string fileName, std::fstream & file,
 backgroundChunkCharType chunk[][xWidth], const yx targetCoord,
 std::streampos & chunkSeekPos, const yx viewPortSize);
bool findCRChunk
(const std::string fileName, std::fstream & file,
 char chunk[][xWidth], const yx targetCoord, std::streampos & chunkSeekPos,
 const yx viewPortSize);
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
  auto duplicateCoordInBgDest =
    [& singleChunkFileName, & multiChunkFileName, viewPortSize]
    (std::fstream & source, std::fstream & dest,
     std::streampos & destChunkSeekPos)
    {
      backgroundChunkCharType chunk[yHeight][xWidth];
      yx sourceChunkCoord;
      
      readInChunkCoordFromFile
	(sourceChunkCoord, singleChunkFileName, source, true);
      
      // Seek back to the start of the source file.
      source.seekg(0, std::ios::beg);
      return findBgChunk
	(multiChunkFileName, dest, chunk, sourceChunkCoord, destChunkSeekPos,
	 viewPortSize); 
    };

  auto duplicateCoordInCRDest =
    [& singleChunkFileName, & multiChunkFileName, viewPortSize]
    (std::fstream & source, std::fstream & dest,
     std::streampos & destChunkSeekPos)
    {
      char chunk[yHeight][xWidth];
      yx sourceChunkCoord;

      readInChunkCoordFromFile
	(sourceChunkCoord, singleChunkFileName, source, true);

      // Seek back to the start of the source file.
      source.seekg(0, std::ios::beg);
      return findCRChunk
	(multiChunkFileName, dest, chunk, sourceChunkCoord, destChunkSeekPos,
	 viewPortSize);
    };

  /* Compactant should be open (using std::ios::in | std::ios::out |
     std::ios::binary). */
  auto compactDestFileInPlace =
    [& singleChunkFileName, & multiChunkFileName, viewPortSize]
    (std::fstream & compactant, std::streampos & chunkStartSeekPos,
     std::streampos chunkEndSeekPos)
    {
      const std::streamsize sizeOfChunkToRemove
	{chunkEndSeekPos - chunkStartSeekPos};
      std::vector<char> buffer(sizeOfChunkToRemove);

      while (!compactant.eof())
	{
	  /* Read a chunk of data from the position after the deleted record. */
	  compactant.seekg(chunkEndSeekPos);
	  compactant.read(buffer.data(), sizeOfChunkToRemove);

	  /* If we've reached the end of the file, resize the buffer. */
	  if (compactant.gcount() < sizeOfChunkToRemove)
	    {
	      buffer.resize(compactant.gcount());
	    }

	  /* Write the chunk of data at the start position of the deleted
	     record */
	  compactant.seekp(chunkStartSeekPos);
	  compactant.write(buffer.data(), buffer.size());

	  // Update the start and end positions.
	  chunkStartSeekPos += compactant.gcount();
	  chunkEndSeekPos += compactant.gcount();
	}

      // truncate the file at the new end position
      compactant.close();
      compactant.open(multiChunkFileName, std::ios::in | std::ios::out |
		std::ios::trunc);
      if(!compactant)
	{
	  exit(concat("Error: failed to open destination file \"",
		      multiChunkFileName, "\" (for truncation after "
		      "compaction) when trying to append file \"",
		      singleChunkFileName, "\" to it."), ERROR_OPENING_FILE);
	}
      compactant.seekp(chunkStartSeekPos);
      compactant.close();
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

    /* We need to decompress chunks to find out if there is a chunk with a
       duplicate coordinate. This is done slightly differently for each type of
       file. */
    if(checkForPostfix(multiChunkFileName.c_str(), BACKGROUND_FILE_EXTENSION))
      {
	// We're appending to a background file.
	duplicatChunkCoordFound = duplicateCoordInBgDest(source, dest,
							 destChunkSeekPos);
      }
    else
      {
	// We're appending to a character rules file.
	duplicatChunkCoordFound = duplicateCoordInCRDest(source, dest,
							 destChunkSeekPos);
      }

    if(duplicatChunkCoordFound)
      {
	/* We've found a duplicate chunk (ask the user if they wish to
	   overwrite it. */
	int input {};
	addDuplicateChunk = getConfirmation
	  (viewPortSize, helpColorPair, input, subMenuSleepTimeMs,
	   concat
	   ("\t", yx{}, " already exists in the file \"", multiChunkFileName,
	    "\". Appending the chunk from file \"", singleChunkFileName, "\" "
	    "will override the existing chunk with the same coordinate in \"",
	    multiChunkFileName, "\". Do you still want to append the chunk "
	    "from \"", singleChunkFileName, "\", y/n?"));
	if(addDuplicateChunk)
	  {
	    // CompactBgDestFileInPlace() closes dest.
	    compactDestFileInPlace(dest, destChunkSeekPos, dest.tellg());
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

  endwin();
  std::cout<<"target Chunk = "<<targetCoord<<'\n';
  
  while(true)
    {
      // Store position in file before attempting to read next chunk.
      chunkSeekPos = file.tellg();
      if(!getCRChunk(file, chunk, viewPortSize, currentCoord,
		     fileName, true))
	{
	  std::cout<<"didn't find chunk\n";
	  break;
	}
      else if(currentCoord == targetCoord)
	{
	  foundChunk = true;
	  break;
	}

      std::cout<<"found chunk "<<currentCoord<<'\n';
    }

  exit(-1);

  return foundChunk;
}


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
