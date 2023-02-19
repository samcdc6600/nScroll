#ifndef SPRITE_HPP_
#define SPRITE_HPP_
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include "background.hpp"
#include "utils.hpp"


class sprite
{
  //=========================== Member Variables ===============================
public:
  enum directions
    {				// Direction a sprite can move in.
      /* Sprites should have either 1 set of slices (in which case they should
	 only index into spriteS with DIR_NONE) or 5 sets of slices (in which case
	 they should only index into spriteS with DIR_NONE, DIR_UP, DIR_RIGHT,
	 DIR_DOWN and DIR_LEFT) */
      DIR_NONE,
      DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT,
    };
  
private:
  // enum sliceLineIndex
  //   {				// Used as index into sliceLine vector.
  //     SLICE_LINE_ONE,
  //     SLICE_LINE_TWO
  //   };
  
protected:
    const yx viewPortSize;
  /* Sprites should only have these numbers of sets of slices. 5
     and not 4 because we must account for DIR_NONE. */
  static constexpr int spriteNums1 {1}, spriteNums2 {5};
  // Stores the animation directions a sprite instance can use.
  std::vector<directions> spriteAnimationDirections {};
  /* Position of the top left corner of the sprite object relative to the
     window. */
  yx position;
  /* Holds the maximum bottom right offset. Calculated from all slices. Used
     (along with position) for inital collision detection and bounds checking */
  yx maxBottomRightOffset {};
  directions direction {DIR_NONE};
    /* Holds the sprite animation (slice) that we are upto in the sequences.
     Should not go above spriteSlices.size(); and should wrape back around to
     zero. */
  int currentSliceNumber;
  /* StartTime and currentTime keep track of when sprite animations can be
     updated. */
  std::chrono::high_resolution_clock::time_point startTime =
    std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point currentTime =
    std::chrono::high_resolution_clock::now();  
    
private:
  struct sliceLine
  {
    std::vector<unsigned short> sliceLine;	// Holds line of slice.
    int offset;	// Horizontal offset of sliceLine from current curser position.
  };
  struct sliceData
  { // A slice is essentially one frame of a sprite animation.
    std::vector<sliceLine> slice {};
    // // Hold's a list of boundry coordinates for the slice.
    // std::vector<yx> sliceBoundryCoords {};
  };
  struct partiallyProcessedSliceLine
  {
    std::string sliceLine;	// Holds unprocessed slice line.
    int offset;	// Horizontal offset of sliceLine from current curser position.
  };
  
protected:
  /* Hold's a single sprite instance that will be setup by this ABC in it's
     constructor */
  struct spriteData
  { /* Speed of sprite animation. */
    int cycleSpeed;
    /* Outer vector holds inner vectors that hold sliceLine objects
       that make up the horizontal lines of slices.
       This object holds the slices that make up the sprite. */
    std::vector<sliceData> spriteSlices {};
  };
  // The set of slices for each sprite direction are stored here.
  std::vector<spriteData *> spriteSliceSets {};
  // This vector hold's the sprites (sprite slices and cycle speed's.)
  std::vector<spriteData> spriteS;

  //=========================== Member Functions ===============================
public:
  /* This constructor reads the sprite file/s located at spritePaths[x] and
     converts it's/their contents to the internal data structure needed by the
     sprite. */
  sprite(std::vector<std::string> & spritePaths, const yx max,
	 const backgroundData & background, const yx pos, const directions dir,
	 const bool fullyIn = false);
  ~sprite();
private:
  // Split up file into cycleSpeed and unprocessesd representation of sprite.
  std::vector<std::vector<partiallyProcessedSliceLine>>
  parserPhaseOne(const std::string & spriteFile, spriteData & sD);
  /* Extracts sprite slice lines by calling getSliceLine(const std::string &,
     int &) and pushing each slice line onto a vector of
     partiallyProcessedSliceLine's which it then returnes. */
  std::vector<std::vector<partiallyProcessedSliceLine>>
  getSliceLines(const std::string & spriteFile, int & iter);
  /* Extracts sprite slice line from spriteFile and places in spriteSliceLine,
     if there are spaces to the left places in lSpace and finally spFIter udates
     iter to new position. */
  partiallyProcessedSliceLine getSliceLine(const std::string & spriteFile,
					   int & spFIter);
  /* Calls collaps on slice lines and stores the lines in a type corresponding
     to the return value */
  void parserPhaseTwo
  (const std::vector<std::vector<sprite::partiallyProcessedSliceLine>> & pPSL,
   spriteData & sD);
  /* Sets maxBottomRightOffset to the maximum yOffset and xOffset as calculated
     from the tallest spriteSlice and longest sliceLine in sD_basespriteSlices.
     The offsets are interprited as a point at (0,0) or to the lower left of
     position. These values are used for collision detection and bounds
     checking. */
  void getMaxYXOffset();
  
protected:
  // /* Checks that the sprite is in a level chunk. If fullyIn is set to false only
  //    one character of the sprite needs to be in a chunk. If it is set to true
  //    the sprite must be fully visible within the chunk. Exits with an error
  //    message if the sprite isn't in a chunk. GetMaxYXOffset() should be called
  //    before this function to set maxBottomRightOffset. */ 
  // void checkInitialPosIsInLevelChunk
  // (std::vector<std::string> & spritePaths, const yx maxBottomRightOffset,
  //  const backgroundData & background, const bool fullyIn);
  /* Initialises sD_base */
  void loadSprite(const char spriteFileName [], spriteData & sD);
  void resetCurrentSliceNum()
  {
    currentSliceNumber = 0;
  }
  /* Sprites should have either 1 set of slices 4 sets of slices 8. This
     function checks that spriteS.size() is equal to one of these values and
     adds the appropriate directions from directions to
     spriteAnimationDirections. */
  void initialiseDirectionsVector();
  /* Checks to make sure that dir is one of the values found in the enum
     directions. Returns dir if it is.*/
  directions checkDirection(const directions dir);
  /* Returns a yx struct that is a copy of the data member position with it's y
     and x member's incremented, decremented or left unchanged depending on the
     value of dir. */
  yx getNewPos(const directions dir);
  bool checkBoundValue(const int bound);
  // void checkBoundValue(const std::string callerName, const int bound);
  // // void checkBoundValues(const std::string callerName, const int yBound,
  // // 			const int xBound);
  
public:
  /* Y is a potential y position of the sprite (e.g. peekPos.y) and yMargin is
     the margin size in the y dimension. Returns true if the sprite is not in
     the inner margins in the y axis, that is the sprite is in the inner most
     part of the window. */
  bool notInWindowInnerMarginY(const int y, const int yMargin);
    /* X is a potential x position of the sprite (e.g. peekPos.x) and xMargin is
     the margin size in the x dimension. Returns true if the sprite is not in
     the inner margins in the x axis, that is the sprite is in the inner most
     part of the window. */
  bool notInWindowInnerMarginX(const int x, const int xMargin);
  // /* Y is a potential y position of the sprite (e.g. peekPos.y). Returns true
  //    if the sprite (with it's y position set to y) would be within the window in
  //    the y axis. */
  // bool inWindowY(const int y);
  // /* X is a potential x position of the sprite (e.g. peekPos.x). Returns true
  //    if the sprite (with it's x position set to x) would be within the window
  //    in the x axis. */
  // bool inWindowX(const int x);
  // bool inLevelY(const int y, const int bgYLen);
  // bool inLevelX(const int x, const int bgXLen);
  bool leftOfWindowInnerRightMargin(const int x, const int xBound,
				    const yx viewPortSize);
  bool rightOfWindowInnerLeftMargin(const int x, const int xBound);
  // /* Checks that y and x are in range (0 to screen height and width), by calling
  //    checkRange(const int, const int). */
  // bool inWindow(const int y, const int x);
  /* Returns the of position of the sprite after moving one character (including
     diagonal movement) in the direction dir */
  yx peekAtPos(const directions dir);
  // Returns the sprite position.
  yx getPos() const {return position;}
  std::string getXPosAsStr() const
  {
    std::stringstream ss {};
    ss<<position.x;
    return ss.str();
  }
  std::string getYPosAsStr() const
  {
    std::stringstream ss {};
    ss<<position.y;
    return ss.str();
  }
  /* Returns largest bottom right coordinates (relative to the start of the
   sprite.) Used for simple collision detection. */
  yx getMaxBottomRightOffset() const;
  directions getDirection() {return direction;}
  void updateDirection(const directions dir)
  {
    checkDirection(dir);
    resetCurrentSliceNum();
    direction = spriteAnimationDirections[dir];
  }
  /* update's position of sprite in an absoulte fashion with reference to the
     background */
  virtual void updatePosAbs(int y, int x);
  /* update's sprite's y and x position value's by a difference of no more then
     1. The direction depends on the value of ch. */
  virtual void updatePosRel(const directions dir);
};

#endif
