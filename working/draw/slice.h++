#ifndef SLICE_H_
#define SLICE_H_
#include <vector>

//returns vector<int> or size y x at offset from buff. Note buff must be y hight and at least x long
//(offset + x cannot go past end of (array / y) and offset should not be negative (however at the
//current time offset is an unsigned long so there is no required check). This should be checked for
//in the physics code and should cause an exception in this code.)
std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x);

#endif
