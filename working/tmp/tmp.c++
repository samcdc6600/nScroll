/* should check that the bounds file that has been read in is the same length as the background file
after it has been collapsed into int's. The length should be a comparison of number of char's to
number of int's */
#ifndef CHECKBOUNDSOFBOUNDS_H_
#define CHECKBOUNDSOFBOUNDS_H_
#include <string>
#include <vector>

/* should return an integer that represents the difference in "size" between the background file and
the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background); 

#endif
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


// Code for 
//Value of transparent space is 1 higher then 159 * 64 which is above the space of all ACS and ASCII characters whether colored or not.
constexpr int TRANS_SP {10176};
//"Collapse's" buff and stores into ret. copies all characters not part of an escape sequence into
//ret (one char goes into one int (thus ret may actually be larger then buff (but I say it is collapsed
//for reasons explained shortly)). All escape sequences are not copied reather they are used as an
//index into a chMap object and the return value (an int that should be unique) is stored into one value
//of ret instead of all the characters that make up the escape sequence.
//nonColor escape sequences start with /nc
void collapse(const std::string & buffWithSpaces, std::vector<int> & ret);

#endif
#ifndef COLOUROFFSET_H_
#define COLOUROFFSET_H_
#include <map>
#include <string>
#include <vector>


class colourMap//this classes main function is to help in converting the background input from char's to int's
{
 private:
  std::map<std::string, int> map;//stores color types to int offsets
  std::vector<std::string> colorPairs
    {//BLACK_WHITE is defined as pair zero and cannot be changed and thus must be first
      //      "WHITE_BLACK",
      "BLACK_BLACK", "BLACK_RED", "BLACK_GREEN", "BLACK_YELLOW", "BLACK_BLUE", "BLACK_MAGENTA", "BLACK_CYAN",
      "BLACK_WHITE", "RED_BLACK", "RED_RED", "RED_GREEN", "RED_YELLOW", "RED_BLUE", "RED_MAGENTA", "RED_CYAN",
      "RED_WHITE", "GREEN_BLACK", "GREEN_RED", "GREEN_GREEN", "GREEN_YELLOW", "GREEN_BLUE", "GREEN_MAGENTA",
      "GREEN_CYAN", "GREEN_WHITE", "YELLOW_BLACK", "YELLOW_RED", "YELLOW_GREEN", "YELLOW_YELLOW", "YELLOW_BLUE",
      "YELLOW_MAGENTA", "YELLOW_CYAN", "YELLOW_WHITE", "BLUE_BLACK", "BLUE_RED", "BLUE_GREEN", "BLUE_YELLOW",
      "BLUE_BLUE", "BLUE_MAGENTA", "BLUE_CYAN", "BLUE_WHITE", "MAGENTA_BLACK", "MAGENTA_RED", "MAGENTA_GREEN",
      "MAGENTA_YELLOW", "MAGENTA_BLUE", "MAGENTA_MAGENTA", "MAGENTA_CYAN", "MAGENTA_WHITE", "CYAN_BLACK",
      "CYAN_RED", "CYAN_GREEN", "CYAN_YELLOW", "CYAN_BLUE", "CYAN_MAGENTA", "CYAN_CYAN", "CYAN_WHITE", "WHITE_RED",
      "WHITE_GREEN", "WHITE_YELLOW", "WHITE_BLUE", "WHITE_MAGENTA", "WHITE_CYAN", "WHITE_WHITE"
    };
 public:
  colourMap();
  int getRange(const std::string str)
  {
    return map.at(str);//return tange (offset)
  }
};

class setColorMode//This class is used to change the color mode of Ncurses
{
 private:
  const int colorMax {63};
  const int defaultColor {};
  void setState(const int state);//set the color state in Ncurses
  bool inRange(const int color);//is the color passed to this function in range?
 public:
  setColorMode(const int color);
  void setColor(const int color);//set color to color
  void clearColor();//set color to default
};

#endif
#ifndef DIRECTION_H_ 
#define DIRECTION_H_

//direction's that the sprite can move in
enum {DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN};

#endif
#ifndef DRAW_EXIT_H
#define DRAW_EXIT_H


void drawExit(const yx maxyx);


#endif

#ifndef DRAW_H_
#define DRAW_H_
#include <vector>
#include "../common.h++"
#include "../sprite/sprite.h++"
#include "../physics/physics.h++"


#define DRAW_NO_OP 10176	/* This character should be skipped */
extern const int yHeight;


void draw(const std::vector<int> & buff,
	  const std::map<std::string, std::vector<rules::spriteInfo>> & nonPlayerSprites,
	  player * playerSprite, const yx maxyx, const unsigned long offset); /* Draws all that is visible :) */
/* Places curser at (0,0) and iterates over the current background slice, calling draw for each character. */
void drawBackground(const std::vector<int> & buff, const yx maxyx, const unsigned long offset);
/* Checks that y and x are in range and then calls mvprintw(y, x, "") */
void setCursor(const int y, const int x, const yx maxyx);
/* Does the actual drawing (e.g. calls printw() or addch() and set's colour appropriately. 
   NOTE: I had to rename this function "drawCh" from "draw" because I was getting a segmentation fault when
   calling it from sprie::draw in sprite.cpp. Sprite does not contain a member function with the same signature as
   this function, so I'm really not sure what the problem is. I suspect it has something to do with sprite::draw
   being a memeber function and maybe I had unwittingly strayed into undefined behaviour :O. In any case I think
   drawCh is a more apt name anway :)*/
void drawCh(int ch);
bool inColorRange(const int ch); /* Checks whethere ch is a colour character or not. */
int getColor(const int ch);	 /* Returns colour code encoded in ch. */
#endif
#ifndef INITCURSES_H_
#define INITCURSES_H_

#include <string>
#include <ncurses.h>
#include <sstream>
#include "../common.h++"


constexpr int yHeight {33}, xWidth{125};        // The window must be these dimensions                                 


void initialiseCurses(yx & maxyx);
void initColorPairs();


#endif
#ifndef LEVELINTERACTIONHANDLER_H_
#define LEVELINTERACTIONHANDLER_H_
#include <string>
#include <vector>

char intersection(const std::string & boundsInteraction, const std::vector<int> spChoords);//check level agains sprite
char intersection(const std::vector<int> playerSpChoords, const std::vector<int> spChoords);//check player against sprite
char nearPass(const std::string & boundsInteraction, const std::vector<int> spChoords);//check level against sprite
char nearPass(const std::vector<int> playerSpChoords, const std::vector<int> spChoords);//check player against sprite

#endif
#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
#include <fstream>
#include "../../physics/physics.h++" // For rules.


constexpr char HEADER_START [] = {"(p("}; // The header should always start with me.
constexpr char FIELD_START_DENOTATION {'('};	 // Marks the start of a new field or section of the current file.
constexpr char FIELD_END_DENOTATION {')'};	 // Marks the end of the current field or section of the current file.
constexpr char HEADER_END_DENOTATION {'#'}; // Marks the end of the header sectino of the file.
constexpr char STRING_DENOTATION {'"'}; // This character denotes the start of a sting
constexpr int SHOW_COUNT {45};		// How many characters to print in printN function.
// This character denotes the presence of another string when used after a string.
constexpr char STRINGS_SEPERATION {','};
constexpr char ESCAPE_CHAR {'\\'}; // The character used for escape sequences (within a string) in .rules.lev file's.
constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
constexpr char NULL_BYTE {'\0'};
/* Coordinate character's. I have decided to add an uppercase alternative for each character. This is so that
   rules.lev file's will be more forgiving of "mistakes." */
constexpr char BOARDER_CHAR {'b'}; // Player character's cannot pass through coordinate's marked as this.
constexpr char BOARDER_CHAR_UPPER {'B'};
constexpr char KILL_CHAR {'k'};	// Player character's are killed uppon intersection with coordinate's marked as this.
constexpr char KILL_CHAR_UPPER {'K'};
constexpr char DEGRADE_HEALTH_CHAR {'d'}; // Player character's loose health uppon intersection with coordinate's
constexpr char DEGRADE_HEALTH_CHAR_UPPER {'D'}; // marked as this.
constexpr char DEGRADE_HEALTH_BOARDER_CHAR {'i'}; // Combination of DEGRADE_HEALTH_CHAR and BOARDER_CHAR.
constexpr char DEGRADE_HEALTH_BOARDER_CHAR_UPPER {'I'};
constexpr char END_LEV_CHAR {'e'}; // Player character's "win" the level uppon intersection with coordinate's marked
constexpr char END_LEV_CHAR_UPPER {'E'}; // as this.
constexpr char LIFE_POWER_UP_CHAR {'l'}; // Player character's gain health point's when comming into contact
constexpr char LIFE_POWER_UP_CHAR_UPPER {'L'};


/* This function should initialise the argument background that is passed to it. This variable should then contain
   the "grphical data" of the level. It should initialise the levelRules argument that is passed to it, This variable
   should then contain the "rules and non player sprites" as well as the player sprite of the level. The bgFileName
   and rulesFileName are the names of the files that should be read in and parsed to initialise the background and
   levelRules arguments. */
void loadAssets(const yx maxyx, const char bgFileName [], std::vector<int> & background,
		const char rulesFileName [], rules & levelRules);
/* Opens file "name" and copies it's content's into buff. If there is an error opening the file returns false.
   Return's true otherwise. */
bool loadASCIIFile(const char name [], std::string & buff);
/* Load rules file, parse header, extract sprite coordinates and sprite name/s and process sprites. extract sprite
   rule after processing each sprite.
   Header should contain file location and starting position of player sprite and the player should be initialised
   using this info. The header may also contain other information. 
   When a sprite coordinate, sprite and rule triple has been extracted add sprite and rule using sprite coordinate as
   key to a vector in the spriteCoords map in the form of a spriteInfo struct */
void parseAndInitialiseRules(const yx maxyx, const char rulesFileName [], rules & levelRules);
/* Extract and parse header info in buff. */
void parse(const yx maxyx, std::string & buff, const char rulesFileName [], rules & levelRules);
/* Increment's (advances) i by (n -1), if i equals iEnd before being incremented n times we throw an exception. */
template <typename T_A, typename T_B> auto getAdvancedIter(T_A i, T_B iEnd, const size_t n) -> T_A
{
  for(int iter {}; iter < (n -1); ++iter)
    {
      i++;
      if(*i == *iEnd)
	throw std::out_of_range ("range error");
    }
  return i;
}
/* Branch to differnt parsing function's depending on the values of current and peek. InHeader tells whether we are in
   the header (parsing decisions may be different depending on it's state.) */
void switchOnCurrent(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		    std::string::const_iterator & peek, std::string::const_iterator max, rules & levelRules);
// Dispatches functions based on the value of *peek. Should only be called when *current == FIELD_START_DENOTATION
void handleCurrentIsFieldStartDenotation(const yx maxyx, std::string & buff, std::string::const_iterator & current,
					 std::string::const_iterator & peek, std::string::const_iterator max,
					 rules & levelRules);
/* Read in the character *current (which should already have been checked for validity), then read in
   the coordinate pair, finally the function should check to see that this coordinate pair is unique in the object
   levelRules.charCoords and if it is use the coordinates as a key to store the character (which is to be interprited
   as a Coordinate character */
void handleCoordinateCharacter(std::string & buff, std::string::const_iterator & current,
			    std::string::const_iterator & peek, std::string::const_iterator & max,
			    rules & levelRules);
/* Read in the character *current (which should already have been checked for it's validity), then read in
   the coordinate pair, finally the function should check to see that this coordinate pair is unique in the object
   levelRules.charCoords and if it is use the coordinates as a key to store the character (which is to be interprited
   as a Coordinate character */
void handleCoordinateCharacter(std::string::const_iterator & current, std::string::const_iterator & peek,
			    std::string::const_iterator & max, const rules & levelRules);
/* Calls handleStringDenotationAfterFieldDenotation to read in string's. Then calls getCoords to read in coordinates.
Then initialises player */
void initPlayerSprite(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator & max, rules & levelRules);
/* Check's for coordinate of the form "(Y,X)" and return's "Y,X" (discarding "(" and ")") if Y and X are valid
   integer number's that do not falloutside of the X and Y values in maxyx and are greater or equal to zero.  */
std::string getCoords(const std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, const std::string::const_iterator & max);
/* Read's in a number of string's separated by the STRING_SEPERATION character. Stop when current reaches the
  character where a STRING_SEPERATION character is expected. Returns each string read in a vector. */
std::vector<std::string> handleStringDenotationAfterFieldDenotation(std::string & buff,
							       std::string::const_iterator & current,
							       std::string::const_iterator & peek,
							       std::string::const_iterator max);
/* Called when we encounter the STRING_DENOTATION character. Extract's and returns string (dealing with escape
   character's.) Call's exit if there is an error */
std::string getString(std::string::const_iterator & current, std::string::const_iterator & peek,
		      const std::string::const_iterator max);
/* Increment's peek untill it point's to a non white space character. At which point proceeds to delete
   character's [current, peek) if (++current != peek). Return's false if peek becomes >= max while incrementing.
   It is assumed that current and peek point to consecutive elment's */
bool rubOutSpace(std::string & buff, std::string::const_iterator & current, std::string::const_iterator & peek,
	       std::string::const_iterator & max);
void printCurrent(std::string::const_iterator & current, const int SHOW_COUNT, std::stringstream & e);


#endif
#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "../sprite/sprite.h++"
#include "../sprite/player/player.h++"

struct rules
{
  // The player cannot pass within this many character's of the windo boarder's
  const yx playerNoGoInnerBoarder {5, 8};
  struct spriteInfo		/* For sprites (holds sprite data (slices) and the rule for the sprite. */
  {
    /* Sprite data to go here (when I do it.) */
    sprite * sp {};		/* This has to be a pointer because sprite is an ABC. */
    std::string rule {};
  };
  /* The key's for these maps should be of the form "Y,X" Where Y and X are numbers and the whole thing is a string */
  std::map<std::string, char> coordChars {}; /* For coordinate rules. */
   /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer;		/* Info about user controlled sprite (AKA player.) */
  ~rules()
  {
    delete(gamePlayer);
  }

private:
  int playerPosition {};

  // Moves the player 
  void movePlayer(const sprite::directions input);

public:
  int physics(const int input);  
};

#endif
#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include "../sprite.h++"
#include "../../common.h++"
#include "direction.h++"	//contains an enumeration with direction definitions


class player: public sprite
{/*this class should expand on the ABC sprite by adding the following member's and methods:
   spriteSliceS1 (shold hold the slices that make up the sprite), cycleSpeed1 (should hold the cycle speed of the
   second sprite), direction (should hold the direction that the player is moving in. The direction should also
   dictate whitch of the two sprite's is used), health (should hold the health of the player),
   -----------------METHODS FOR THIS CLASS SHOULD BE LISTED HERE----------------------*/

  //these objects hold the slices that make up the 2nd, 3rd and 4th sprites
  static constexpr int NUM_PLAYER_SPRITES {4};
  static constexpr int INITIAL_SPRITE_INDEX {0};
  static constexpr int ADDITIONAL_SPRITES_OFFSET {1};
  spriteData sD_player [NUM_PLAYER_SPRITES];
  int health;
  int direction;
  //this function has intentionally been left in a private section of this class and with an empty
  //body because it is not intended for use.
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}
 public:
  //read's sprite files and converts them so they may be used with the classes draw function/s
 player(std::vector<std::string> sprites, const yx max, const yx pos, const int h, const int d)
   : sprite(max, pos, sprites[INITIAL_SPRITE_INDEX].c_str()), health(h), direction(d)
  {
    if(!(d >= DIR_LEFT && d <= DIR_DOWN))
      {
	std::stringstream e {};
	e<<"Error when initialising player: direction ("<<d<<") out of range.";
	exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
      }
    if(sprites.size() != NUM_PLAYER_SPRITES)
      {
	std::stringstream e {};
	e<<"Error when initialising player: "<<sprites.size()<<" player sprites, but expected "
	 <<NUM_PLAYER_SPRITES<<" sprites.";
	exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
      }

    for(int iter {ADDITIONAL_SPRITES_OFFSET}; iter < NUM_PLAYER_SPRITES; ++iter)
      {
	getSprite(sprites[iter].c_str(), sD_player[iter]);
      }
  }
  
  
  virtual ~player() {};
  virtual void updatePosRel(directions dir);
  virtual void draw(bool updateSlice);
  //  virtual void draw() {draw(false);}
  
};

#endif
#ifndef SLICE_H_
#define SLICE_H_
#include <vector>

//returns vector<int> or size y x at offset from buff. Note buff must be y hight and at least x long
//(offset + x cannot go past end of (array / y) and offset should not be negative (however at the
//current time offset is an unsigned long so there is no required check). This should be checked for
//in the physics code and should cause an exception in this code.)
std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x);

#endif
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
  void resetCurrentSliceNum(int sprite)
  {
    currentSliceNumber = 0;
  }
  inline bool inBounds(const int y, const int x, const int innerBoarderY, innerBoarderX)
  {				// Check that y, x and also y + offset and x + offset are within the windo - innerBoarder7 and innerboarderx etc lksdajfldsa.
    return ((checkRange(y, 
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
  void checkSpriteRanges(const int spriteNum);
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
#include "checkBoundsOfBounds.h++"


/* should return an integer that represents the difference in "size" between the background file and
the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background)
{  
  return boarder.size() - background.size();
}
#include <stdexcept>
#include "collapse.h++"
#include "../../draw/colorS.h++"
#include <ncurses.h>
//#include <sstream>

chMap::chMap()
{
  int chVal {128};//128 is one above the ascii character set
  for(std::string strIter: characters)//insert all the spcial character types into the map along with their key values
    {
      strMap[strIter]=chVal;//enter element into make with a key of strIter and a value of offset
      ++chVal;//increment chVal (each character must have a uniqe value not equal to any ascii value).
    }
}

int chMap::find(const std::string str)
{//map.at() throws and out of range exception if no matching key is found
  return strMap.at(str);//returns value that corresponds to the key str
}


void collapse(const std::string & buffWithSpaces, std::vector<int> & ret)
{//collaps special character escape sequences on first pass and colour escape sequences on second.
  ret.clear();//make suren that ret is empty.
  //declared as static so that the constructor will only be called once. This is an optimization since the constructor does
  static chMap escCharNums;//a lot of work that is redundent if done more then once.
  static colourMap cuMap;//static for the same reason as the last veriable. (this class converts to and from colour offsets).
  //---------------------------------(First pass (get rid of spaces))---------------------------------------------
  std::string buff {};
  for(int iter{}; iter < buffWithSpaces.size(); ++iter)
    {//get rid of spaces
      if(buffWithSpaces[iter] != '\n')
	buff.push_back(buffWithSpaces[iter]);
    }
  
  std::string escSeq{};
  std::vector<int> preRet{};
  //----------------------------------(Second pass (encode special characters))-------------------------------------
  for(int iter{}; iter < buff.size(); ++iter)//collaps "\nc/" ecsape sequences (special characters).
    {//if we have found the start of a nonColour escape sequence
      if(buff[iter] == '\\' && buff[1 + iter] == 'n' && buff[2 + iter] == 'c')
	{
	  iter += 3;//skip first three characters of escape sequencse ("\nc").
	  if(iter > buff.size())//check for errors in formatting
	    {//if iter is out side of range.
	      std::string eMsg {"in collaps.cpp ->collaps(const std::string & buffWithSpace, std::vector<int> ret). First pass. iter is > buff.size. Iter ="};
	      eMsg += iter;
	      eMsg += buff.size();
	      eMsg += '\n';
	      throw std::logic_error(eMsg);
	    }
	  for( ; buff[iter] != '/'; ++iter)
	    {//the next two if statments check for errors
	      if(iter > buff.size())
		{
		  std::string eMsg {"in collaps.cpp->collaps(const std::string & buffWithSpace, std::vector<int ret). iter ="};
		  eMsg += iter;       eMsg += "\n";
		  throw std::logic_error(eMsg);
		}
	      escSeq += buff[iter];//get nonColour escape sequence		
	    }
	  //set next element in preRet to int value that corresponds to escSeq nonColour escape sequence
	  preRet.push_back(escCharNums.find(escSeq));
	  escSeq.clear();//clear escSeq so that it is empty next time we find a nonColour escape sequence and use += on escSeq
	}
      else
	{//if we have found a normal character or a transparent space (no character is displayed) character. 
	  if(buff[iter] == '\\' && buff[1 + iter] == 't' && buff[2 + iter] == 'r' && buff[3 + iter] == 'a' && buff[4 + iter] == 'n'
	     && buff[5 + iter] == 's' && buff[6 + iter] == 'S' && buff[7 + iter] == 'P' && buff[8 + iter] == '/')
	    {
	      preRet.push_back(TRANS_SP);
	      iter += 8;
	    }
	  else
	    preRet.push_back(buff[iter]);
	}
    }

  //------------------------------(Third pass (encode color characters))-------------------------------------------
  for(int iter{}; iter < preRet.size(); ++iter)
    {
      if(char(preRet[iter]) == '\\' && char(preRet[1 + iter]) == 'c' && char(preRet[2 + iter]) == 'u')
	{//we are in the start of a color escape sequence
	  iter += 3;
	  if(iter > preRet.size())
	    {
	      throw std::logic_error("iter += 3 if(iter > preRet.size())");
	    }
	  for( ; char(preRet[iter]) != '/'; ++iter)
	    {
	      if(iter > preRet.size())
		{
		  throw std::logic_error("for( ; char(preRet[iter]) != '/'; ++iter) if(iter > preRet.size())");
		}
	      escSeq += preRet[iter];//escSeq holds the string used to get the color code cuMap
	    }//we have the color information (in escSeq)
	  iter++;
	  bool inColor {true};
	  for(int iterColor{iter}; inColor; ++iterColor)
	    {//now apply color information to characters
	      if((iterColor - 3) >= preRet.size())
		{
		  mvprintw(0, 0, "iterColor = %d, preRet.size() = %d ", iterColor, preRet.size());
			   getch();
		  throw std::logic_error("if(iterColor >= preRet.size())");
		}
	      if(char(preRet[iterColor]) == '\\' && char(preRet[iterColor + 1]) == 'c' && char(preRet[iterColor + 2]) == 'x' && char(preRet[iterColor + 3]) == '/')
		{//have we reached the closing escape sequences?
		  inColor = false;
		  iter = iterColor + 3;
		}
	      else
		{//apply color information to character
		  if(preRet[iterColor] != 10176)//if the character is not a "transparent space" character!
		    {//non "transparent space" characters have color information applied here.
		      ret.push_back(preRet[iterColor] + cuMap.getRange(escSeq));
		    }
		  else
		    {//transparent space! (this character is a special case and so color information should not be applied to it).
		      ret.push_back(preRet[iterColor]);
		    }
		}
	    }
	  inColor = true;
	  escSeq.clear();
	}
      else
	{//character with default color
	  ret.push_back(preRet[iter]);
	}
    }
}
#include <ncurses.h>
#include "colorS.h++"

colourMap::colourMap()
{
  const int offset{159};
  int colourVal {offset};//range of characters
  for(std::string colourIter: colorPairs)
    {
      map[colourIter] = colourVal;//associate color pair with range (offset)
      colourVal += offset;//set to next range
    }
}

void setColorMode::setState(const int state)
{
  if(!inRange(state))
    {//state is out of range
      std::string eMsg {"in colorS.cpp->setState(const int state), state = "};
      eMsg += state;
      eMsg += "\n";
      throw std::logic_error(eMsg);      
    }
  attron(COLOR_PAIR(state));
}

bool setColorMode::inRange(const int color)
{
  if(color < 0 || color > colorMax)
    return false;//color is out of range
  return true;//color is in range
}

setColorMode::setColorMode(const int color): defaultColor(color)//set the default color and the color state
{
  if(!inRange(color))
    {
      std::string eMsg {"in colorS.cpp->setColorMode(const int color), color = "};
      eMsg += color;
      eMsg += "\n";
      throw std::logic_error(eMsg);
    }
  setState(defaultColor);//set default color
}

void setColorMode::setColor(const int color)
{
  if(!inRange(color))//is the color variable out of range?
    {
      std::string eMsg {"in colorS.cpp->setColor(const int color), color = "};
      eMsg += color;
      eMsg += "\n";
      throw std::logic_error(eMsg);
    }
  setState(color);//turn on color color
}

void setColorMode::clearColor()
{
  setState(defaultColor);//turn on defaultColor color
}
#include <ncurses.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "../common.h++"
#include "slice.h++"
#include "draw.h++"
#include "colorS.h++"


// Class handels the setting of color when printing characters (used in draw.cpp) argument is the default color pair.
setColorMode colorMode{56}; //If argument to object constructor is changed it must also be changed in main.cpp.



void draw(const std::vector<int> & buff,
	  const std::map<std::string, std::vector<rules::spriteInfo>> & nonPlayerSprites,
	  player * playerSprite, const yx maxyx, const unsigned long offset)
{
  drawBackground(buff, maxyx, offset);
  /*
    /* Draw non player sprites. All this should be needed to be able to uncomment this and have it compile and work is
    to add in a check to make sure that the sprite "it" is in a visible part of the screen (to make sure we're not
    doing useless work!) * /
  for(MapType::iterator it = nonPlayerSprites.begin(); it != nonPlayerSprites.end; ++it)
    {
      if(it.second.in_range(maxyx, iter);
	 it.second->draw(true);
    }
  */
  /* The physics functions should ensure that the palyer should never be out of the visible range, so no check should
     be required. */
  playerSprite->draw(true);
  refresh();
}


void drawBackground(const std::vector<int> & buff, const yx maxyx, const unsigned long offset)
{
  std::vector<int> slice {getSlice(buff, offset, maxyx.y, maxyx.x)};      
  //  getSlice(buff, offSet, winWidth, slice);
  //  printw("%s", slice.c_str());

  setCursor(0, 0, maxyx);//move curser back to start of screen
  for(auto iter: slice)
    {
      drawCh(iter);
    }
}


void setCursor(const int y, const int x, const yx maxyx)
{
  try
    {
      if(y >= 0 && x >= 0)
	{
	  if(y < maxyx.y && x < maxyx.x)
	    mvprintw(y, x, "");
	  else
	    {
	      std::stringstream e;
	      e<<"In setCursor(), y and or x out of range. Range = ("<<maxyx.y<<"(y),"<<maxyx.x
		  <<"(x)). Y and x given = "<<y<<", "<<x<<" respectively.";
	      throw std::logic_error(e.str());
	    }
	}
    }
  catch(std::logic_error e)
    {      
      exit(e.what(), ERROR_CURSOR_PARAM);
    }
      
}


inline void drawCh(int ch)
{				/* Although this function is large I have decided to make it inline to increase the
				   possiblility that it will be inlined because it will typically be called many
				   times. */
  if(inColorRange(ch)) //-----------------print in non default color-------------------------------------
    {
      if(ch == DRAW_NO_OP)
	{}	// No OP. Used for sprites!
      else
	{
	  int colorCode = getColor(ch); // Get color information from ch.
	  colorMode.setColor(colorCode); // Set ncurses color pair using colorCode.
	  ch -= (colorCode * 159); // Remove color information from ch so that it may be printed.
	}
      if(ch < 128)
	{			// Normal ASCII ch.
	  printw((char *)(&ch));
	}
      else
	{
	  switch(ch)
	    {
	    case 128:		  
	      addch(ACS_ULCORNER);
	      break;
	    case 129:
	      addch(ACS_LLCORNER);
	      break;
	    case 130:
	      addch(ACS_LRCORNER);
	      break;
	    case 131:
	      addch(ACS_LTEE);
	      break;	      
	    case 132:
	      addch(ACS_RTEE);
	      break;
	    case 133:
	      addch(ACS_BTEE);
	      break;
	    case 134:
	      addch(ACS_TTEE);
	      break;
	    case 135:
	      addch(ACS_HLINE);
	      break;
	    case 136:
	      addch(ACS_VLINE);
	      break;
	    case 137:
	      addch(ACS_PLUS);
	      break;
	    case 138:
	      addch(ACS_S1);
	      break;
	    case 139:
	      addch(ACS_S3);
	      break;
	    case 140:
	      addch(ACS_S7);
	      break;
	    case 141:
	      addch(ACS_S9);
	      break;
	    case 142:
	      addch(ACS_DIAMOND);
	      break;
	    case 143:
	      addch(ACS_CKBOARD);
	      break;
	    case 144:
	      addch(ACS_DEGREE);
	      break;
	    case 145:
	      addch(ACS_PLMINUS);
	      break;
	    case 146:
	      addch(ACS_BULLET);
	      break;
	    case 147:
	      addch(ACS_LARROW);
	      break;
	    case 148:
	      addch(ACS_RARROW);
	      break;
	    case 149:
	      addch(ACS_DARROW);
	      break;
	    case 150:
	      addch(ACS_UARROW);
	      break;
	    case 151:
	      addch(ACS_BOARD);
	      break;
	    case 152:
	      addch(ACS_LANTERN);
	      break;
	    case 153:
	      addch(ACS_BLOCK);
	      break;
	    case 154:
	      addch(ACS_LEQUAL);
	      break;
	    case 155:
	      addch(ACS_GEQUAL);
	      break;
	    case 156:
	      addch(ACS_PI);
	      break;
	    case 157:
	      addch(ACS_NEQUAL);
	      break;
	    case 158:
	      addch(ACS_STERLING);
	      break;
	    case DRAW_NO_OP:
	      break;	      
	    default:
	      std::stringstream e;
	      e<<"in draw.cpp void draw(const std::vector<int> & buff, const int offSet, "
		"const int winWidth). Case = default. Ch = "<<ch;
	      exit(e.str(), ERROR_CHARACTER_RANGE);
	      break;
	    }
	}
      colorMode.clearColor(); // Set color mode back to default
    }
  else
    { //----------------------------------print in default color-------------------------------------------
      if(ch < 128)
	printw((char *)(&ch));
      else
	{
	  switch(ch)
	    {
	    case 128:		  
	      addch(ACS_ULCORNER);
	      break;
	    case 129:
	      addch(ACS_LLCORNER);
	      break;
	    case 130:
	      addch(ACS_LRCORNER);
	      break;
	    case 131:
	      addch(ACS_LTEE);
	      break;	      
	    case 132:
	      addch(ACS_RTEE);
	      break;
	    case 133:
	      addch(ACS_BTEE);
	      break;
	    case 134:
	      addch(ACS_TTEE);
	      break;
	    case 135:
	      addch(ACS_HLINE);
	      break;
	    case 136:
	      addch(ACS_VLINE);
	      break;
	    case 137:
	      addch(ACS_PLUS);
	      break;
	    case 138:
	      addch(ACS_S1);
	      break;
	    case 139:
	      addch(ACS_S3);
	      break;
	    case 140:
	      addch(ACS_S7);
	      break;
	    case 141:
	      addch(ACS_S9);
	      break;
	    case 142:
	      addch(ACS_DIAMOND);
	      break;
	    case 143:
	      addch(ACS_CKBOARD);
	      break;
	    case 144:
	      addch(ACS_DEGREE);
	      break;
	    case 145:
	      addch(ACS_PLMINUS);
	      break;
	    case 146:
	      addch(ACS_BULLET);
	      break;
	    case 147:
	      addch(ACS_LARROW);
	      break;
	    case 148:
	      addch(ACS_RARROW);
	      break;
	    case 149:
	      addch(ACS_DARROW);
	      break;
	    case 150:
	      addch(ACS_UARROW);
	      break;
	    case 151:
	      addch(ACS_BOARD);
	      break;
	    case 152:
	      addch(ACS_LANTERN);
	      break;
	    case 153:
	      addch(ACS_BLOCK);
	      break;
	    case 154:
	      addch(ACS_LEQUAL);
	      break;
	    case 155:
	      addch(ACS_GEQUAL);
	      break;
	    case 156:
	      addch(ACS_PI);
	      break;
	    case 157:
	      addch(ACS_NEQUAL);
	      break;
	    case 158:
	      addch(ACS_STERLING);
	      break;
	    case DRAW_NO_OP:
	      break;	      
	    default:
	      std::stringstream e;
	      e<<"in draw.cpp void draw(const std::vector<int> & buff, const int offSet, "
		"const int winWidth). Case = default (no colour.). Ch = "<<ch;
	      exit(e.str(), ERROR_CHARACTER_RANGE);
	      break;
	    }
	}
    }
}

bool inColorRange(const int ch)
{
  if(ch > MONO_CH_MAX)
    return true;		// In color range
  return false;			// Not in color range
}
int getColor(const int ch)
{
  int color {ch / (MONO_CH_MAX +1)};		// Ch modulo 158 equals the color code
  if(color <= COLOR_CH_MAX)		// If the color code is in range 0 - 63
    return color;

  // If the color code is out of range
  std::string e {"in draw.cpp->getColor(const int ch). color = "};
  e += color;
  e += "\n";
  exit(e, ERROR_COLOR_CODE_RANGE);
  throw std::logic_error(e);
}
#include <string>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "../common.h++"
#include "drawExit.h++"
#include "draw.h++"


void drawExit(const yx maxyx)
{
  const std::string swanSong {"~~*-F-A-R-E-W-E-L-L-*~~"};
  int padding {1};
  bool direction {true};	// True indicates a "growing" direction (i.e. depletion zone around swanSong.)
  int sleepTime {10};	// :)
  constexpr int sleepTimeStep {4};	// How much to increment or decrement sleep time by.
  constexpr int frames {62};
  int ch {};  
  srand(time(NULL));
  
  for(int iter {}; iter < frames; ++iter)
    {
      for(int xIter {}; xIter < maxyx.x; ++xIter)
	{
	  for(int yIter {}; yIter < maxyx.y; ++yIter)
	    {
	      if(swanSong.size() != 0) // Check that message has non zero size.
		{		  
		  if((xIter > ((maxyx.x /2) - (padding + swanSong.size() /2))) &&
		     (xIter < ((maxyx.x /2) + (padding + swanSong.size() /2))) &&
		     (yIter > ((maxyx.y /2) - padding)) &&
		     (yIter < ((maxyx.y /2) + padding)))
		    {
		      if(xIter == ((maxyx.x /2) - (swanSong.size() /2)) &&
			 yIter == (maxyx.y /2))
			 mvprintw(yIter, xIter, swanSong.c_str());
		    }
		  else
		    {
		      setCursor(yIter, xIter, maxyx);
		      ch = (rand() % MONO_CH_MAX) * (rand() % COLOR_CH_MAX);
		      drawCh(ch);
		    }
									       
		}
	    }
	}
      refresh();
      sleep(sleepTime);
      if(iter == (frames /2))
	direction = false;	/* We've printed half the frames so reverse the direction of growth of the 
				   depletion zone around swanSong and start decresing sleeptime. :) */
      if(direction)
	{
	  ++padding;
	  if(iter < frames /4)
	    sleepTime += sleepTimeStep -2;
	  else
	    sleepTime += sleepTimeStep;
	}
      else
	{
	  --padding;
	  if(iter > (frames - (frames /4)))
	    sleepTime -= sleepTimeStep -2;
	  else
	    sleepTime -= sleepTimeStep;
	}
    }
}
#include "initCurses.h++"


void initialiseCurses(yx & maxyx)
{
  initscr();                                  // Start curses mode                                                     
  getmaxyx(stdscr, maxyx.y, maxyx.x); // Screen size in characters                                                     
  if(maxyx.y != yHeight || maxyx.x != xWidth) // Check the window size.                                                
    {
      std::stringstream e {};
      e<<"error windows must be "<<yHeight<<" characters high and "<<xWidth<<" characters wide. "
        "The current height is "<<maxyx.y<<" characters and the current width is "<<maxyx.x<<" characters.\n";
      exit(e.str(), ERROR_WIN_PARAM);
    }
  if(has_colors() == FALSE)  // If the terminal does not support colors                                                
    {
      exit("Error your terminal does not support colour :'(\n", ERROR_WIN_PARAM);
    }
  raw();                     // To disable line buffering                                                              
  curs_set(0);               // Make the cursor invisible                                                              
  noecho();                  // Turn echoing off on the terminal                                                       
  start_color();             // Start color and initialise color pairs!                                                
  nodelay(stdscr, TRUE);     // Dissable blocking while waiting for input (use non blocking sys call.)                 
  initColorPairs();          // Initialise the color pairs                                                             
}


void initColorPairs()
{
  attron(COLOR_PAIR(56));       //default color. Must also be changed in draw.cpp!!!!!!!!!!!!!!!!!!!!!!!!              
  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_RED);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_BLACK, COLOR_BLUE);
  init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(7, COLOR_BLACK, COLOR_CYAN);
  init_pair(8, COLOR_BLACK, COLOR_WHITE);
  init_pair(9, COLOR_RED, COLOR_BLACK);
  init_pair(10, COLOR_RED, COLOR_RED);
  init_pair(11, COLOR_RED, COLOR_GREEN);
  init_pair(12, COLOR_RED, COLOR_YELLOW);
  init_pair(13, COLOR_RED, COLOR_BLUE);
  init_pair(14, COLOR_RED, COLOR_MAGENTA);
  init_pair(15, COLOR_RED, COLOR_CYAN);
  init_pair(16, COLOR_RED, COLOR_WHITE);
  init_pair(17, COLOR_GREEN, COLOR_BLACK);
  init_pair(18, COLOR_GREEN, COLOR_RED);
  init_pair(19, COLOR_GREEN, COLOR_GREEN);
  init_pair(20, COLOR_GREEN, COLOR_YELLOW);
  init_pair(21, COLOR_GREEN, COLOR_BLUE);
  init_pair(22, COLOR_GREEN, COLOR_MAGENTA);
  init_pair(23, COLOR_GREEN, COLOR_CYAN);
  init_pair(24, COLOR_GREEN, COLOR_WHITE);
  init_pair(25, COLOR_YELLOW, COLOR_BLACK);
  init_pair(26, COLOR_YELLOW, COLOR_RED);
  init_pair(27, COLOR_YELLOW, COLOR_GREEN);
  init_pair(28, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(29, COLOR_YELLOW, COLOR_BLUE);
  init_pair(30, COLOR_YELLOW, COLOR_MAGENTA);
  init_pair(31, COLOR_YELLOW, COLOR_CYAN);
  init_pair(32, COLOR_YELLOW, COLOR_WHITE);
  init_pair(33, COLOR_BLUE, COLOR_BLACK);
  init_pair(34, COLOR_BLUE, COLOR_RED);
  init_pair(35, COLOR_BLUE, COLOR_GREEN);
  init_pair(36, COLOR_BLUE, COLOR_YELLOW);
  init_pair(37, COLOR_BLUE, COLOR_BLUE);
  init_pair(38, COLOR_BLUE, COLOR_MAGENTA);
  init_pair(39, COLOR_BLUE, COLOR_CYAN);
  init_pair(40, COLOR_BLUE, COLOR_WHITE);
  init_pair(41, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(42, COLOR_MAGENTA, COLOR_RED);
  init_pair(43, COLOR_MAGENTA, COLOR_GREEN);
  init_pair(44, COLOR_MAGENTA, COLOR_YELLOW);
  init_pair(45, COLOR_MAGENTA, COLOR_BLUE);
  init_pair(46, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(47, COLOR_MAGENTA, COLOR_CYAN);
  init_pair(48, COLOR_MAGENTA, COLOR_WHITE);
  init_pair(49, COLOR_CYAN, COLOR_BLACK);
  init_pair(50, COLOR_CYAN, COLOR_RED);
  init_pair(51, COLOR_CYAN, COLOR_GREEN);
  init_pair(52, COLOR_CYAN, COLOR_YELLOW);
  init_pair(53, COLOR_CYAN, COLOR_BLUE);
  init_pair(54, COLOR_CYAN, COLOR_MAGENTA);
  init_pair(55, COLOR_CYAN, COLOR_CYAN);
  init_pair(56, COLOR_CYAN, COLOR_WHITE);
  init_pair(57, COLOR_WHITE, COLOR_RED);
  init_pair(58, COLOR_WHITE, COLOR_GREEN);
  init_pair(59, COLOR_WHITE, COLOR_YELLOW);
  init_pair(60, COLOR_WHITE, COLOR_BLUE);
  init_pair(61, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(62, COLOR_WHITE, COLOR_CYAN);
  init_pair(63, COLOR_WHITE, COLOR_WHITE);
}
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
#include <iostream>
#include <sstream>
#include <cstring>
//#include <locale>
#include <ncurses.h>// test code
#include "loadAssets.h++"
#include "../collapse/collapse.h++"


void loadAssets(const yx maxyx, const char bgFileName [], std::vector<int> & background,
		const char rulesFileName [], rules & levelRules)
{
  // Initialise background.
  //constexpr char backgroundFileName [] = "assets/level1/level1.backgound.lev";
  std::string levelBackGround {};
  if(!loadASCIIFile(bgFileName, levelBackGround))
    {
      std::stringstream a {};
      a<<"Error unable to open: \""<<bgFileName<<"\"";
      exit(a.str(), ERROR_OPENING_FILE);
    }
  collapse(levelBackGround, background); //collapse nonColor escape sequences.
  // Initialise player and non-player sprites (rules file).
  parseAndInitialiseRules(maxyx, rulesFileName, levelRules);
}


bool loadASCIIFile(const char name [], std::string & buff)
{
  std::ifstream file;
  file.open(name);
  if(!file.is_open())
    {				// Couldn't open file.
      file.close();
      return false;
    }
  char c;
  while(file.get(c))		// Copy file content's to buff.
    buff.push_back(c);
  file.close();
  
  return true;
}


void parseAndInitialiseRules(const yx maxyx, const char rulesFileName [], rules & levelRules)
{
  std::string buff {};
  if(!loadASCIIFile(rulesFileName, buff)) // Load in the rules file.
    {
      std::stringstream e {};
      e<<"Error unable to open: \""<<rulesFileName<<"\"";
      exit(e.str(), ERROR_OPENING_FILE);
    }
  parse(maxyx, buff, rulesFileName, levelRules);
  // Initialise player.
  /*  yx spritePos = {10, 10}; // Set inital position for player.
      levelRules.gamePlayer = (new player("assets/sprites/sprite(0).sprite", "assets/sprites/sprite(1).sprite",
      "assets/sprites/sprite(2).sprite", "assets/sprites/sprite(3).sprite",
      maxyx, spritePos, 5, DIR_RIGHT)); // Set sprite for player */
}


void parse(const yx maxyx, std::string & buff, const char rulesFileName [], rules & levelRules)
{
  if(strncmp(buff.c_str(), HEADER_START, sizeof(HEADER_START) -1) != 0)
    {
      std::stringstream e {};
      e<<"Error header of the .rules.lev file \""<<rulesFileName<<"\" is malformed!";
      exit(e.str(), ERROR_RULES_LEV_HEADER);
    }


  try
    {     
      for(std::string::const_iterator peek {getAdvancedIter(buff.begin(), buff.end(), sizeof(HEADER_START) -1)},
	    current {peek++}; *peek != NULL_BYTE; ++peek, ++current)
	{
	  switchOnCurrent(maxyx, buff, current, peek, buff.end(), levelRules);
	}
    }
  catch (std::out_of_range & e)
    {				/* We are using a specific error code here because we are only expecting to recive an
				   exception from the getAdvancedIter function. We had decided to remove all excption
				   handling from this program. However we are going to use it in this one case to kee
				   the getAdvancedIter function more general. */
      exit(e.what(), ERROR_RULES_LEV_HEADER);
    }
}


void switchOnCurrent(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		    std::string::const_iterator & peek, std::string::const_iterator max, rules & levelRules)
{
  switch(*current)
    {
    case FIELD_START_DENOTATION:    // Handle start of new field or section.
      handleCurrentIsFieldStartDenotation(maxyx, buff, current, peek, max, levelRules);
      break;
    default:
      std::stringstream e {};
      e<<"Error current character not expected at current position in the rules.lev file. The following is the "
       <<SHOW_COUNT<<" character's after and including the current character (note output may be less then "
       <<SHOW_COUNT<<" character's if the EOF character was encountered):\n";
      printCurrent(current, SHOW_COUNT, e);
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -START- ------------------------------- -----------------------------


void handleCurrentIsFieldStartDenotation(const yx maxyx, std::string & buff, std::string::const_iterator & current,
					 std::string::const_iterator & peek, std::string::const_iterator max,
					 rules & levelRules)
{			// We have a sprite
  static bool inHeader {true};
  if(!inHeader)
    {
      switch(*peek)
	{
	case STRING_DENOTATION:
	  endwin();
	  std::cout<<"String denotation character encountered after start denotation (not in header.)"<<std::endl;
	  sleep(10000);
	  break;
	default:
	  if(*peek == BOARDER_CHAR			||	*peek == BOARDER_CHAR_UPPER
	     || *peek == KILL_CHAR			||	*peek == KILL_CHAR_UPPER
	     || *peek == DEGRADE_HEALTH_CHAR		||	*peek == DEGRADE_HEALTH_CHAR_UPPER
	     || *peek == DEGRADE_HEALTH_BOARDER_CHAR	||	*peek == DEGRADE_HEALTH_BOARDER_CHAR_UPPER
	     || *peek == END_LEV_CHAR			||	*peek == END_LEV_CHAR_UPPER
	     || *peek == LIFE_POWER_UP_CHAR		||	*peek == LIFE_POWER_UP_CHAR_UPPER)
	    {	      // Call function to handle coordinate character's character
	      ++current, ++peek;
	      handleCoordinateCharacter(buff, current, peek, max, levelRules);
	      // Make sure there's no space between end of coordinate character field and next field (if any.)
	      rubOutSpace(buff, current, peek, max);
	    }
	  else
	    {
	      std::stringstream e {};
	      e<<"Error expected but did not encounter coordinate character. The following is the "<<SHOW_COUNT
	       <<" character's after and including the current character (note output may be less then "<<SHOW_COUNT
	       <<" character's if the EOF character was encountered):\n";
	      printCurrent(current, SHOW_COUNT, e);
	      exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE_CHAR_FIELD);
	    }
	  break;
	}
    }
  else			// Read in the player sprite info.
    {
      initPlayerSprite(maxyx, buff, current, peek, max, levelRules);
      // Make sure there's no space between end of player field and HEADER_END_DENOTATION character.
      rubOutSpace(buff, current, peek, max); 
      ++current, ++peek;	// Move peek past HEADER_END_DENOTATION character.
      if(*current != HEADER_END_DENOTATION)
	{
	  std::stringstream e {};
	  e<<"Error expected but did not see \""<<HEADER_END_DENOTATION<<"\" character to denote the end of the"
	   "header";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
      // Make sure there's no space between HEADER_END_DENOTATION and next field.
      rubOutSpace(buff, current, peek, max);
      inHeader = false;		// We are leaving the header.
    }
}


// FUNCTIONS TO BE DISPATCHED BY SWITCHONCURRENT -END- ------------------------------- -----------------------------


/* Read in the character *current (which should already have been checked for validity), then read in
   the coordinate pair, finally the function should check to see that this coordinate pair is unique in the object
   levelRules.charCoords and if it is use the coordinates as a key to store the character (which is to be interprited
   as a Coordinate character */
void handleCoordinateCharacter(std::string & buff, std::string::const_iterator & current,
			    std::string::const_iterator & peek, std::string::const_iterator & max,
			    rules & levelRules)
{
  const char c {*current};	// Get coordinate char (should be pre checked for validity.)
  // Make sure there is no space between coordinate char and FIELD_START_DENOTATION for the coordinate field.
  rubOutSpace(buff, current, peek, max);
  if(*peek != FIELD_START_DENOTATION)
    {
      std::stringstream e {};
      e<<"Error encountered coordinate character but did not encounter "<<FIELD_START_DENOTATION<<" character.";
      exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
    }
  ++current, ++peek;		// Set current to FIELD_START_DENOTATION character position.
  std::string key {getCoords(buff, current, peek, max)};
  // Make sure there's no space between the end of the coordinate field and the end of the coordinate character field.
  rubOutSpace(buff, current, peek, max);
  if(levelRules.coordChars.find(key) == levelRules.coordChars.end())// Key not found
      levelRules.coordChars[key] = c; // Add our beautiful key, coordinate character pair to the coordChars map :).
  else
    {				// Error duplicate key's in rules.lev file.
      std::stringstream e {};
      e<<"Error duplicate character coordinate's ("<<key<<") encountered in rules.lev file.";
      exit(e.str().c_str(), ERROR_DUPLICATE_COORDINATE);
    }
  // Advance peek past end of coordinate character field and thus past FIELD_END_DENOTATION character.
  ++current, ++peek;
}


void initPlayerSprite(const yx maxyx, std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, std::string::const_iterator & max, rules & levelRules)
{
  std::vector<std::string> sprites {handleStringDenotationAfterFieldDenotation(buff, current, peek, max)};
  rubOutSpace(buff, current, peek, max); // Make sure there's no space between string field and coordinate field.
  current++, peek++;
  
  if(*current == FIELD_START_DENOTATION)
    {
      std::stringstream coordsSS {getCoords(buff, current, peek, max)};
      yx initPos {};		// Initial sprite position.
      coordsSS>>initPos.y;
      {				// We are only using c to skip the COORD_SEPERATION character!
	char c {};
	coordsSS>>c;
      }
      coordsSS>>initPos.x;
      levelRules.gamePlayer = (new player(sprites, maxyx, initPos, 25, DIR_RIGHT));
      ++current, ++peek;
    }
  else
    {
      std::stringstream e {};
      e<<"Error in rules.lev file header, encountered end of string field (denoted by \""<<FIELD_END_DENOTATION
       <<"\") after string. But did not then encounter sprite coordinates (denoted by \""<<FIELD_START_DENOTATION
       <<".\"";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


std::string getCoords(const std::string & buff, std::string::const_iterator & current,
		      std::string::const_iterator & peek, const std::string::const_iterator & max)
{
  std::string ret;  
  ++current, ++peek;		// Advance current to first digit.

  if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
    {				// Error!
      std::stringstream e {};
      e<<"Error parsing coordinate, Y coordinate malformed! Coordinats must be of the form \"Y"<<COORD_SEPERATION
       <<"X\", where Y and X are integer numbers.";
      exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
    }
  
  while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
    {				// Get Y coordinate.
      ret.push_back(*current);
      ++current, ++peek;	// Next character.
    }
  
  if(*current == COORD_SEPERATION)
    {
      ret.push_back(*current);	// COORD_SEPERATION is part of our coords.
      ++current, ++peek;
      
      if(!inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
	{				// Error!
	  std::stringstream e {};
	  e<<"Error parsing coordinate, X coordinate malformed! Coordinats must be of the form \"Y"<<COORD_SEPERATION
	   <<"X\", where Y and X are integer numbers.";
	  exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
	}
      
      while(inSingleDigitRange(*current, ASCII_NUMBER_OFFSET))
	{			// Get X coordinate.
	  ret.push_back(*current);
	  ++current, ++peek;
	}
    }
  else
    {				// Error!
      std::stringstream e {};
      e<<"Error parsing coordinate, missing \""<<COORD_SEPERATION<<"\" character or non integer character before"
	" \""<<COORD_SEPERATION<<"\" character.";
      exit(e.str().c_str(), ERROR_MALFORMED_COORDINATE);
    }

  if(*current != FIELD_END_DENOTATION)
    {	  
      std::stringstream e {};
      e<<"Error in rules.lev file. Encountered character other then \""<<FIELD_END_DENOTATION
       <<"\" or integer character while parsing X coordinate.";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  
  return ret;
}

std::vector<std::string> handleStringDenotationAfterFieldDenotation(std::string & buff,
								    std::string::const_iterator & current,
								    std::string::const_iterator & peek,
								    std::string::const_iterator max)
{
  std::vector<std::string> ret {};
  do
    {	  
      ret.push_back(std::string {getString(++current, ++peek, max)}); // Get the sprite path.
	  
      if(std::isspace(*current)) // Check for spaces after string
	{
	  --current, --peek;
	  rubOutSpace(buff, current, peek, max); // Remove leading space before STRINGS_SEPERATION.
	  ++current, ++peek; // Set current to STRINGS_SEPERATION.
	}
      rubOutSpace(buff, current, peek, max); // Remove trailing space after STRINGS_SEPERATION.

	  
      if(*current != STRINGS_SEPERATION)
	{
	  if(*current == FIELD_END_DENOTATION)
	    break;		// We've read all the little stringy wingy's in :).
	  else
	    {			  // Didn't encounter STRING_SEPERATION or FIELD_END_DENOTATION.
	      std::stringstream e {}; // We encountered was a syntax error.
	      e<<"Error encountered a character other then '"<<STRING_DENOTATION<<"' or '"<<FIELD_END_DENOTATION
	       <<"' when reading string's and between string's";
	      exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
	    }
	}
      else
	{
	  if(*peek == STRING_DENOTATION)
	    continue;
	  else
	    {			// Encounterd STRING_SEPERATION but no STRING_DENOTATION.
	      std::stringstream e {};
	      e<<"Error in field containig string/s found '"<<STRINGS_SEPERATION
	       <<"' that was not followed by '"<<STRING_DENOTATION<<'\'';
	      exit(e.str().c_str(), ERROR_RULES_STRING_FIELDS);
	    }
	}
      // Initalise sprite and add info to sprite map.
    }
  while(*current == STRINGS_SEPERATION); // There can be more then one sprite

  return ret;
}


std::string getString(std::string::const_iterator & current, std::string::const_iterator & peek,
		      const std::string::const_iterator max)
{
  std::string ret {};
  ++current, ++peek;		// Skip inital STRING_DENOTATION character.
  while(true)
    {
      if(peek == max)
	{
	  std::stringstream e {};
	  e<<"Error parsing string, we have reached the end of the file but have not encountred the appropriate"
	    "character's. A rules.lev file must end with the character's \""<<STRING_DENOTATION<<FIELD_END_DENOTATION
	   <<"\", if the last piece of information in the last field in the file is a string.";
	  exit(e.str().c_str(), ERROR_MALFORMED_STRING);
	}
      switch(*current)
	{
	case ESCAPE_CHAR:
	  if(*peek == ESCAPE_CHAR)
	    {			// The ESCAPE_CHAR character has been escaped.
	      ret.push_back(ESCAPE_CHAR);
	      ++current, ++peek;
	    }
	  else
	    if(*peek == STRING_DENOTATION)
	      {			// The STRING_DENOTATION character has been escaped.
		ret.push_back(STRING_DENOTATION);
		++current, ++peek;
	      }
	    else
	      {
		std::stringstream e {};
		e<<"Error parsing string, encountered '"<<ESCAPE_CHAR<<"' but did not encounter '"<<ESCAPE_CHAR<<
		  "' or '"<<STRING_DENOTATION<<"' following it.";
		exit(e.str().c_str(), ERROR_MALFORMED_STRING);
	      }
	  break;
	  
	case STRING_DENOTATION:	// We've reached the end of the string :^).
	  ++current, ++peek;		// Current should be after the STRING_DENOTATION character.
	  return ret;
	
	default:
	  ret.push_back(*current);	  
	}
      ++current, ++peek;
    }
}


bool rubOutSpace(std::string & buff, std::string::const_iterator & current, std::string::const_iterator & peek,
		 std::string::const_iterator & max)
{
  bool foundSp {false};
  while(std::isspace(*peek))
    {
      ++peek;
      foundSp = true;
      if(peek >= max)
	return false;
    }
  if(foundSp)			// Only erase if we found space.
    {
      current = buff.erase(++current, peek); // Why ++current? Erases the sequnece in the range [first, last).
      --current; // Set current to element it was pointing to uppon entering function.
      peek = current, ++peek;	// Reset peak.
      max = buff.end();		// Reset max.
    }
  
  return true;
}


void printCurrent(std::string::const_iterator & current, const int SHOW_COUNT, std::stringstream & e)
{
  for(int iter {SHOW_COUNT} ; *current != NULL_BYTE && iter > 0; --iter, ++current)
    {
      e<<*current;
    }
}
#include "physics.h++"
#include "../sprite/sprite.h++"
#include "levelInteractionHandler.h++"


int rules::physics(const int input)
{
  if(input == sprite::UP || input == sprite::UP_UPPER ||
	       input == sprite::LEFT || input == sprite::LEFT_UPPER ||
	       input == sprite::DOWN || input == sprite::DOWN_UPPER ||
	       input == sprite::RIGHT || input == sprite::RIGHT_UPPER)
              {
		movePlayer(sprite::directions(input));
		gamePlayer->updatePosRel(sprite::directions(input));
	      }
  return playerPosition;
}


void rules::movePlayer(const sprite::directions input)
{
  if(inBounds())
    {
    }
  else
    {
    }
}
//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "player.h++"
#include "../../draw/colorS.h++"
#include "../../draw/draw.h++"

extern setColorMode colorMode;//must be included for the draw function

/*Direction's that ch can take on
  Q---W---E
  |...^...|
  A<--|-->D
  |...v...|                                            
  z---S---X                                                     
*/
void player::updatePosRel(sprite::directions dir)
{//update's position of sprite in a relative fashion with reference to the sprite and update's direction.
  yx p {getNewPos(dir)};
  if(inScreenBounds(p.y, p.x))
    position = p;		// The position is mint :).
  else
    {			// Oh no :'(.
      switch(dir)
	{
	case LEFT_UP:
	case LEFT_UP_UPPER://left up
	  if(direction != DIR_LEFT)//if we are changind the direction
	    {
	      resetCurrentSliceNum(DIR_RIGHT);//we don't wan't to cause a seg fault
	      direction = DIR_LEFT;//change sprite
	    }	   
	  break;
	case UP:
	case UP_UPPER://up
	  if(direction != DIR_UP)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_UP;
	    }
	  break;
	case RIGHT_UP:
	case RIGHT_UP_UPPER://right up
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	  break;
	case LEFT:
	case LEFT_UPPER://left
	  if(direction != DIR_LEFT)
	    {	       
	      resetCurrentSliceNum(DIR_RIGHT);
	      direction = DIR_LEFT;
	    }
	  break;
	case RIGHT:
	case RIGHT_UPPER://right
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	  break;
	case LEFT_DOWN:
	case LEFT_DOWN_UPPER://left down
	  if(direction != DIR_LEFT)
	    {
	      resetCurrentSliceNum(DIR_RIGHT);
	      direction = DIR_LEFT;
	    }
	  break;
	case DOWN:
	case DOWN_UPPER://down
	  if(direction != DIR_DOWN)
	    {
	      resetCurrentSliceNum(DIR_DOWN);
	      direction = DIR_DOWN;
	    }
	  break;
	case RIGHT_DOWN:
	case RIGHT_DOWN_UPPER://right down
	  if(direction != DIR_RIGHT)
	    {
	      resetCurrentSliceNum(DIR_LEFT);
	      direction = DIR_RIGHT;
	    }
	}
    }
}

void player::draw(bool updateSlice)
{
  switch(direction)
    {
    case DIR_LEFT:
      sprite::draw(0, updateSlice);
      break;
    case DIR_RIGHT:
      sprite::draw(1, updateSlice);
      break;
    case DIR_UP:
      sprite::draw(2, updateSlice);
      break;
    case DIR_DOWN:
      sprite::draw(3, updateSlice);
      break;
    default:
      //add exception here!------------------------------------------~~~~~~~~~~~~~~*******************(O)
      break;
    }    
}
#include <stdexcept>
#include <ncurses.h>
#include "slice.h++"
#include "../common.h++"


std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x)
{
  if((offset + x) > (buff.size() / y))
    { // Check for error condition (can't scroll past end of background file).
      exit("error in (slice.cpp)->(std::vector<int> getSlice(std::vector<int> buff, unsigned long offset, int y,"
	   "int x)): (offset + x) > (array / y)!", ERROR_GENERIC_RANGE_ERROR);
    }  
  std::vector<int> slice {};  
  for(int iterY {}; iterY < y; ++iterY)	// Column.
    {
      for(int iterX {}; iterX < x; ++iterX) // Row.
	{ // (offset + screen length) + (column number * level length)	  
	  slice.push_back(buff[(offset + iterX) + (iterY * (buff.size() / y))]);
	}
    }
  return slice;
}
#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <iostream>
#include "sprite.h++"
#include "../initial/load/loadAssets.h++"
#include "../initial/collapse/collapse.h++"// For second phase
#include "../draw/draw.h++"
#include "../common.h++"


sprite::sprite(const yx max, const yx pos, const char spriteFileName [])
  : maxyx(max), position(pos), currentSliceNumber(0),
    startTime(std::chrono::high_resolution_clock::now()),
    currentTime(std::chrono::high_resolution_clock::now())
{
  getSprite(spriteFileName, sD_base);
  getMaxYXOffset();
  //  get
}


void sprite::getSprite(const char spriteFileName [], spriteData & sD)
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
  int max {};
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
	    int iter {SLICE_LINE_ONE};
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
  for(int iter {}; iter < s[y].sliceLine.size(); ++iter)
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


yx sprite::getNewPos(const sprite::directions dir)
{
  yx d {};
  switch(dir)
    {
    case LEFT_UP:
    case LEFT_UP_UPPER:
      d.y = position.y -1;
      d.x = position.x -1;
      break;
    case UP:
    case UP_UPPER:
      d.y = position.y -1;
      d.x = position.x;
      break;
    case RIGHT_UP:
    case RIGHT_UP_UPPER:
      d.y = position.y -1;
      d.x = position.x +1;
      break;
    case LEFT:
    case LEFT_UPPER:
      d.y = position.y;
      d.x = position.x -1;
      break;
    case RIGHT:
    case RIGHT_UPPER:
      d.y = position.y;
      d.x = position.x +1;
      break;
    case LEFT_DOWN:
    case LEFT_DOWN_UPPER:
      d.y = position.y +1;
      d.x = position.x -1;
      break;
    case DOWN:
    case DOWN_UPPER:
      d.y = position.y +1;
      d.x = position.x;
      break;
    case RIGHT_DOWN:
    case RIGHT_DOWN_UPPER:
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
  //  return
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
  for(int sliceLine{}; sliceLine < spriteS[spriteNum].spriteSlices[currentSliceNumber].slice.size(); ++sliceLine)
    {      
      for(int sliceLineIter{};
	  sliceLineIter < spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine.size();
	  ++sliceLineIter)
	{ // Move curser to the right position.
	  setCursor(position.y + sliceLine, position.x +
		    spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].offset + sliceLineIter, maxyx);
	  // Get the character.
	  int ch {spriteS[spriteNum].spriteSlices[currentSliceNumber].slice[sliceLine].sliceLine[sliceLineIter]};
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
