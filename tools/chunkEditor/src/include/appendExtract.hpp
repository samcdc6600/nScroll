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

#endif
