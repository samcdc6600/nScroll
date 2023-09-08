#include <fstream>
#include "include/appendExtract.hpp"


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
