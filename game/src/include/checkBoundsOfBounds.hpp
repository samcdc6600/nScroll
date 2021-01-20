/* should check that the bounds file that has been read in is the same length as
the background file after it has been collapsed into int's. The length should be
a comparison of number of char's to number of int's */
#ifndef CHECKBOUNDSOFBOUNDS_H_
#define CHECKBOUNDSOFBOUNDS_H_
#include <string>
#include <vector>

/* should return an integer that represents the difference in "size" between the
   background file and the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background); 

#endif
