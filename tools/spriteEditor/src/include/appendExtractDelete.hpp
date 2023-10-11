#ifndef APPENDMODE_HPP__
#define APPENDMODE_HPP__


#include <string>
#include "utils.hpp"


/* Appends singleChunkFileName to multiChunkFileName. SingleChunkFileName and
 multiChunkFileName should already have their file extensions validated to
 ensure that they are the right type of files and in the right order and
 combination. */
void append(const std::string singleChunkFileName,
            const std::string multiChunkFileName, const yx viewPortSize);
/* Calls:
   extract(const std::string multiChunkFileName,
           const std::string singleChunkFileName, const yx coord,
           const yx viewPortSize).
   See appendExtract.cpp. */
void extract(const std::string multiChunkFileName,
	     const std::string singleChunkFileName,
	     const std::string yCoord, const std::string xCoord,
	     const yx viewPortSize);
/* Deletes chunk with coordinate (yCoord, xCoord) from multichunkfilename (if
   it exists.) Exits with error otherwise. If chunk (yCoord, xCoord) is not the
   last chunk the file is compacted. */
void deleteChunk
(const std::string multiChunkFileName, const std::string yCoord,
 const std::string xCoord, const yx viewPortSize);


#endif
