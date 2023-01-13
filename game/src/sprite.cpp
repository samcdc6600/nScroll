#include <stdexcept>
#include <curses.h>
#include <iostream>
#include "include/sprite.hpp"
#include "include/collapse.hpp"


sprite::sprite(std::vector<std::string> & spritePaths, const yx max,
	       const size_t bgSize, const yx pos, const directions dir)
  : maxyx(max), position(pos), direction(checkDirection(dir)),
    currentSliceNumber(0),
    startTime(std::chrono::high_resolution_clock::now()),
    currentTime(std::chrono::high_resolution_clock::now())
{
  for(auto spriteFileName {spritePaths.begin()};
      spriteFileName != spritePaths.end(); spriteFileName++)
    {
      spriteSliceSets.push_back(new spriteData ());
      loadSprite(spriteFileName->c_str(), *spriteSliceSets.back());
    }
  initialiseDirectionsVector();
  getMaxYXOffset();
  checkInitialPosIsInRangeOfLevel
    (spritePaths, maxBottomRightOffset, bgSize, pos);
}


sprite::~sprite()
{
  for(auto sliceSet {spriteSliceSets.begin()};
      sliceSet != spriteSliceSets.end(); sliceSet++)
    { // The elements of spriteSliceSets were dynamically allocated!
      delete *sliceSet;
    }
};


std::vector<std::vector<sprite::partiallyProcessedSliceLine>>
sprite::parserPhaseOne(const std::string & spriteFile, spriteData & sD)
{
  if(spriteFile.size() < 8)
    {				// Sprite file too short. Malformed.
      std::stringstream e;
      e<<"in sprite/sprite.cpp "
	"sdt::vector<std::vector<partiallyProcessedSliceLine>> "
	"sprite::parserPhaseOne(const std::string & spriteFile) Error "
	"spriteFile.size() < 8. spriteFile name = ";
      throw std::logic_error(e.str());
    }
  /* stores what position we are upto in the parse. start's off as 3 becaue it
     is used after the initial character sequence. */
  int iter{3}; 

  // Get cs value --------------------------------------------------------------
  if(spriteFile[0] == 'c' && spriteFile[1] == 's' && spriteFile[2] == '(')
    { // Will hold ASCII representation of cycleSpeed0 integer veriable.
      std::stringstream cS {};
      // Have we obtained and converted the cycle speed value yet?
      bool runGetCs {true}; 
      for( ; runGetCs; ++iter)
	{
	  if(size_t(iter) >= spriteFile.size())
	    {			// Malformed cs value.
	      std::stringstream errorMsg;
	      errorMsg<<"in sprite/sprite.cpp "
		"sdt::vector<std::vector<partiallyProcessedSliceLine>>"
		"sprite::parserPhaseOne(const std::string & spriteFile)"
		" Error malformed cs value. spriteFile name = ";
	      throw std::logic_error(errorMsg.str());
	    }
	  else
	    if(!std::isdigit(spriteFile[iter]) && spriteFile[iter] != ')')
	      {			// Malformed cs value
		std::stringstream errorMsg;
		errorMsg<<"in sprite/sprite.cpp "
		  " sdt::vector<std::vector<partiallyProcessedSliceLine>> "
		  "sprite::parserPhaseOne(const std::string & spriteFile)"
		  " Error malformed cs value (illegal character). spriteFile "
		  "name = ";
		throw std::logic_error(errorMsg.str());
	      }
	    else
	      {
		if(spriteFile[iter] == ')')
		  {
		    if(cS.rdbuf()->in_avail() == 0)
		      {		// If cS is empty
			std::stringstream errorMsg;
			errorMsg<<"in sprite/sprite.cpp "
			  " sdt::vector<std::vector"
			  "<partiallyProcessedSliceLine>>  sprite::"
			  "parserPhaseOne(const std::string & spriteFile)"
			  " Error malformed cs value (missing value). "
			  "spriteFile name = ";
			throw std::logic_error(errorMsg.str());
		      }
		    /* Set sD.sliceLine to ASCII representation of cycle speed
		       from sprite file. */
		    cS>>sD.cycleSpeed; 
		    runGetCs = false;
		    iter++; // Move to next positon for next phase of parsing.
		    continue;
		  }
		// Get next part of ASCII representation of cycle speed.
		cS<<spriteFile[iter];
	      }	      
	}
    }
  else
    {				// Begining of sprite file malformed.
      std::stringstream errorMsg;
      errorMsg<<"in sprite/sprite.cpp "
	" sdt::vector<std::vector<partiallyProcessedSliceLine>> "
	"sprite::parserPhaseOne(const std::string & spriteFile) Error "
	"spriteFile does not start with the sequence \"cs(\". spriteFile "
	"name = ";
      throw std::logic_error(errorMsg.str());
    } // End of get cs value ---------------------------------------------------
  // Iter should now point to the second line.
  return getSliceLines(spriteFile, iter);  // Read sprite slices.
}


std::vector<std::vector<sprite::partiallyProcessedSliceLine>>
sprite::getSliceLines(const std::string & spriteFile, int & iter)
{
  std::vector<std::vector<partiallyProcessedSliceLine>> spriteSlices{};
  bool readSlices {true};
  // Get partially processed slices.
  for(int sliceIter {}; readSlices; ++sliceIter)
    {				// Get partially processed slice lines.
      while(spriteFile[iter] != 's' && spriteFile[iter + 1] != 'e')
	{
      	  if(spriteFile[iter] == '\n')
	    { // Make sure we do not accidently add the new line character.
	      ++iter;
	    }
	  // +1 because sliceIter start's at 0.
	  spriteSlices.resize(sliceIter + 1);
	  /* Save partially processed slice line to slice
	     spriteSlices[sliceIter]. */
	  spriteSlices[sliceIter].push_back(getSliceLine(spriteFile, iter));
	}
      if(spriteFile[iter + 1] == 'e' && spriteFile[iter + 2] == 'e')
	{		      // We have reached the end of spriteFile.
	  readSlices = false;
	  continue;
	}
      else
	for( ;spriteFile[iter] != '\n'; ++iter)
	  {}
    }
  return spriteSlices;
}


sprite::partiallyProcessedSliceLine sprite::getSliceLine
(const std::string & spriteFile, int & spFIter)
{			    // Partially processed slice to be returned.
  partiallyProcessedSliceLine ppsl {};

  for( ; spriteFile[spFIter] == ' '; ++spFIter)	// Get left space cound.
    ppsl.offset++;		// Update left space count.
  if(spriteFile[spFIter] == '\n')
    { /* There is nothing but spaces on this line (left spcae ppsl.offset not
	 currently supported on line with just spaces). */
      for(int iter {}; iter < ppsl.offset; ++iter)
	ppsl.sliceLine.push_back(' ');
      spFIter++;	   // Update spFIter to point to the next line.
      //      ppsl.sliceLine.push_back('\0');
      return ppsl;		// We have gotton all the info.
    }
  else
    {
      for( ; spriteFile[spFIter] != '\n'; ++spFIter)
	{ // Get remaining side of sprite slice line.
	  ppsl.sliceLine.push_back(spriteFile[spFIter]);
	}
      //      ppsl.sliceLine.push_back('\0');
      spFIter++;	    // Update spFIter to pint to the next line.
      return ppsl;
    }

  std::stringstream errorMsg;
  errorMsg<<"in sprite/sprite.cpp at end of void getSliceLine(const std::string"
    "& spriteFile, int & spFIter, int & ppsl.offset, std::string & "
    "ppsl.sliceLine). Error this point should never be reached.";
  throw std::logic_error(errorMsg.str());
}


/* Calls collaps on slice lines and stores the lines in a type corresponding to
   the return value. */
void sprite::parserPhaseTwo
(const std::vector<std::vector<sprite::partiallyProcessedSliceLine>> & pPSL,
 spriteData & sD)
{ // Make outer vector (number of slices) the right size.
  sD.spriteSlices.resize(pPSL.size());

  // Iterte through slices.
  for(size_t sliceIter{}; sliceIter < pPSL.size(); ++sliceIter)
    {				// Iterate throught slice lines.
      for(size_t sliceLineIter{}; sliceLineIter < pPSL[sliceIter].size();
	  ++sliceLineIter)
	{ // Make slice at sliceIter the right size (number of slice lines).
	  sD.spriteSlices[sliceIter].slice.resize(pPSL[sliceIter].size());
	  // Collapse and copy slice line.
	  collapse(pPSL[sliceIter][sliceLineIter].sliceLine,
		   sD.spriteSlices[sliceIter].slice[sliceLineIter].sliceLine);
	  // Copy offset value over.
	  sD.spriteSlices[sliceIter].slice[sliceLineIter].offset =
	    pPSL[sliceIter][sliceLineIter].offset;
	}
    }
  refresh();
}


void sprite::getMaxYXOffset()
{ /* I tried my best to make maxBottomRightOffset const, however I was thwarted
     by a seg fault that would occur when calling resize() on a vector in
     getSprite() when getSprite() was called in the constructors member
     initializer list >:'( */
  size_t max {};

  for(auto spriteSlices: spriteSliceSets)
    {
      for(sliceData s: (*spriteSlices).spriteSlices) // Get y offset :).
	{
	  max = s.slice.size() > max ? s.slice.size() : s.slice.size();
	}
    }

  // The upper left of the sprite is (0,0) so we need size -1 :).
  maxBottomRightOffset.y = max -1;
  max = 0;
    for(auto spriteSlices: spriteSliceSets)
      {
	for(sliceData s: (*spriteSlices).spriteSlices)
	  {
	    for(sliceLine sl: s.slice)
	      {
		max = (sl.sliceLine.size() + sl.offset) > max ?
		  (sl.sliceLine.size() + sl.offset) : max;
	      }
	  }
    }
  maxBottomRightOffset.x = max -1;
}


void sprite::checkInitialPosIsInRangeOfLevel
(std::vector<std::string> & spritePaths, const yx maxBottomRightOffset,
 const size_t bgXSize, const yx pos)
{
  const size_t bgLen {bgXSize / maxyx.y};
  if((pos.x + maxBottomRightOffset.x) < 0 || pos.x > (long)bgLen -1 ||
     (pos.y + maxBottomRightOffset.y) < 0 || pos.y > maxyx.y -1)
    {
      std::stringstream e {};
      e<<"Error: initial position for non player sprite with file/s (";
      for(std::string name: spritePaths)
	{
	  e<<"\""<<name<<"\", ";
	}
      e<<") is out of range. ("<<pos.y<<','<<pos.x<<") given for position, but"
	" sprite has maximum size ("<<maxBottomRightOffset.y + 1<<','
       <<maxBottomRightOffset.x + 1<<") and background has size ("<<maxyx.y<<','
       <<bgLen<<"). Remember coords start at 0 and are in the form (y,x).\n";
      exit(e.str().c_str(), ERROR_SPRITE_POS_RANGE);
    }
}


void sprite::loadSprite(const char spriteFileName [], spriteData & sD)
{
  // Load sprite asset. --------------------------------------------------------
  std::string rawSprite {};// Holds raw contents of sprite file.
  if(!loadFileIntoString(spriteFileName, rawSprite))
    {
      std::stringstream e {};
      e<<"Error opening sprite file \""<<spriteFileName<<"\" :'( .";
      exit(e.str().c_str(), ERROR_OPENING_FILE);
    }
  // Call parserPhaseOne. ------------------------------------------------------
  std::vector<std::vector<partiallyProcessedSliceLine>> pPSpriteSliceS
    {parserPhaseOne(rawSprite, sD)};
  // Call parserPhaseTwo. ------------------------------------------------------
  /* Call collapse on each slice as a whole and then store slices back into
     container. */
  parserPhaseTwo(pPSpriteSliceS, sD);
  // Append sD to spriteS vector.
  spriteS.push_back(sD);
  /* Set currentSliceNumber to 0. This variable should only take on values
     between 0 and (spriteSlices.size() -1) */
  currentSliceNumber = 0;
}


void sprite::initialiseDirectionsVector()
{
  switch(spriteS.size())
    {
    case spriteNums1:
      spriteAnimationDirections = std::vector<directions>
	{DIR_NONE, DIR_NONE, DIR_NONE, DIR_NONE, DIR_NONE};
      break;
    case spriteNums2:
      spriteAnimationDirections = std::vector<directions> {DIR_NONE,
	DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT};
      break;
    default:
      std::stringstream e {};
      e<<"Error (in initialiseDirectionsVector() in sprite.cpp): SpriteS::size "
	"("<<spriteS.size()<<") not equal to "<<spriteNums1<<" or "<<spriteNums2
       <<".\n";
	
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


sprite::directions sprite::checkDirection(const directions dir)
{
  if(!(dir == DIR_NONE || dir == DIR_UP || dir == DIR_RIGHT ||
       dir == DIR_DOWN || dir == DIR_LEFT))
    {
      std::stringstream e;
      e<<"Error (in sprite.cpp): in checkDirection(), dir ("<<dir<<") !="
	" direction in the enum sprite::directions.";
      exit(e.str(), ERROR_INVALID_DIRECTION);
    }
  return dir;
}


yx sprite::getNewPos(const directions dir)
{
  yx d {};
  switch(dir)
    {
    case DIR_NONE:
      d.y = position.y;
      d.x = position.x;
      break;
    case DIR_UP:
      d.y = position.y -1;
      d.x = position.x;
      break;
    case DIR_RIGHT:
      d.y = position.y;
      d.x = position.x +1;
      break;
    case DIR_DOWN:
      d.y = position.y +1;
      d.x = position.x;
      break;
    case DIR_LEFT:
      d.y = position.y;
      d.x = position.x -1;
      break;
    default:
      std::stringstream e {};
      e<<"Error direction ("<<dir<<") not valid.";
      exit(e.str().c_str(), ERROR_INVALID_DIRECTION);
    }

  return d;
}


bool sprite::checkBoundValue(const int bound)
{
  if(!(bound >= 0))
    {
      return true;
    }
  return false;
}


bool sprite::notInWindowInnerMarginY(const int y, const int yMargin)
{
  checkBoundValue(yMargin);
  const int maximumY {y + maxBottomRightOffset.y};
  // >= and < because we start at 0.
  return (y >= yMargin) && maximumY < (maxyx.y - yMargin);
}


bool sprite::notInWindowInnerMarginX(const int x, const int xMargin)
{
    checkBoundValue(xMargin);
    const int maximumX {x + maxBottomRightOffset.x};
    // >= and < because we start at 0.
    return (x >= xMargin && maximumX < (maxyx.x - xMargin));
}


// bool sprite::inWindowY(const int y)
// {
//   const int maximumY {y + maxBottomRightOffset.y};
//   // >= and < because we start at 0.
//   return (y >= 0) && (maximumY < maxyx.y);
// }


// bool sprite::inWindowX(const int x)
// {
//   const int maximumX {x + maxBottomRightOffset.x};
//   // >= and < because we start at 0.
//   return (x >= 0) && (maximumX < maxyx.x);
// }


bool sprite::inLevelY(const int y, const int bgYLen)
{
  const int maximumY {y + maxBottomRightOffset.y};
  return (y >= 0) && (maximumY < bgYLen);
}


bool sprite::inLevelX(const int x, const int bgXLen)
{
  const int maximumX {x + maxBottomRightOffset.x};
  return (x >= 0) && (maximumX < bgXLen);
}


bool sprite::leftOfWindowInnerRightMargin(const int x, const int xBound,
					  const yx maxyx)
{ // Return true if we are to the left of the windows right inner margin.
  checkBoundValue(xBound);
  return ((x + maxBottomRightOffset.x) < (maxyx.x - xBound));
}


bool sprite::rightOfWindowInnerLeftMargin(const int x, const int xBound)
{ // Return true if we are to the right of the windows left inner margin.
    checkBoundValue(xBound);
    return (xBound < x);
}


// bool sprite::inWindow(const int y, const int x)
// {
//   return inWindowInnerMargin(y, x, 0, 0); // We have an inner boarder of 0, 0
// }


/* Returns the of position of the sprite after moving one character (including
   diagonal movement) in the direction dir */
yx sprite::peekAtPos(const directions dir) { return getNewPos(dir); }


yx sprite::getMaxBottomRightOffset() const
{
  return maxBottomRightOffset;
}


void sprite::updatePosAbs(int y, int x)
{ //add in bound's checking latter!
  position.y = y, position.x = x; // Update position.
}


/* Direction's that ch can take on.
   Q---W---E
   |...^...|
   A.<-|->.D
   |...v...|
   z---S---X
*/
void sprite::updatePosRel(const sprite::directions dir)
{
  position = getNewPos(dir);
}
