#include "levelInteractionHandler.h++"

//test level agains sprite's
char intersection(const std::string & boundsInteraction, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any character other then space in boundsInteration
  //return that charater
  return 'n';
}

//test player sprite against sprite's
char intersection(const std::vector<int> playerSpChoords, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any value in playerSpChoords
  //return characte code or something
  return 'n';
}

//test level agains sprite's
char nearPass(const std::string & boundsInteraction, const std::vector<int> spChoords)
{
  //if any value in spChoords is next to any character other then space in boundsInteration
  //return that charater
  return 'n';
}

//test player sprite against sprite's
char nearPass(const std::vector<int> playerSpChoords, const std::vector<int> spChoords)
{
  //if any value in spChoords matches any value in playerSpChoords
  //return a character code that corispondest to the side that was matched relative to spChoords
  //for example 't' for top. (this might be used to detect that the player has "touched" the top
  //of the sprite and if it is an enimy sprite then appropriate action can be taken.
  return 'n';
}
