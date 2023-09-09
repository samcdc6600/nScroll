#include <fstream>
#include "include/appendExtract.hpp"


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
    std::ifstream source(singleChunkFileName, std::ios::binary);
    // Open dest in append mode.
    std::ofstream dest(multiChunkFileName, std::ios::binary | std::ios::app);

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

    // Append source to dest.
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
    source.close();
    dest.close();
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
  // Open files for writing / reading.
  std::ifstream source(multiChunkFileName, std::ios::binary);
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

  
  // Progressively check coordinates decompress chunks.
  yx currentCoord {};
  bool foundChunk {false};
  if(checkForPostfix(multiChunkFileName.c_str(), BACKGROUND_FILE_EXTENSION))
    {
      backgroundChunkCharType chunk[yHeight][xWidth];
      
      while(true)
	{
	  if(!getBgChunk(source, chunk, viewPortSize, currentCoord,
			 singleChunkFileName, true))
	    {
	      break;
	    }
	  else if(currentCoord == coord)
	    {
	      foundChunk = true;
	      break;
	    }
	}

        if(foundChunk)
	{
	  // Copy chunk to new file.
	  endwin();
	  std::cout<<"Found chunk "<<coord<<'\n';
	  exit(-1);
	}
      else
	{
	  // Exit with error.
	  exit(concat("Error: failed to fined chunk ", coord, " in source file "
		      "\"", multiChunkFileName, "\" when trying to extract "
		      "chunk."), ERROR_INVALID_CMD_ARGS);
	}
    }
  else
    {
      // char chunk[yHeight][xWidth];
      // while(getCRChunk(source, ))
      // 	{
      // 	}
    }

  // Write chunk found to singleChunkFileName file.
}
