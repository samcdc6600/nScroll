#include <cstdlib>
#include <fstream>
#include "include/background.hpp"
#include "include/colorS.hpp"
#include "include/collapse.hpp"


void backgroundData::loadAndInitialiseBackground()
{
  if(!checkForPostfix(&fileName[0], BACKGROUND_FILE_EXTENSION))
    {
      exit(concat
	   ("Error: opening file \"", fileName, "\" as a background "
	    "file. Postfix for ", BACKGROUND_FILE_EXTENSION, " files should "
	    "be \"", BACKGROUND_FILE_EXTENSION, "\"."), ERROR_OPENING_FILE);
    }

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
  std::vector<chunkFileElementBaseType> mapChunk {};
  /* Chunk elements are stored on disk using a smaller number. */
  std::vector<chunkElementBaseType> convertedMapChunk {};
  int mapChunksRead {};

  auto convertMapChunk = [& mapChunk, & convertedMapChunk]()
  {
    /* Note here that we are using 32 less 1 because this prevents space from
       being 0 which causes a problem in getColor in the line:
       int color {((ch -1) / maxCharNum) +1};
       We can't be bothered to try to fix it since it works otherwise and 32 is
       enough to remove fix the characters in ints. */
    constexpr int lowerUnusedASCIIChNum {32 -1};
    constexpr int maxCharNum {MONO_CH_MAX - lowerUnusedASCIIChNum};
    constexpr int aSCIIChMaxEffective
      {ASCII_CH_MAX - lowerUnusedASCIIChNum};

    auto getColor = [](const int ch)
    {
      /* A character is encoded as it's basic value + it's color value -1 times
	 maxCharNum. Since integer division rounds down (ch -1) / maxCharNum
	 should give the color code. less 1. */
      int color {((ch -1) / maxCharNum)};
      if(color > colorParams::effectiveGameColorPairsNo)
	{
	  exit(concat("Error (in getColor()): encountered colour (", color, ") "
		      "code that is out of range.\n"),
	       ERROR_COLOR_CODE_RANGE);
	}
      return color;
    };
    
    for(chunkFileElementBaseType ch: mapChunk)
      {
	int color {getColor(ch)};
	// Remove colour information (first color starts at 1, so we remove 1.)
	int rawCh {ch - ((getColor(ch) -1) * maxCharNum)};
	/* To avoid having to store character color pair combinations using
	   ints (as opposed to unsigned shorts) we remove lowerUnusedASCIIChNum
	   from each raw character (one that has no color pair info). So we must
	   add it back here! */
	rawCh += lowerUnusedASCIIChNum;
	/* A character is encoded as it's basic value + it's color value -1
	   times maxCharNum. Since integer division rounds down (ch -1) /
	   maxCharNum should give the color code. less 1. */
	rawCh += ((color -1) * MONO_CH_MAX);
	convertedMapChunk.push_back(rawCh + lowerUnusedASCIIChNum);
      }
  };
	
  while(getBgChunk(bgFile, mapChunk, chunkSize, mapCoord,
		   fileName, true))
    {
      mapChunksRead++;
      convertMapChunk();
      insertChunk(mapCoord, convertedMapChunk, mapChunksRead, fileName,
		  background);
      mapChunk.clear();
      convertedMapChunk.clear();
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
(std::fstream & file, std::vector<chunkFileElementBaseType> & chunk,
 const yx chunkSize, yx & chunkCoord, const std::string & fileName,
 const bool multiChunkFile)
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
  chunkFileElementBaseType bgChar {};
  int chunkCharsFound {};
  while(file.read(reinterpret_cast<char*>(&bgChar),
		  sizeof(chunkFileElementBaseType)))
    {      
      if(bgChar == bgRunLengthSequenceSignifier)
	{
	  chunkFileElementBaseType runLength {};
	  if(!file.read(reinterpret_cast<char*>(&runLength),
			sizeof(chunkFileElementBaseType)))
	    {
	      exit(concat(eMsgStart, "File ends after run-length sequence "
			  "signifier (or an IO error has occurred.)"),
		   ERROR_MALFORMED_FILE);
	    }
	  if(!file.read(reinterpret_cast<char*>(&bgChar),
			sizeof(chunkFileElementBaseType)))
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
		  file.seekg(-sizeof(chunkFileElementBaseType), std::ios::cur);
		  goto MEGA_BREAK;
		}
	      chunk.push_back(bgChar);
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
	      file.seekg(-sizeof(chunkFileElementBaseType), std::ios::cur);
	      goto MEGA_BREAK;
	    }
	  chunk.push_back(bgChar);
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
