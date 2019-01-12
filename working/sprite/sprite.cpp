#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <iostream>
#include "sprite.h"
#include "../initial/load/loadASCIIFile.h"
#include "../initial/collapse/collapse.h"// For second phase
#include "../draw/draw.h"


sprite::sprite(const yx max, const yx pos, const std::string & spriteFileName)
  : maxyx(max), position(pos), currentSliceNumber(0),
    startTime(std::chrono::high_resolution_clock::now()),
    currentTime(std::chrono::high_resolution_clock::now())
{
  getSprite(spriteFileName, sD_base);
  getMaxYXOffset();
}


void sprite::getSprite(const std::string & spriteFileName, spriteData & sD)
{
  try
    {
      // Load sprite asset--------------------------------------------------------------------------------      
      std::string rawSprite {};// Holds raw contents of sprite file.
      if(!loadASCIIFile(spriteFileName.c_str(), rawSprite))
	{
	  std::stringstream errorMsg;
	  errorMsg<<"In \"sprite::sprite(const std::string & spriteFile)\", in sprite/sprite.cpp. Error opening ";
	  throw std::logic_error(errorMsg.str());
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
    }
  catch(std::logic_error errorMsg)
    {	     /* adds spriteFileName.c_str() to exception and rethrows. */
      mvprintw(0, 0, "spriteFileName.c_str() = %s", spriteFileName.c_str());
      throw errorMsg;
    }
}

  /* returns the maximum yOffset and xOffset as calculated from the tallest spriteSlice and longest sliceLine in
     sD_basespriteSlices. The offsets are interprited as a point at (0,0) or to the lower left of position. These
     values are used for possible collision detection and bounds checking. */
void sprite::getMaxYXOffset()
{ /* I tried my best to make maxBottomRightOffset const, however I was thwarted by a seg fault that would occur when
     calling resize() on a vector in getSprite() when getSprite() was called in the constructors membor initializer
     list >:'( */
  int max {};
  for(std::vector<sliceLine> slice: sD_base.spriteSlices) // Get y offset.
    max = slice.size() > max ? slice.size() : max;
  maxBottomRightOffset.y = max -1; // The offset is from the upper left of the sprite. So we account for that with -1.
  max = 0;
  for(std::vector<sliceLine> slice: sD_base.spriteSlices) // Get x offset.
    for(sliceLine sl: slice)
      max = sl.sliceLine.size() > max ? sl.sliceLine.size() : max;
  maxBottomRightOffset.x = max -1;
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

  for(int sliceIter{}; sliceIter < pPSL.size(); ++sliceIter) // Iterte through slices.
    {				// Iterate throught slice lines.
      for(int sliceLineIter{}; sliceLineIter < pPSL[sliceIter].size(); ++sliceLineIter)
	{ // Make slice at sliceIter the right size (number of slice lines).
	  sD.spriteSlices[sliceIter].resize(pPSL[sliceIter].size());
	  // Collapse and copy slice line.
	  collapse(pPSL[sliceIter][sliceLineIter].sliceLine, sD.spriteSlices[sliceIter][sliceLineIter].sliceLine);
	  // Copy offset value over.
	  sD.spriteSlices[sliceIter][sliceLineIter].offset = pPSL[sliceIter][sliceLineIter].offset;
	}
    }
  refresh();
}


void sprite::checkSpriteRanges(const int spriteNum)
{
  if(position.y < 0 || position.y > maxyx.y || position.x < 0 || position.x > maxyx.x)
    {
      std::stringstream errorMsg;
      errorMsg<<"in sprite/sprite.cpp\n"
	      <<"void sprite::draw(int spriteNum). position.y or position.x value out of range,"
	      <<"\nwhere range is from (0,0) to ("<<maxyx.y<<","<<maxyx.x<<"). and the position.y and position.x \n"
	      <<"coordinates were ("<<position.y<<","<<position.x<<")\n";
      throw std::logic_error(errorMsg.str());
    }
  if(spriteNum > spriteS.size() || spriteNum < 0)    
    {
      std::stringstream errorMsg;
      errorMsg<<"in sprite/sprite.cpp\n"
	      <<"void sprite::draw(int spriteNum)."
	      <<" the value of spriteNum that was passed to the function is out of range. "
	      <<"spriteNum - 1 ="<<spriteNum<<", spriteS.size() = "<<spriteS.size()<<"\n";
      throw std::logic_error(errorMsg.str());
    }  
}

const yx sprite::getMaxBottomRightOffset()
{
  return maxBottomRightOffset;
}


void sprite::updatePosAbs(int y, int x)
{ //add in bound's checking latter!
  /* Position can't be outside of the background file and player class should have a version of this function that
     makes sure that it's position is always within a certin boundary that fall's within the visiable section of the
     background. */
  position.y = y, position.x = x; // Update position.
}


/* Direction's that ch can take on.
Q---W---E
|...^...|
A.<--->.D
|...v...|
z---S---X
 */
void sprite::updatePosRel(const char ch)
{
  try
    {
      switch(ch)
	{
	case 'q':
	case 'Q':		// Left up.
	  --position.y;
	  --position.x;
	  break;
	case 'w':
	case 'W':		// Up.
	  --position.y;
	  break;
	case 'e':
	case 'E':		// Right up.
	  --position.y;
	  ++position.x;
	  break;
	case 'a':
	case 'A':		// Left.
	  --position.x;
	  break;
	case 'd':
	case 'D':		// Right.
	  ++position.x;
	  break;
	case 'z':
	case 'Z':		// Left down.
	  ++position.y;
	  --position.x;
	  break;
	case 's':
	case 'S':		// Down.
	  ++position.y;
	  break;
	case 'x':
	case 'X':		// Right down.
	  ++position.y;
	  ++position.x;
	  break;      
      
	default:	
	  std::stringstream e;
	  e<<"Ch out of range."
		  <<"range = [q, w, e, a, d, z, s, x] & upper case forms."
		  <<"ch = "<<ch;
	  throw std::logic_error(e.str());
	}
    }
  catch(std::logic_error eMsg)
    {
      endwin();
      std::cerr<<"In \"void sprite::updatePosRel(char ch)\", in sprite/sprite.cpp.\n"
	       <<eMsg.what()<<std::endl;
      exit(ERR_POS_CH_RANGE);
    }
}


void sprite::draw(int spriteNum, bool updateSlice)
{
  //  checkSpriteRanges(spriteNum);
  for(int sliceLine{}; sliceLine < spriteS[spriteNum].spriteSlices[currentSliceNumber].size(); ++sliceLine)
    {      
      for(int sliceLineIter{};
	  sliceLineIter < spriteS[spriteNum].spriteSlices[currentSliceNumber][sliceLine].sliceLine.size();
	  ++sliceLineIter)
	{ // Move curser to the right position.
	  setCursor(position.y + sliceLine, position.x +
		    spriteS[spriteNum].spriteSlices[currentSliceNumber][sliceLine].offset + sliceLineIter, maxyx);
	  // Get the character.
	  int ch {spriteS[spriteNum].spriteSlices[currentSliceNumber][sliceLine].sliceLine[sliceLineIter]};
	  drawCh(ch);
	}
  
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
