#ifndef SPRITE_H_
#define SPRITE_H_
#include <vector>
#include <string>
#include <chrono>
#include "../common.h"

// This class should be inherited by other classes such as
// PlayerSprite and enimySprite and nonInterativeSprite
class sprite
{
 private:
  
  const yx maxyx;		// Window size.
  
 protected:
  
    yx position;     // Position of sprite relative to background and top left corner of sprite object.
    /* Holds the maximum bottom right offset. Calculated from all slices.*/
    yx maxBottomRightOffset; /* Used for inital (possible) collision detection and bounds checking */
    
 private://------------------------------------------------------------------------------------------------------
 
  // Holds the sprite animation (slice) that we are upto in the sequences.
  // Should not go above spriteSlices.size(); and should wrappe back around to zero.  
  int currentSliceNumber;
  // These two veriables keep track of when sprites can be updates
  std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    struct sliceLine
  {
    std::vector<int> sliceLine;	// Holds line of slice.
    int offset;	// Horizontal offset of sliceLine from current curser position.
  };
  struct partiallyProcessedSliceLine
  {
    std::string sliceLine;	// Holds unprocessed slice line.
    int offset;	// Horizontal offset of sliceLine from current curser position.
  };
  
 protected:
  
  struct spriteData
  { /* Speed of sprite animation. */
    int cycleSpeed;
    /* Outer vector holds inner vectors that hold sliceLine objects
       that make up the horizontal lines of slices.
       This object holds the slices that make up the sprite. */
    std::vector<std::vector<sliceLine>> spriteSlices {};
  } sD_base;    // Hold's a single sprite instance that will be setup by this ABC in it's constructor.
  
 private:  
  
  // Split up file into cycleSpeed and unprocessesd representation of sprite.
  std::vector<std::vector<partiallyProcessedSliceLine>> parserPhaseOne(const std::string & spriteFile,
								       spriteData & sD);
  /* Extracts sprite slice lines by calling getSliceLine(const std::string &, int &) and pushing each slice line
     onto a vector of partiallyProcessedSliceLine's which it then returnes. */
  std::vector<std::vector<partiallyProcessedSliceLine>> getSliceLines(const std::string & spriteFile, int & iter);
  /* Extracts sprite slice line for spriteFile and places in spriteSliceLine, if there are spaces to the left places 
     in lSpace and finally spFIter udates iter to new position. */
  partiallyProcessedSliceLine getSliceLine(const std::string & spriteFile, int & spFIter);
  // Calls collaps on slice lines and stores the lines in a type corresponding to the return value
  void parserPhaseTwo(const std::vector<std::vector<sprite::partiallyProcessedSliceLine>> & pPSL, spriteData & sD);
  
 protected:
  // This vector hold's the sprites (sprite slices and cycle speed's.)
  std::vector<spriteData> spriteS;
  
  /* Initialises sD_base */
  void getSprite(const char spriteFileName [], spriteData & sD);
  /* Sets maxBottomRightOffset to the maximum yOffset and xOffset as calculated from the tallest spriteSlice and
     longest sliceLine in sD_basespriteSlices. The offsets are interprited as a point at (0,0) or to the lower left 
     of position. These values are used for possible collision detection and bounds checking. */
  void getMaxYXOffset();
  void resetCurrentSliceNum(int sprite)
  {
    currentSliceNumber = 0;
  }
  /* Checks that y and x are in range (0 to screen height and width), by calling checkRange(const int, const int). */
  inline bool inScreenBounds(const int y, const int x)
  {     /* The reason for having to call checkRange twice is so that it may be a more generic function and we done
	   have to change it or create another auxulary functino. The runtime overhead of calling it twice is seen as
	   exceptible (given that is program is probably very far from optimal in many other dimensions. */
    bool ret = ((checkRange(y, maxyx.y) && checkRange(x, maxyx.x)) ? true : false) &&
      ((checkRange(y + maxBottomRightOffset.y, maxyx.y) && checkRange(x + maxBottomRightOffset.x, maxyx.x))
       ? true : false);
    return ret;
  }
  /* Checks if position.y and position.x are in range and if the spriteNum is in range. Throws and exception if 
     there is an error */
  void checkSpriteRanges(const int spriteNum);
  /* We can get a const version of maxBottomRightOffset in a derived class (couldn't make maxBottomRightOffset in
     sprite. At least we can force it for derived classes.) */
  const yx getMaxBottomRightOffset();
  
 public://----------------------------------------------------------------------------------------------------------
  
  //constructor reads spriteFile and converts it to the internal object structure
  sprite(const yx max, const yx pos, const char spriteFileName []);
  virtual void updatePosAbs(int y, int x);	/* update's position of sprite in an absoulte fashion with reference
						   to the background */
  virtual void updatePosRel(const char ch);	/* update's position of sprite in a relative fashion with reference to
						   the sprite. The argument tell's witch direction to increment. */
  virtual void draw(int spriteNum, bool updateSlice);	/* displays sprite (potentially more then one file's worth of
							   sprite) */
  virtual void draw(bool updateSlice) = 0; /* For sprites that only have one set of slices. */
  virtual void draw() {draw(false);}	   /* Draw same sprite slice as before. */
  virtual ~sprite() {};
};

#endif
