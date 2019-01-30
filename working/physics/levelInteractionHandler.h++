#ifndef LEVELINTERACTIONHANDLER_H_
#define LEVELINTERACTIONHANDLER_H_
#include <string>
#include <vector>

char intersection(const std::string & boundsInteraction, const std::vector<int> spChoords);//check level agains sprite
char intersection(const std::vector<int> playerSpChoords, const std::vector<int> spChoords);//check player against sprite
char nearPass(const std::string & boundsInteraction, const std::vector<int> spChoords);//check level against sprite
char nearPass(const std::vector<int> playerSpChoords, const std::vector<int> spChoords);//check player against sprite

#endif
