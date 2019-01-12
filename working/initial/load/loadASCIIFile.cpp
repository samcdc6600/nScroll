#include "loadASCIIFile.h"
#include <iostream>

bool loadASCIIFile(const std::string & name, std::string & buff)
{
  std::ifstream file;
  file.open(name.c_str());
  if(!file.is_open())
    {
      file.close();
      return false;//file could not be opened
    }
  char c;
  while(file.get(c))
    buff.push_back(c);//get file contents

  file.close();
  return true;
}
