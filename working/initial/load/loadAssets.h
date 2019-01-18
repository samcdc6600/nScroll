#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_

#include <string>
#include <fstream>

//copies contents of levelName file into level pointer
bool loadASCIIFile(const std::string & name, std::string & buff);

#endif
