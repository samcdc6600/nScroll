#include "include/background.hpp"
#include "include/colorS.hpp"
#include "include/collapse.hpp"
#include <cstdlib>


void backgroundData::loadAndInitialiseBackground()
{
  std::string rawLevelBackground {};

  //checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION)
  if(!checkForPostfix(&fileName[0], BACKGROUND_FILE_EXTENSION))
    {
      exit(concat
	   ("Error: opening file \"", fileName, "\" as a background "
	    "file. Postfix for ", BACKGROUND_FILE_EXTENSION, " files should "
	    "be \"", BACKGROUND_FILE_EXTENSION, "\"."), ERROR_OPENING_FILE);
    }
  // loadFileIntoString
  //   (fileName, rawLevelBackground,
  //    concat("trying to read ", BACKGROUND_FILE_EXTENSION, " file"));

    std::fstream bgFile(fileName, std::ios::in | std::ios::binary);

  if(!bgFile)
    {
      exit(concat("Error: failed to open background file \"", fileName, "\"."),
	   ERROR_OPENING_FILE);
    }
  else if(bgFile.peek() == std::ifstream::traits_type::eof())
    {
      exit(concat("Error: background file \"", fileName, "\" found to be "
		  "empty. \"", BACKGROUND_FILE_EXTENSION, "\" files must not "
		  "be empty."), ERROR_BACKGROUND);
    }
  
  initialiseBackground(bgFile, chunkMap);
  bgFile.close();
}


void backgroundData::initialiseBackground
(std::fstream & bgFile, chunkMapType & background)
{
  yx mapCoord {};
  bgChunk mapChunk;
  int mapChunksRead {};
	
      while(getBgChunk(bgFile, mapChunk, chunkSize, mapCoord,
		       fileName, true))
	{
	  mapChunksRead++;
	  /* We don't want to re-write getBgChunk or insertChunk so we have to
	     do this conversion here. */
	  std::vector<chunkElementBaseType> linearMapChunk;
	  for(int yIter {}; yIter < chunkSize.y; yIter++)
	    {
	      for(int xIter {}; xIter < chunkSize.y; xIter++)
		{
		  linearMapChunk.push_back(mapChunk[yIter][xIter]);
		}
	    }
	  insertChunk(mapCoord, linearMapChunk, mapChunksRead, fileName,
		      background);
	  linearMapChunk.clear();
	}
      if(mapChunksRead == 0)
	{
	  exit(concat
	       ("Error: unable to read any chunks from background file \"",
		fileName, "\" when attempting to initialise backgroundData "
		"object (note that the file isn't empty!) "),
	       ERROR_MALFORMED_FILE);
	}
}


// Note that multiChunkFile has a default value of false.
bool backgroundData::getBgChunk
(std::fstream & file, bgChunk chunk, const yx chunkSize, yx & chunkCoord,
 const std::string & fileName, const bool multiChunkFile)
{
  const std::string eMsgStart
    {concat("Error: trying to read in background file \"", fileName,
	    "\". ")};  
  
  if(!readInChunkCoordFromFile(chunkCoord, fileName, file, !multiChunkFile))
    {
      return false;
    }
  
  // Read in chunk body.
  int yIter {}, xIter {};
  chunkElementBaseType bgChar {};
  int chunkCharsFound {};
  while(file.read(reinterpret_cast<char*>(&bgChar), sizeof(int)))
    {      
      if(bgChar == bgRunLengthSequenceSignifier)
	{
	  int runLength {};
	  if(!file.read(reinterpret_cast<char*>(&runLength), sizeof(int)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier (or an IO error has occurred.)"),
		   ERROR_MALFORMED_FILE);
	    }
	  if(!file.read(reinterpret_cast<char*>(&bgChar), sizeof(int)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier and length (or an IO error has occurred.)"
			  ), ERROR_MALFORMED_FILE);
	    }
	  for(int iter {}; iter < runLength; iter++)
	    {
	      if(!checkYOfVirtualNestedChunkLoop
		 (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
		{
		  /* This is a multi chunk file and we've just read past the end
		     of a chunk so we need to back up. */
		  file.seekg(-sizeof(int), std::ios::cur);
		  goto MEGA_BREAK;
		}
	      chunk[yIter][xIter] = bgChar;
	      chunkCharsFound++;
	      updateVirtualNestedChunkYXLoop(xIter, yIter, chunkSize);
	    }
	}
      else
	{
	  if(!checkYOfVirtualNestedChunkLoop
	     (xIter, yIter, chunkSize, multiChunkFile, eMsgStart))
	    {
	      /* This is a multi chunk file and we've just read past the end
		 of a chunk so we need to back up. */
	      file.seekg(-sizeof(int), std::ios::cur);
	      goto MEGA_BREAK;
	    }
	  chunk[yIter][xIter] = bgChar;
	  chunkCharsFound++;
	  updateVirtualNestedChunkYXLoop(xIter, yIter, chunkSize);
	}
    }

 MEGA_BREAK:  
  if(chunkCharsFound < chunkSize.y * chunkSize.x)
    {
      if(multiChunkFile)
	{
	  return false;
	}
      else
	{
	  exit(concat(eMsgStart, "Chunk is too small (", chunkCharsFound, ") "
		      "It should be ", chunkSize.y * chunkSize.x, "."),
	       ERROR_MALFORMED_FILE);
	}
    }

  return true;
}
