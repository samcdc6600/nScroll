#ifndef SPRITE_H_
#define SPRITE_H_
#include <vector>
#include <string>
#include <chrono>
#include "../common.h++"

// This class should be inherited by other classes such as
// PlayerSprite and enimySprite and nonInterativeSprite
class sprite
{
public:

  // Direction Characters.
  enum directions
    {
      LEFT_UP ='q',
      LEFT_UP_UPPER ='Q',
      UP ='w',
      UP_UPPER ='W',
      RIGHT_UP ='e',
      RIGHT_UP_UPPER ='E',
      LEFT ='a',
      LEFT_UPPER ='A',
      RIGHT ='d',
      RIGHT_UPPER ='D',
      LEFT_DOWN ='z',
      LEFT_DOWN_UPPER ='Z',
      DOWN ='s',
      DOWN_UPPER ='S',
      RIGHT_DOWN ='x',
      RIGHT_DOWN_UPPER ='X'
    };

  enum directionCodes
    {
      DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN
    };
  
private:
  
  const yx maxyx;		// Window size.
  enum sliceLineIndex
    {				// Used as index into sliceLine vector.
      SLICE_LINE_ONE,
      SLICE_LINE_TWO
    };
  
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
  
  struct sliceData
  {
    std::vector<sliceLine> slice {};    
    std::vector<yx> sliceBoundryCoords {}; // Hold's a list of boundry coordinates for the slice.
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
    std::vector<sliceData> spriteSlices {};
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
  /* Sets maxBottomRightOffset to the maximum yOffset and xOffset as calculated from the tallest spriteSlice and
     longest sliceLine in sD_basespriteSlices. The offsets are interprited as a point at (0,0) or to the lower left 
     of position. These values are used for possible collision detection and bounds checking. */
  void getMaxYXOffset();
  // Iterates through spriteSlices in sD and set's up boundry coords for each slice in sD.
  void setUpBoundryCoordsVector(spriteData & sD);
  /* Addes the index of every character in s[y].sliceLine that is not equal to TRANS_SP plus s[y].offset to
     sliceBoundryCoords (using .push_back().) */
  void getBoundryCoordsForWholeSingleSliceLine(std::vector<sliceLine> & s, const int y,
					       std::vector<yx> & sliceBoundryCoords);
  /* Operation is the same as getBoundryCoordsForWholeSingleSliceLine with the exception that only the coordinates
     plus s[y].offset of end (non TRANS_SP) chars are added to sliceBoundryCoords. If all character's are TRANS_SP
     then no coords are added and if there is only one non TRANS_SP character then only it's coordinate plus offset is
     added. */
  void getBoundryCoordsForEndSofSingleSliceLine(std::vector<sliceLine> & s, const int y,
						std::vector<yx> & sliceBoundryCoords);
  
protected:
  // This vector hold's the sprites (sprite slices and cycle speed's.)
  std::vector<spriteData> spriteS;

  /* Initialises sD_base */
  void getSprite(const char spriteFileName [], spriteData & sD);
  void resetCurrentSliceNum()
  {
    currentSliceNumber = 0;
  }
  
  inline bool inBounds(const int y, const int x, const int innerBoarderY, int innerBoarderX)
  {				// Check that y, x and also y + offset and x + offset are within the windo - innerBoarder7 and innerboarderx etc lksdajfldsa.
    //    return ((checkRange(y, 
  }
  /* Checks that y and x are in range (0 to screen height and width), by calling checkRange(const int, const int). */
  inline bool inScreenBounds(const int y, const int x)
  {
    bool ret = ((checkRange(y, 0, maxyx.y) && checkRange(x, 0, maxyx.x)) ? true : false) &&
      ((checkRange(y + maxBottomRightOffset.y, 0, maxyx.y) && checkRange(x + maxBottomRightOffset.x, 0, maxyx.x))
       ? true : false);
    return ret;
  }
  /* Checks if position.y and position.x are in range and if the spriteNum is in range. Throws and exception if 
     there is an error */
  
  // Return's false if there is no match for dir.
  bool checkDirection(const directions dir)
  {
    return (dir != LEFT_UP && dir != LEFT_UP_UPPER && dir != UP && dir != UP_UPPER &&
	    dir != RIGHT_UP && dir != RIGHT_UP_UPPER && dir != LEFT && dir != LEFT_UPPER &&
	    dir != RIGHT && dir != RIGHT_UPPER && dir != LEFT_DOWN && dir != LEFT_DOWN_UPPER &&
	    dir != DOWN && dir != DOWN_UPPER && dir != RIGHT_DOWN && dir != RIGHT_DOWN_UPPER) ? false : true;
  }
  /* We can get a const version of maxBottomRightOffset in a derived class (couldn't make maxBottomRightOffset in
     sprite. At least we can force it for derived classes.) */
  const yx getMaxBottomRightOffset();
  /* Returns a yx struct that is a copy of the data member position with it's y and x member's incremented,
     decremented or left unchanged depending on the value of dir. */
  yx getNewPos(const directions dir);
  
public://----------------------------------------------------------------------------------------------------------
  
  //constructor reads spriteFile and converts it to the internal object structure
  sprite(const yx max, const yx pos, const char spriteFileName []);
  /* Returns the of position of the sprite after moving one character (including diagonal movement) in the
     direction dir */
  yx peekAtPos(const directions dir);
  // update's position of sprite in an absoulte fashion with reference to the background
  virtual void updatePosAbs(int y, int x);
  /* update's sprite's y and x position value's by a difference of no more then 1. The direction depends on the
     value of ch. */
  virtual void updatePosRel(const directions dir);
  // displays sprite (potentially more then one file's worth of sprites.)
  virtual void draw(int spriteNum, bool updateSlice);
  // For sprites that only have one set of slices.
  virtual void draw(bool updateSlice) = 0;
  // Draw same sprite slice as before.
  virtual void draw() {draw(false);}
  virtual ~sprite() {};
};

#endif
