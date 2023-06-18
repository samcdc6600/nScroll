#ifndef SPRITE_HPP_
#define SPRITE_HPP_
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include "background.hpp"
#include "utils.hpp"


/* This class (sprite) is the top level class for all sprites and should
   contain only (as a general rule) things that are common to all child
   classes (such bgSprite sprites and animateSprite. */
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
  
protected:
  /* Position of the top left corner of the sprite object relative to the
     window. */
  yx positionVPRel;
  directions direction {DIR_NONE};

  class spriteTimers
  {
    /* This class is used to store fixedTimeStep timing for sprites. Classes
       that inherit from sprite will probably need to add things to this class
       (this can be done using the following syntax, which of course technically
       is inheriting from spriteTimers):
	    "
	    class childClass : public sprite
	    {
	    	...
		class spriteTimers: public sprite::spriteTimers
		{
		...
		};
		...
	    }
	    "
       The timers variable should also be masked with a variable of the new
       classes type. */
    double timeSinceLastSliceUpdate {};
  protected:
    /* Time between invocations of time related operations on sprite (assuming
       the computer system is fast enough.) */
    const double fixedTimeStep;
    
  public:    
    spriteTimers(const double fTS):
      fixedTimeStep(fTS)
    {}


    /* This func should be called each draw tick to update the time since the
       last slice change. */
    void updateSliceTimer();
    /* This func should be called each draw tick to see if it is time to update
       the slice number again. */
    double getTimeSinceLastSliceUpdate() const;
    /* This func should be called each time the slice number is updated. */
    void resetTimeSinceLastSliceUpdate();
  };

  spriteTimers timers;
  
  const yx viewPortSize;
  /* Sprites should only have these numbers of sets of slices. 5
     and not 4 because we must account for DIR_NONE. */
  static constexpr int spriteNums1 {1}, spriteNums2 {5};
  // Stores the animation directions a sprite instance can use.
  std::vector<directions> spriteAnimationDirections {};
  /* Holds the maximum bottom right offset. Calculated from all slices. Used
     (along with position) for inital collision detection and bounds checking */
  yx maxBottomRightOffset {};
    /* Holds the sprite animation (slice) that we are upto in the sequences.
     Should not go above spriteSlices.size(); and should wrape back around to
     zero. */
  int currentSliceNumber;
    
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
  {
    /* Speed of sprite animation in ms. */
    double cycleSpeed;
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
  sprite(const double fixedTimeStep, std::vector<std::string> & spritePaths,
	 const yx max, const yx pos, const directions dir);
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
     The offsets are interpreted as a point at (0,0) or to the lower left of
     position. These values are used for collision detection and bounds
     checking. */
  void setMaxYXOffset();
  
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
  bool leftOfWindowInnerRightPadding(const int x, const int xBound,
				    const yx viewPortSize);
  bool rightOfWindowInnerLeftPadding(const int x, const int xBound);
  // /* Checks that y and x are in range (0 to screen height and width), by calling
  //    checkRange(const int, const int). */
  // bool inWindow(const int y, const int x);
  /* Returns the of position of the sprite after moving one character (including
     diagonal movement) in the direction dir */
  yx peekAtPos(const directions dir);

  
  // Returns the relative sprite position.
  yx getPos() const {return positionVPRel;}

  
  std::string getXPosAsStr() const
  {
    std::stringstream ss {};
    ss<<positionVPRel.x;
    return ss.str();
  }

  
  std::string getYPosAsStr() const
  {
    std::stringstream ss {};
    ss<<positionVPRel.y;
    return ss.str();
  }
  
  /* Returns largest bottom right coordinates (relative to the start of the
   sprite.) Used for simple collision detection. */
  yx getMaxBottomRightOffset() const;

  
  directions getDirection() const
  {
    return direction;
  }

  
  void updateDirection(const directions dir)
  {
    checkDirection(dir);
    resetCurrentSliceNum();
    direction = spriteAnimationDirections[dir];
  }

  
  /* Sets relative position to newRelPos */
  virtual void updatePos(const yx newRelPos);
  /* Adds to relative position based on the value of dir. */
  virtual void updatePos(const directions dir);
};

#endif
