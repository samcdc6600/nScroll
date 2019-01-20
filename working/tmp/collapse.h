#ifndef COLLAPS_H_
#define COLLAPS_H_
#include <string>
#include <vector>
#include <map>

class chMap
{
 private:
  std::map<std::string, int> strMap;//stores special character types to int code mappings
  std::vector<std::string> characters//special characters types
    { "ACS_ULCORNER", "ACS_LLCORNER", "ACS_LRCORNER", "ACS_LTEE", "ACS_RTEE", "ACS_BTEE", "ACS_TTEE",
	"ACS_HLINE", "ACS_VLINE", "ACS_PLUS", "ACS_S1", "ACS_S3", "ACS_S7", "ACS_S9", "ACS_DIAMOND",
	"ACS_CKBOARD", "ACS_DEGREE", "ACS_PLMINUS", "ACS_BULLET", "ACS_LARROW", "ACS_RARROW",
	"ACS_DARROW", "ACS_UARROW", "ACS_BOARD", "ACS_LANTERN", "ACS_BLOCK", "ACS_LEQUAL",
	"ACS_GEQUAL", "ACS_PI", "ACS_NEQUAL", "ACS_STERLING" };
 public:
 chMap();
 int find(const std::string str);
};
//"Collapse's" buff and stores into ret. copies all characters not part of an escape sequence into
//ret (one char goes into one int (thus ret may actually be larger then buff (but I say it is collapsed
//for reasons explained shortly)). All escape sequences are not copied reather they are used as an
//index into a chMap object and the return value (an int that should be unique) is stored into one value
//of ret instead of all the characters that make up the escape sequence.
//nonColor escape sequences start with /nc
void collapse(const std::string & buffWithSpaces, std::vector<int> & ret);

#endif
