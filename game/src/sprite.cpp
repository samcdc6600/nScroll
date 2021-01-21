#include <stdexcept>
#include <curses.h>
#include <iostream>
#include "include/sprite.hpp"
#include "include/loadAssets.hpp"
#include "include/collapse.hpp"// For second phase
#include "include/draw.hpp"
#include "include/common.hpp"


sprite::sprite(const yx max, const yx pos, const directions dir, const char spriteFileName [])
  : maxyx(max), position(pos), direction(dir), currentSliceNumber(0),
    startTime(std::chrono::high_resolution_clock::now()),
    currentTime(std::chrono::high_resolution_clock::now())
{
  if(!checkDirection(dir))
    {
      std::stringstream e {};
      e<<"Error when initialising sprite: direction ("<<dir<<") out of range.";
      exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  loadSprite(spriteFileName, sD_base);
  getMaxYXOffset();
}


void sprite::loadSprite(const char spriteFileName [], spriteData & sD)
{
  // Load sprite asset--------------------------------------------------------------------------------      
  std::string rawSprite {};// Holds raw contents of sprite file.
  if(!loadASCIIFile(spriteFileName, rawSprite))
    {
      std::stringstream e {};
      e<<"Error opening sprite file \""<<spriteFileName<<"\" :'( .";
      exit(e.str().c_str(), ERROR_OPENING_FILE);
    }
  // Call parserPhaseOne------------------------------------------------------------------------------
  std::vector<std::vector<partiallyProcessedSliceLine>> pPSpriteSliceS {parserPhaseOne(rawSprite, sD)};
  // Call parserPhaseTwo------------------------------------------------------------------------------
  // Call collapse on each slice as a whole and then store slices back into container.      
  parserPhaseTwo(pPSpriteSliceS, sD);
  // Append sD to spriteS vector.
  spriteS.push_back(sD);
  /* Set currentSliceNumber to 0. This veriable should only take on values between 0 and
     (spriteSlices.size() -1) */
  currentSliceNumber = 0;
  setUpBoundryCoordsVector(sD);
}


void sprite::getMaxYXOffset()
{ /* I tried my best to make maxBottomRightOffset const, however I was thwarted by a seg fault that would occur when
     calling resize() on a vector in getSprite() when getSprite() was called in the constructors member initializer
     list >:'( */
  size_t max {};
  for(sliceData s: sD_base.spriteSlices) // Get y offset :).
    max = s.slice.size() > max ? s.slice.size() : s.slice.size();
  maxBottomRightOffset.y = max -1; // The upper left of the sprite is (0,0) so we need size -1 :).
  max = 0;
  for(sliceData s: sD_base.spriteSlices)
    for(sliceLine sl: s.slice)
      max = sl.sliceLine.size() > max ? sl.sliceLine.size() : max;
  maxBottomRightOffset.x = max -1;
}


void sprite::setUpBoundryCoordsVector(spriteData & sD)
{				// std::vector<std::vector<std::vector<int>>>
  for(auto s: sD.spriteSlices)
    {
      switch(s.slice.size())
	{
	case 0:
	  continue;		// Nothing to do
	case 1:			// Only one line in slice.
	  getBoundryCoordsForWholeSingleSliceLine(s.slice, SLICE_LINE_ONE, s.sliceBoundryCoords);      
	  break;
	case 2:			// Only two lines in slice.
	  getBoundryCoordsForWholeSingleSliceLine(s.slice, SLICE_LINE_ONE, s.sliceBoundryCoords);
	  getBoundryCoordsForWholeSingleSliceLine(s.slice, SLICE_LINE_TWO, s.sliceBoundryCoords);
	  break;
	default:     		// More then two lines (general case.)
	  {
	    size_t iter {SLICE_LINE_ONE};
	    constexpr int endOffset {1};
	    getBoundryCoordsForWholeSingleSliceLine(s.slice, iter, s.sliceBoundryCoords);
	    iter++;		// Advance to next slice line.
	    for( ; iter < s.slice.size() -endOffset; ++iter)
	      getBoundryCoordsForEndSofSingleSliceLine(s.slice, iter, s.sliceBoundryCoords);
	    getBoundryCoordsForWholeSingleSliceLine(s.slice, iter, s.sliceBoundryCoords);
	  }
	}
    }
}


void sprite::getBoundryCoordsForWholeSingleSliceLine(std::vector<sliceLine> & s, const int y,
						     std::vector<yx> & sliceBoundryCoords)
{
  for(int iter {}; iter < s[y].sliceLine.size(); ++iter)
    {
      if(s[y].sliceLine[iter] != TRANS_SP)
	{
	  yx c {y, iter + s[y].offset};
	  sliceBoundryCoords.push_back(c);
	}
    }
}


/* Operation is the same as getBoundryCoordsForWholeSingleSliceLine with the exception that only the coordinates
   plus s[y].offset of end (non TRANS_SP) chars are added to sliceBoundryCoords. If all character's are TRANS_SP
   then no coords are added and if there is only one non TRANS_SP character then only it's coordinate plus offset is
   added. */
void sprite::getBoundryCoordsForEndSofSingleSliceLine(std::vector<sliceLine> & s, const int y,
						      std::vector<yx> & sliceBoundryCoords)
{
  constexpr int subZero {-1};
  int first {subZero}, last {subZero}; // Keep track of the first non TRANS_SP char and of the last non TRANS_SP char.
  for(size_t iter {}; iter < s[y].sliceLine.size(); ++iter)
    {
      if(s[y].sliceLine[iter] != TRANS_SP)
	{			// We have seen a non TRANS_SP char.
	  if(first < 0)
	    {			// It's the first one.
	      first = iter;
	      yx c {y, first + s[y].offset};
	      sliceBoundryCoords.push_back(c); // Add first choord.
	    }
	  else
	    {
	      last = iter;
	    }
	}
    }
  if(first != last && last != subZero)
    {				// Add last choord.
      yx c {y, last + s[y].offset};
      sliceBoundryCoords.push_back(c);
    }
}


bool sprite::checkDirection(const directions dir)
{ // Return's false if there is no match for dir.
  return {(dir == DIR_UP ||
	   dir == DIR_RIGHT_UP ||
	   dir == DIR_RIGHT ||
	   dir == DIR_RIGHT_DOWN ||
	   dir == DIR_DOWN ||
	   dir == DIR_LEFT_DOWN ||
	   dir == DIR_LEFT ||
	   dir != DIR_LEFT_UP)};
}


std::vector<std::vector<sprite::partiallyProcessedSliceLine>> sprite::parserPhaseOne(const std::string & spriteFile,
										     spriteData & sD)
{
  if(spriteFile.size() < 8)
    {				// Sprite file too short. Malformed.
      std::stringstream errorMsg;
      errorMsg<<"in sprite/sprite.cpp "
	      <<" sdt::vector<std::vector<partiallyProcessedSliceLine>> sprite::parserPhaseOne(const std::string & "
	"spriteFile)"
	      <<" Error spriteFile.size() < 8. spriteFile name = ";
      throw std::logic_error(errorMsg.str());
    }
  int iter{3}; /* stores what position we are upto in the parse. start's off as 3 becaue it is used after the initial
		  character sequence. */
  // Get cs value----------------------------------------------------------------------
  if(spriteFile[0] == 'c' && spriteFile[1] == 's' && spriteFile[2] == '(')
    {
      std::stringstream cS {}; // Will hold ASCII representation of cycleSpeed0 integer veriable.
      bool runGetCs {true}; // Have we obtained and converted the cycle speed value yet?
      for( ; runGetCs; ++iter)
	{
	  if(iter >= spriteFile.size())
	    {			// Malformed cs value.
	      std::stringstream errorMsg;
	      errorMsg<<"in sprite/sprite.cpp "
		      <<" sdt::vector<std::vector<partiallyProcessedSliceLine>> sprite::parserPhaseOne(const "
		"std::string & spriteFile)"
		      <<" Error malformed cs value. spriteFile name = ";
	      throw std::logic_error(errorMsg.str());
	    }
	  else
	    if(!std::isdigit(spriteFile[iter]) && spriteFile[iter] != ')')
	      {			// Malformed cs value
		std::stringstream errorMsg;
		errorMsg<<"in sprite/sprite.cpp "
			<<" sdt::vector<std::vector<partiallyProcessedSliceLine>> sprite::parserPhaseOne(const "
		  "std::string & spriteFile)"
			<<" Error malformed cs value (illegal character). spriteFile name = ";
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
				<<" sdt::vector<std::vector<partiallyProcessedSliceLine>> "
			  "sprite::parserPhaseOne(const std::string & spriteFile)"
				<<" Error malformed cs value (missing value). spriteFile name = ";
			throw std::logic_error(errorMsg.str());
		      }
		    cS>>sD.cycleSpeed; // Set sD.sliceLine to ASCII representation of cycle speed from sprite file.
		    runGetCs = false;
		    iter++; // Move to next positon for next phase of parsing.
		    continue;
		  }
		cS<<spriteFile[iter]; // Get next part of ASCII representation of cycle speed.
	      }	      
	}
    }
  else
    {				// Begining of sprite file malformed.
      std::stringstream errorMsg;
      errorMsg<<"in sprite/sprite.cpp "
	      <<" sdt::vector<std::vector<partiallyProcessedSliceLine>> sprite::parserPhaseOne(const std::string & spriteFile)"
	      <<" Error spriteFile does not start with the sequence \"cs(\". spriteFile name = ";
      throw std::logic_error(errorMsg.str());
    } // End of get cs value----------------------------------------------------------------------------------------
  // Iter should now point to the second line.
  return getSliceLines(spriteFile, iter);  // Read sprite slices.
}


std::vector<std::vector<sprite::partiallyProcessedSliceLine>> sprite::getSliceLines(const std::string & spriteFile,
										    int & iter)
{
  std::vector<std::vector<partiallyProcessedSliceLine>> spriteSlices{};
  bool readSlices {true};
  for(int sliceIter {}; readSlices; ++sliceIter) // Get partially processed slices.
    {				// Get partially processed slice lines.
      while(spriteFile[iter] != 's' && spriteFile[iter + 1] != 'e')
	{
      	  if(spriteFile[iter] == '\n')
	    ++iter; // Make sure we do not accidently add the new line character.
	  spriteSlices.resize(sliceIter + 1); // +1 because sliceIter start's at 0.
	  // Save partially processed slice line to slice spriteSlices[sliceIter].
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


sprite::partiallyProcessedSliceLine sprite::getSliceLine(const std::string & spriteFile, int & spFIter)
{			    // Partially processed slice to be returned.
  partiallyProcessedSliceLine ppsl {};

  for( ; spriteFile[spFIter] == ' '; ++spFIter)	// Get left space cound.
    ppsl.offset++;		// Update left space count.
  if(spriteFile[spFIter] == '\n')
    { /* There is nothing but spaces on this line (left spcae ppsl.offset not currently supported on line with just
	 spaces). */
      for(int iter {}; iter < ppsl.offset; ++iter)
	ppsl.sliceLine.push_back(' ');
      spFIter++;	   // Update spFIter to point to the next line.
      //      ppsl.sliceLine.push_back('\0');
      return ppsl;		// We have gotton all the info.
    }
  else
    {
      for( ; spriteFile[spFIter] != '\n'; ++spFIter)
	{
	  ppsl.sliceLine.push_back(spriteFile[spFIter]); // Get remaining side of sprite slice line.
	}
      //      ppsl.sliceLine.push_back('\0');
      spFIter++;	    // Update spFIter to pint to the next line.
      return ppsl;
    }

  std::stringstream errorMsg;
  errorMsg<<"in sprite/sprite.cpp "
	  <<"at end of void getSliceLine(const std::string & spriteFile, int & spFIter, "
	  <<"int & ppsl.offset, std::string & ppsl.sliceLine). Error this point should never be reached.";
  throw std::logic_error(errorMsg.str());
}


// Calls collaps on slice lines and stores the lines in a type corresponding to the return value.
void sprite::parserPhaseTwo(const std::vector<std::vector<sprite::partiallyProcessedSliceLine>> & pPSL,
			    spriteData & sD)
{
  sD.spriteSlices.resize(pPSL.size()); // Make outer vector (number of slices) the right size.

  for(size_t sliceIter{}; sliceIter < pPSL.size(); ++sliceIter) // Iterte through slices.
    {				// Iterate throught slice lines.
      for(size_t sliceLineIter{}; sliceLineIter < pPSL[sliceIter].size(); ++sliceLineIter)
	{ // Make slice at sliceIter the right size (number of slice lines).
	  sD.spriteSlices[sliceIter].slice.resize(pPSL[sliceIter].size());
	  // Collapse and copy slice line.
	  collapse(pPSL[sliceIter][sliceLineIter].sliceLine,
		   sD.spriteSlices[sliceIter].slice[sliceLineIter].sliceLine);
	  // Copy offset value over.
	  sD.spriteSlices[sliceIter].slice[sliceLineIter].offset = pPSL[sliceIter][sliceLineIter].offset;
	}
    }
  refresh();
}


const yx sprite::getMaxBottomRightOffset()
{
  return maxBottomRightOffset;
}


yx sprite::getNewPos(const directions dir)
{
  yx d {};
  switch(dir)
    {
    case DIR_LEFT_UP:
      d.y = position.y -1;
      d.x = position.x -1;
      break;
    case DIR_UP:
      d.y = position.y -1;
      d.x = position.x;
      break;
    case DIR_RIGHT_UP:
      d.y = position.y -1;
      d.x = position.x +1;
      break;
    case DIR_LEFT:
      d.y = position.y;
      d.x = position.x -1;
      break;
    case DIR_RIGHT:
      d.y = position.y;
      d.x = position.x +1;
      break;
    case DIR_LEFT_DOWN:
      d.y = position.y +1;
      d.x = position.x -1;
      break;
    case DIR_DOWN:
      d.y = position.y +1;
      d.x = position.x;
      break;
    case DIR_RIGHT_DOWN:
      d.y = position.y +1;
      d.x = position.x +1;
      break;
    default:
      std::stringstream e {};
      e<<"Error direction ("<<char(dir)<<") not valid.";
      exit(e.str().c_str(), ERROR_INVALID_DIRECTION);
    }

  return d;
}


/* Returns the of position of the sprite after moving one character (including diagonal movement) in the
   direction dir */
yx sprite::peekAtPos(const directions dir)
{
  return getNewPos(dir);
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


void sprite::draw(int spriteNum, bool updateSlice)
{
  //  checkSpriteRanges(spriteNum);
  /*  printw("currentSliceNumber = ");
      refresh();
      sleep(1000);*/
  for(size_t sliceLine{}; sliceLine < spriteS[spriteNum].spriteSlices[currentSliceNumber].slice.size(); ++sliceLine)
    {      
      for(size_t sliceLineIter{};
	  sliceLineIter < spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine.size();
	  ++sliceLineIter)
	{ // Move curser to the right position.
	  setCursor(position.y + sliceLine, position.x +
		    spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].offset + sliceLineIter, maxyx);
	  // Get the character.
	  int ch {spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine[sliceLineIter]};
	  drawCh(ch);

  
	  if(updateSlice)
	    {
	      currentTime = std::chrono::high_resolution_clock::now();
	      if(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() >
		 spriteS[spriteNum].cycleSpeed)
		{
		  startTime = std::chrono::high_resolution_clock::now();
		  currentSliceNumber++; // Move to next slice
		  // -1 because indexing from 0, so currentSliceNumber shouldn't be bigger then size() -1
		  if(currentSliceNumber > (spriteS[spriteNum].spriteSlices.size() -1))
		    currentSliceNumber = 0; // We have displayed all the slices so the value should wrape arround.
		}      
	    }
	}
    }
}
