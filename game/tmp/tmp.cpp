#include "include/backgroundSprite.hpp"

#include "include/checkBoundsOfBounds.hpp"


/* should return an integer that represents the difference in "size" between the background file and
the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background)
{  
  return boarder.size() - background.size();
}
#include <stdexcept>
#include <sstream>
#include <ncurses.h>
#include "include/utils.hpp"
#include "include/collapse.hpp"
#include "include/colorS.hpp"


constexpr char NAME_OF_THIS_FILE [] = "collapse.cpp";


// Copies rawBuff to buff with all '\n' characters removed.
void firstPassOfCollapse(const std::string & rawBuff,
		       std::string & buff);
/* Copies buff to preRet with all "\nc/" (non colour) escape sequences
processed. */
void secondPassOfCollapse(std::string & buff, std::vector<int> & preRet);
void thirdPassOfCollapse(std::vector<int> & preRet, std::vector<int> & ret);


chMap::chMap()
{
  int chVal {ASCII_CH_MAX + 1};
  /* insert all the spcial character types into the map along with their key
  values */
  for(std::string strIter: characters)
    {				/* enter element into map with a key of strIter
				   and a value of offset. */
      strMap[strIter]=chVal;
      /* increment chVal (each character must have a uniqe value not equal to
	 any ascii value). */
      ++chVal;
    }
}


int chMap::find(const std::string str)
{//map.at() throws and out of range exception if no matching key is found
  return strMap.at(str);//returns value that corresponds to the key str
}


void collapse(const std::string & rawBuff,
	      std::vector<int> & ret)
{
  ret.clear();			// Make sure that ret is empty.
  std::string buff {};

  firstPassOfCollapse(rawBuff, buff);  
  std::vector<int> preRet {};
  secondPassOfCollapse(buff, preRet);
  thirdPassOfCollapse(preRet, ret);
}


void firstPassOfCollapse(const std::string & rawBuff,
		       std::string & buff)
{
  for(size_t iter{}; iter < rawBuff.size(); ++iter)
    {
      if(rawBuff[iter] != '\n')
	buff.push_back(rawBuff[iter]);
    }
}


void secondPassOfCollapse(std::string & buff, std::vector<int> & preRet)
{ // Collaps "\nc/" ecsape sequences (special characters.)
  // escSeq holds the string used to get the color code from escCharNums.
  std::string escSeq {};
  /* Declared as static so that the constructor will only be called once. This
     is an optimization since the constructor does a lot of work that is
     redundent if done more then once. */
  static chMap escCharNums;
  constexpr size_t ncEscLen {3}; // Length of the escape sequence.
  
  for(size_t iter{}; iter < buff.size(); ++iter)
    { // If we have found the start of a nonColour escape sequence.
      if(buff[iter] == '\\' && buff[1 + iter] == 'n' && buff[2 + iter] == 'c')
	{ // Skip first three characters of escape sequencse ("\nc").
	  iter += ncEscLen;
	  if(iter > buff.size()) // Check for errors in formatting.
	    { // If iter is outside of range.
	      std::string eMsg {"in collaps.cpp ->collaps(const std::string & "
		"buffWithSpace, std::vector<int> ret). First pass. iter is > "
		"buff.size. Iter ="};
	      eMsg += iter;
	      eMsg += buff.size();
	      eMsg += '\n';
	      throw std::logic_error(eMsg);
	    }
	  for( ; buff[iter] != '/'; ++iter)
	    { // The next two if statments check for errors.
	      if(iter > buff.size())
		{
		  std::string eMsg {"in collaps.cpp->collaps(const std::string "
		    "& buffWithSpace, std::vector<int ret). iter ="};
		  eMsg += iter;       eMsg += "\n";
		  throw std::logic_error(eMsg);
		}
	      escSeq += buff[iter]; // Get nonColour escape sequence.
	    }
	  /* Set next element in preRet to int value that corresponds to escSeq
	     nonColour escape sequence. */
	  preRet.push_back(escCharNums.find(escSeq));
	  /* Clear escSeq so that it is empty next time we find a nonColour
	     escape sequence and use += on escSeq. */
	  escSeq.clear();
	}
      else
	{ /* If we have found a normal character or a transparent space (no
	     character is displayed) character. */
	  if(buff[iter] == '\\' && buff[1 + iter] == 't' &&
	     buff[2 + iter] == 'r' && buff[3 + iter] == 'a' &&
	     buff[4 + iter] == 'n' && buff[5 + iter] == 's' &&
	     buff[6 + iter] == 'S' && buff[7 + iter] == 'P' &&
	     buff[8 + iter] == '/')
	    {
	      preRet.push_back(TRANS_SP);
	      iter += 8;
	    }
	  else
	    preRet.push_back(buff[iter]);
	}
    }
}


void thirdPassOfCollapse(std::vector<int> & preRet, std::vector<int> & ret)
{
  /* Declared as static so that the constructor will only be called once. This
     is an optimization since the constructor does a lot of work that is
     redundent if done more then once. */
  static colourMap cuMap;
  // EscSeq holds the string used to get the color code from cuMap.
  std::string escSeq {};
  for(size_t iter{}; iter < preRet.size(); ++iter)
    {
      if(char(preRet[iter]) == '\\' && char(preRet[1 + iter]) == 'c' && char(preRet[2 + iter]) == 'u')
	{ // We are in the start of a color escape sequence.
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
	      escSeq += preRet[iter];
	    } // We have the color information (in escSeq.)
	  iter++;
	  bool inColor {true};
	  for(size_t iterColor{iter}; inColor; ++iterColor)
	    {// Now apply color information to characters.
	      if((iterColor - 3) >= preRet.size())
		{
		  std::stringstream e {};
		  e<<"Error (in \""<<NAME_OF_THIS_FILE<<"\"): iterColor = "
		   <<iterColor<<", preRet.size() = "<<preRet.size();
		  exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
		}
	      if(char(preRet[iterColor]) == '\\' && char(preRet[iterColor + 1]) == 'c' && char(preRet[iterColor + 2]) == 'x' && char(preRet[iterColor + 3]) == '/')
		{ // Have we reached the closing escape sequences?
		  inColor = false;
		  iter = iterColor + 3;
		}
	      else
		{ // Apply color information to character.
		  // If the character is not a "transparent space" character!
		  if(preRet[iterColor] != 10176)
		    { /* Non "transparent space" characters have color
			 information applied here. */
		      ret.push_back(preRet[iterColor] + cuMap.getRange(escSeq));
		    }
		  else
		    { /* Transparent space! (this character is a special case
			 and so color information should not be applied to
			 it). */
		      ret.push_back(preRet[iterColor]);
		    }
		}
	    }
	  inColor = true;
	  escSeq.clear();
	}
      else
	{ // Character with default color.
	  ret.push_back(preRet[iter]);
	}
    }
}
#include <ncurses.h>
#include "include/colorS.hpp"

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
#include "include/utils.hpp"
#include "include/slice.hpp"
#include "include/draw.hpp"
#include "include/colorS.hpp"


const std::string FILE_NAME {"draw.cpp"};


/* Class handels the setting of color when printing characters (used in
   draw.cpp) argument is the default color pair. */
//If argument to object constructor is changed it must also be changed in main.cpp.    
setColorMode colorMode{56};


void draw(int * unprocessedDrawBuffer, const std::vector<int> & buff,
	  player * playerSprite, std::vector<bgSprite *> & bgSprites,
	  const yx maxyx, const unsigned long offset)
{
  drawBackground(unprocessedDrawBuffer, buff, maxyx, offset);

  /* NOTE THAT A FLAG THAT IS SETTABLE FROM A RULES.LEV FILE SHOULD BE ADDED TO
     THE SPRITE CLASS THAT SPECIFIES IF A SPRITE SHOULD BE DISPLAYED IN FRONT
     OF THE PLAY (AS OPPOSED TO BEHIND.) THEN THIS FLAG CAN BE CHECK FOR IN THE
     BELOW FOR LOOP. IT'S INDEX CAN BE SAVED IF IT IS TO BE DISPLAYED IN FRONT
     OF THE PLAYER SPRITE AND THEN INSTEAD OF BEING DISPLAYED IN THE BELOW FOR
     LOOP IT CAN BE DISPLAYED AFTER THE PLAYER IS. THEN WE CAN HAVE THE PLAYER
     APPEAR TO SPAWN FROM AN EGG ORB SPACESHIP THING THAT OPENS AND CLOSES AND
     JUST SO HAPPENS TO OPEN AS THE PLAYER SPAWNS AND DROPS TO THE GROUND. WE
     COULD ALSO ADD SOME MECHANISM WHERE BY THE ANIMATION OF THE SPRITE WOULD
     ONLY BE PLAYED ONCE. THIS COULD BE EASILY DONE BY ADDING ANOTHER OPTION TO
     RULES.LEV FILES. THIS FOREGROUND OPTION WOULD ALSO ALLOW FOR THE APPEARANCE
     OF TREES AND COLUMNS BEING IN FRONT OF THE PLAYER.

     NOTE THAT WE ALSO SHOULD CHANGE THE WAY THAT THIS FUNCTION AND THE
     FUNCTIONS IT CALLS WORK SO THAT THEY PRINT INTO A BUFFER THAT IS THE SAME
     SIZE AS THE WINDOW AND THEN THIS BUFFER IS PRINTED. ALSO WE SHOULD
     EXPERIMENT WITH PRINTING NON ACS CHARACTERS THAT ARE CONTIGUOUS AND OF THE
     SAME COLOUR USING ONE FUNCTION CALL (AS AS FAR AS WE CAN TELL THE ACS
     CHARACTERS REQUIRE A DIFFERENT FUNCTION CALL. HOPEFULLY THE ABOVE WILL
     IMPROVE THE POOR PERFORMANCE A LITTLE.

     WE SHOULD ALSO ADD AN OPTION TO RULES.LEV FILES SO THAT THE UPDATESLICE
     VARIABLE IN DRAW() FOR SPRITES CAN BE CONTROLS. THIS WAY WE CAN HAVE SAY A
     TEMPLE WITH COLUMNS THAT JUST HAS ONE SLICE (I THINK THAT'S WHAT I CALLED
     IT LOL) AND WE CAN BYPASS ALL OF THE CODE THAT SWITCH THE SLICE (EVEN
     THOUGH IT'S PROBABLY NOT MUCH OVER HEAD. THIS OPTION WOULD BE OPTIONAL AND
     OFF BY DEFAULT. WE COULD JUST HAVE THE CYCLE TIME IGNORED, THIS WAY WE
     WOULDN'T HAVE TO ADD ANY EXTRA COMPLEXITY TO SPRITE FILES (THE CODE FOR
     WHICH DEFINITELY NEEDS SOME ATTENTION HAHA.) */
  std::vector<bgSprite *> drawInForground;

  for(auto bgS: bgSprites)
    {
      if(!bgS->displayInForground)
	{
	  bgS->draw(unprocessedDrawBuffer, true, offset);
	}
      else
	{
	  /* We think this will probably be faster than searching the whole list
	     again if the list is of a large size (and taking into consideration
	     that most sprites will probably be behind the player). */
	  drawInForground.push_back(bgS);
	}
    }  
  playerSprite->draw(unprocessedDrawBuffer, true);
  for(auto bgSF: drawInForground)
    {
      bgSF->draw(unprocessedDrawBuffer, true, offset);
    }
      
  drawDrawBuffer(unprocessedDrawBuffer, maxyx);
  refresh();
}


void drawBackground(int * unprocessedDrawBuffer, const std::vector<int> & buff,
		    const yx maxyx, const unsigned long offset)
{
  std::vector<int> slice {getSlice(buff, offset, maxyx.y, maxyx.x)};

  for(auto iter: slice)
    {
      *unprocessedDrawBuffer=iter;
      ++unprocessedDrawBuffer;
    }
}


void drawDrawBuffer(int * unprocessedDrawBuffer, const yx maxyx)
{
  mvprintw(0, 0, "");

  for(int iter {}; iter < (maxyx.y * maxyx.x);
      ++iter)
    {
      std::string contiguousColorChars;
      int acsCode;
      bool foundAcsCode;

      setColor(unprocessedDrawBuffer[iter]);
      foundAcsCode = getContiguouslyColordString
	(unprocessedDrawBuffer, iter, maxyx, contiguousColorChars, acsCode);

      if(contiguousColorChars.size() != 0)
	{
	  // Print ASCII chars.
	  printw(contiguousColorChars.c_str());
	}
      if(foundAcsCode)
	{
	  // Print ACS char if found.
	  printAcs(acsCode, false);
	}

      contiguousColorChars.clear();
    }
}


void setColor(const int charCodeWithColor)
{
      if(inColorRange(charCodeWithColor))
	{
	  int colorCode = getColor(charCodeWithColor);
	  colorMode.setColor(colorCode);
	}
      else
	{
	  colorMode.clearColor();
	}
}


inline bool getContiguouslyColordString
(const int * const unprocessedDrawBuffer, int & buffIndex, const yx maxyx,
 std::string & contiguousColorChars, int & acsCode)
{
  const int startColorCode = getColor(unprocessedDrawBuffer[buffIndex]);
  const int startIndex {buffIndex};


  for( ; buffIndex < (maxyx.y * maxyx.x) &&
	 getColor(unprocessedDrawBuffer[buffIndex]) == startColorCode; buffIndex++)
    {
      int ch;
      ch = unprocessedDrawBuffer[buffIndex];

      // Remove colour information (if any) from ch.
      if(inColorRange(ch))
	{
	  ch -= (getColor(ch) * (MONO_CH_MAX + 1));
	}

      if(ch <= ASCII_CH_MAX)
	{
	  // We have an ASCII char.
	  contiguousColorChars.push_back((char)ch);
	}
      else if(ch <= MONO_CH_MAX)
	{
	  // We have an ACS char. 
	  acsCode = ch;
	  return true;
	}
      else
	{
	  exit(concat("Error: encountered default colour character (",
		      (int)ch, ") that is greater than ", MONO_CH_MAX,".!"),
	       ERROR_CHARACTER_RANGE);
	}
    }

  /* We must decrement buffIndex here because it is incremented at the end of
     the loop but the loop isn't run again and thus the next character isn't
     added. */
  buffIndex--;
  return false;
}


void printAcs(const int acsCode, const bool inColor)
{
  switch(acsCode)
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
    default:
      exit(concat("Error: encountered ", inColor ? "colour": "",
		  " ACS character code (", (int)acsCode,
		  ") that is greater than 158",
		  inColor ? "after having colour information removed.": "."),
	   ERROR_CHARACTER_RANGE);
      break;
    }
}


bool inColorRange(const int ch)
{
  if(ch > MONO_CH_MAX)
    return true;
  return false;
}
int getColor(const int ch)
{
  // Ch modulo MONO_CH_MAX equals the color code.
  int color {ch / (MONO_CH_MAX +1)};		
  if(color <= COLOR_CH_MAX)
    return color;

  exit(concat("Error (in ", FILE_NAME, "): encountered colour (", color, ") "
	      "code that is out of range.\n"),
       ERROR_COLOR_CODE_RANGE);
}
#include <string>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "include/utils.hpp"
#include "include/drawExit.hpp"
#include "include/draw.hpp"


void drawExit(const yx maxyx)
{
  // const std::string swanSong {"~~*-F-A-R-E-W-E-L-L-*~~"};
  // int padding {1};
  // bool direction {true};	// True indicates a "growing" direction (i.e. depletion zone around swanSong.)
  // int sleepTime {1};	// :)
  // constexpr int sleepTimeStep {1};	// How much to increment or decrement sleep time by.
  // constexpr int frames {1};
  // int ch {};  
  // srand(time(NULL));
  
  // for(size_t iter {}; iter < frames; ++iter)
  //   {
  //     for(size_t xIter {}; xIter < maxyx.x; ++xIter)
  // 	{
  // 	  for(size_t yIter {}; yIter < maxyx.y; ++yIter)
  // 	    {
  // 	      if(swanSong.size() != 0) // Check that message has non zero size.
  // 		{		  
  // 		  if((xIter > ((maxyx.x /2) - (padding + swanSong.size() /2))) &&
  // 		     (xIter < ((maxyx.x /2) + (padding + swanSong.size() /2))) &&
  // 		     (yIter > ((maxyx.y /2) - padding)) &&
  // 		     (yIter < ((maxyx.y /2) + padding)))
  // 		    {
  // 		      if(xIter == ((maxyx.x /2) - (swanSong.size() /2)) &&
  // 			 yIter == (maxyx.y /2))
  // 			 mvprintw(yIter, xIter, swanSong.c_str());
  // 		    }
  // 		  else
  // 		    {
  // 		      setCursor(yIter, xIter, maxyx);
  // 		      ch = (rand() % MONO_CH_MAX) * (rand() % COLOR_CH_MAX);
  // 		      drawCh(ch);
  // 		    }
									       
  // 		}
  // 	    }
  // 	}
  //     refresh();
  //     sleep(sleepTime);
  //     if(iter == (frames /2))
  // 	direction = false;	/* We've printed half the frames so reverse the direction of growth of the 
  // 				   depletion zone around swanSong and start decresing sleeptime. :) */
  //     if(direction)
  // 	{
  // 	  ++padding;
  // 	  if(iter < frames /4)
  // 	    sleepTime += sleepTimeStep -2;
  // 	  else
  // 	    sleepTime += sleepTimeStep;
  // 	}
  //     else
  // 	{
  // 	  --padding;
  // 	  if(iter > (frames - (frames /4)))
  // 	    sleepTime -= sleepTimeStep -2;
  // 	  else
  // 	    sleepTime -= sleepTimeStep;
  // 	}
  //   }
}
#include "include/initCurses.hpp"


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
#include "include/levelRules.hpp"
#include "include/player.hpp"
#include <string>


void rules::resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime)
{
  std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::high_resolution_clock::now()};
  if((duration_cast<std::chrono::duration<double>>
      (currentTime - oldTime)).count() >= rules::second)
    {
      oldTime = currentTime;
    }
}

//test level agains sprite's
char rules::intersection(const std::string & boundsInteraction,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any character other then space in
     boundsInteration return that charater */
  return 'n';
}

//test player sprite against sprite's
char rules::intersection(const std::vector<int> playerSpChoords,
			 const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return characte code or something */
  return 'n';
}

//test level agains sprite's
char rules::nearPass(const std::string & boundsInteraction,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords is next to any character other then space in
     boundsInteration return that charater */
  return 'n';
}

//test player sprite against sprite's
char rules::nearPass(const std::vector<int> playerSpChoords,
		     const std::vector<int> spChoords)
{
  /* if any value in spChoords matches any value in playerSpChoords
     return a character code that corispondest to the side that was matched
     relative to spChoords for example 't' for top. (this might be used to
     detect that the player has "touched" the top of the sprite and if it is an
     enemy sprite then appropriate action can be taken. */
  return 'n';
}

// =================== TMP TMP TMP ==========================
#include <curses.h>
#include <iostream>
#include <fstream>
bool encountered {false};
// =================== TMP TMP TMP ==========================

void rules::movePlayer(sprite::directions input,
		       int & position, const yx maxyx,
		       const size_t backgroundLength)
{ /* Move the player as long as they will stay within
     PLAYER_MOVMENT_INNER_MARGIN of the left and right borders. If the
     player is PLAYER_MOVMENT_INNER_MARGIN away from either the left or
     right boarder and tries to move closer to the boarder they are closest
     too then the background should be moved instead of the player in the
     appropriate direction (If there is background to spare.) If we are at
     either end of the level then the player cannot move further of
     course. */
  const int currDir {gamePlayer->getDirection()};
  
  if(input == sprite::DIR_UP // && !gamePlayer->isJumpNumMaxedOut()
     )
    {
      // Start jump.
      gamePlayer->startJumping(position, maxyx, coordRules);
      // We wan't to keep moving in the direction we were moving in before.
      input = (sprite::directions)currDir;
    }
  else
    {
      /* We are not jumping or we are past the start of a jump.
	 If we can move down. */
      gamePlayer->handleJumpingAndFalling(position, maxyx, coordRules);
    }

  // Handle contact with boarder characters.
  if((currDir == sprite::DIR_DOWN && input == sprite::DIR_NONE) ||
     input == sprite::DIR_DOWN)
    {
      input = handleGroundCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_RIGHT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_RIGHT)
    {
      input = handleRightCollision(position, maxyx.y);
    }
  else if((currDir == sprite::DIR_LEFT && input == sprite::DIR_NONE) ||
	  input == sprite::DIR_LEFT)
    {
      input = handleLeftCollision(position, maxyx.y);
    }

  handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
    (input, position, maxyx, backgroundLength);
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength)
{
  const yx peekPos {gamePlayer->peekAtPos(newDir)};
  if(gamePlayer->inLevelY(peekPos.y, maxyx.y) &&
     gamePlayer->inLevelX(peekPos.x + position, backgroundLength))
    {
      // We won't be outside of the level if we move.
      handleFinalPlayerMovementAndWindowAndMarginInteractions
	(newDir, position, maxyx, backgroundLength, peekPos);
    }
  else
    {
      gamePlayer->updateDirection(sprite::DIR_NONE);
    }
}


void rules::handleFinalPlayerMovementAndWindowAndMarginInteractions
(const sprite::directions newDir, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
    /* Make any final movement, check for window margin contact and take
     appropriate action if such contact is made. */
  if(((newDir == sprite::DIR_LEFT || newDir == sprite::DIR_RIGHT) &&
      gamePlayer->notInWindowInnerMarginX(peekPos.x,
					  PLAYER_MOVEMENT_INNER_MARGIN.x)) ||
     newDir == sprite::DIR_NONE)	// Only need to check for DIR_NONE here.
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else if((newDir == sprite::DIR_DOWN || newDir == sprite::DIR_UP)
	  && gamePlayer->notInWindowInnerMarginY(peekPos.y,
						 PLAYER_MOVEMENT_INNER_MARGIN.y))
    {
      // We're not going to go into the margin.
      gamePlayer->updatePosRel(newDir);
    }
  else
    {
      movePlayerWhenInteractingWithInnerMargin(newDir, position, maxyx,
					       backgroundLength, peekPos);
    }
}


void rules::movePlayerWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos)
{
  /* We use this variable in the call's to inWindowInnerMargin() when peekPos
     is out of the x boarder range. */
  constexpr int REACHED_INNER_MARGIN_X {0};
  switch(input)
    {
    case sprite::DIR_NONE:
      break;
    case sprite::DIR_UP:
      gamePlayer->updatePosRel(input);
      break;
    case sprite::DIR_RIGHT:
      movePlayerRightWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    case sprite::DIR_DOWN:
      gamePlayer->updatePosRel(input);
      break;
    case sprite::DIR_LEFT:
      movePlayerLeftWhenInteractingWithInnerMargin
	(input, position, maxyx, backgroundLength, peekPos,
	 REACHED_INNER_MARGIN_X);
      break;
    }
}


void rules::movePlayerRightWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->leftOfWindowInnerRightMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x, maxyx))
    { // We are to the left of the inner right margin.
      gamePlayer->updatePosRel(input);
    }
  else if((position + maxyx.x) < backgroundLength)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position++;	// Move background.
    }
  else if((position + maxyx.x) == backgroundLength)
    { /* No background left, so move the player to the right
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


void rules::movePlayerLeftWhenInteractingWithInnerMargin
(const sprite::directions input, int & position, const yx maxyx,
 const size_t backgroundLength, const yx peekPos,
 const int REACHED_INNER_MARGIN_X)
{
  if(gamePlayer->rightOfWindowInnerLeftMargin
     (peekPos.x, PLAYER_MOVEMENT_INNER_MARGIN.x))
    { // We are to the righ of the inner left margin.
      gamePlayer->updatePosRel(input);
    }
  else if(position > 0)
    { // There is still background left to spare.
      gamePlayer->updateDirection(input);
      position--;	// Move background.
    }
  else if(position == 0)
    { /* No background left, so move the player to the left
	 edge of the background. */
      gamePlayer->updatePosRel(input);
    }
}


// int rules::getClosestBoarderCharAbove(const int position,
// 				      const bool directContact)
// {
//   int minBoarderCharDistanceRet {0};
//   int yPos {};
    
//   std::vector<std::string> spriteTopCoords
//     {gamePlayer->getTopXAbsRangeAsStrsForOneOffContact(position)};

//   // All coordinates from spriteTopCoords and above.
//   std::vector<std::string> spriteTopCoordsColumn {};

//   for(std::string strCoord: spriteTopCoords)
//     {
//       std::stringstream coord {strCoord};
//       int spCoordY, spCoordX;
//       char eat;
//       coord>>spCoordY;
//       coord>>eat;
//       coord>>spCoordX;

//       yPos = spCoordY;

//       for(int y {spCoordY}; y >= 0; y--)
// 	{
// 	  std::stringstream newCoord {};
// 	  newCoord<<y;
// 	  newCoord<<',';
// 	  newCoord<<spCoordX;
// 	  spriteTopCoordsColumn.push_back(newCoord.str());
// 	}
//     }


//   bool foundBoarderChar {false};
//   for(std::string coord: spriteTopCoordsColumn)
//     {
//       if(coordChars.find(coord) != coordChars.end())
// 	{
// 	  foundBoarderChar = true;
// 	  std::stringstream boarderCoord {coord};
// 	  int boarderY {};
// 	  boarderCoord>>boarderY;
	  
// 	  minBoarderCharDistanceRet =
// 	    boarderY > minBoarderCharDistanceRet ? (yPos - boarderY):
// 	    (yPos - minBoarderCharDistanceRet);
// 	}
//       mvprintw(0, 0, "%d", minBoarderCharDistanceRet);
//     }

//   // for(std::string strCoord: spriteTopCoordsColumn)
//   //   {
//   //     std::stringstream coord {strCoord};
//   //     int y, x;
//   //     char eat;

//   //     coord>>y;
//   //     coord>>eat;
//   //     coord>>x;

//   //     if((x - position) < 250)
//   // 	{
//   // 	  mvprintw(y, (x - position), "@");
//   // 	  refresh();
//   // 	}
//   //     coord.clear();
//   //   }

//   return foundBoarderChar ? minBoarderCharDistanceRet: -1;
// }


/* Checks for contact with boarder characters when moving down. Returns the
   direction that the player should be moving in. */
sprite::directions rules::handleGroundCollision(const int position,
						const int backgroundHeight)
{
  sprite::directions retDir {sprite::DIR_DOWN};
  for(yx coord:
	gamePlayer->getBottomXAbsRangeAsStrsForOneOffContact(position))
    {
      char rule {};
      if(getCoordRule(coord, coordRules, backgroundHeight, rule) &&
	 (rule == boarderRuleChars::BOARDER_CHAR ||
	  rule == boarderRuleChars::PLATFORM_CHAR))
	{
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  return retDir;
}


/* Checks for contact with boarder characters when moving right. Moves the
   player up one character if a "step" is encountered (as long as the player
   will not go out of bounds.) Returns the direction the player should move in.
*/
sprite::directions rules::handleRightCollision(const int position,
					       const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_RIGHT};
  const std::vector<yx> playerCoords
    {gamePlayer->getRightYAbsRangeAsStrsForOneOffContact(position)};
  const yx bottomRightPlayerCoord
    {playerCoords[playerCoords.size() -1]};
  bool stoppingContact {false};

  char rule {};
  for(yx playerCoord: playerCoords)
    {
      // If there is near contact and it's not with the bottom right coord.
      if(playerCoord != bottomRightPlayerCoord &&
	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
	 rule == BOARDER_CHAR)
	{
	  stoppingContact = true;
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }
  
  if(!stoppingContact &&
     getCoordRule(bottomRightPlayerCoord, coordRules, backgroundHeight, rule) &&
     (rule == BOARDER_CHAR || rule == PLATFORM_CHAR))
    {
      if(gamePlayer->getPos().y > 0)
	{
	  /* If we've hit a "step" (as in the things that constitute staircases)
	     and we are not at the minimum (top of window) y position, then
	     "step up" :). */
	  gamePlayer->updatePosRel(sprite::DIR_UP);
	}
      else
	{
	  // We've hit the top of the window.
	  retDir = sprite::DIR_NONE;
	}
    }
  
  return retDir;
}


sprite::directions rules::handleLeftCollision(const int position,
					      const int backgroundHeight)
{
  using namespace boarderRuleChars;
  sprite::directions retDir {sprite::DIR_LEFT};
  const std::vector<yx> playerCoords
    {gamePlayer->getLeftYAbsRangeAsStrsForOneOffContact(position)};
  const yx bottomLeftPlayerCoord
    {playerCoords[playerCoords.size() -1]};
  bool stoppingContact {false};

  char rule {};
  for(yx playerCoord: playerCoords)
    {
      // If there is near contact and it's not with the bottom right coord.
      if(playerCoord != bottomLeftPlayerCoord &&
	 getCoordRule(playerCoord, coordRules, backgroundHeight, rule) &&
	 rule == BOARDER_CHAR)
	{
	  stoppingContact = true;
	  retDir = sprite::DIR_NONE;
	  break;
	}
    }

  if(!stoppingContact &&
     getCoordRule(bottomLeftPlayerCoord, coordRules, backgroundHeight, rule) &&
     (rule == BOARDER_CHAR ||
      rule == PLATFORM_CHAR))
    {
      if(gamePlayer->getPos().y > 0)
	{
	  /* If we've hit a "step" and we are not at the minimum (top of window)
	     y position, then "step up" :) */
	  gamePlayer->updatePosRel(sprite::DIR_UP);
	}
      else
	{
	  // We've hit the top of the window.
	  retDir = sprite::DIR_NONE;
	}
    }

  return retDir;
}


#ifdef DEBUG
#include <sstream>

void rules::printRuleChars(const int position, const int maxY, const int maxX)
{
  for(int y {}; y < maxY; ++y)
    {
      for(int x {}; x < (coordRules.size() / maxY); ++x)
	{
	  char coordRule;
	  getCoordRule(y, x, coordRules, maxY, coordRule);
	  if(coordRule != ' ' && (x - position) < maxX)
	    {
	      mvprintw(y, (x - position), (std::string {coordRule}).c_str());
	    }
	}
    }
  refresh();
}
#endif


void rules::physics
(const player::directionChars input, int & position, const yx maxyx,
 const size_t backgroundLength,
 std::__1::chrono::steady_clock::time_point & secStartTime)
{
#ifdef DEBUG
  printRuleChars(position, maxyx.y, maxyx.x);
#endif
  
  movePlayer(player::convertDirectionCharsToDirections(input), position, maxyx,
	     backgroundLength);
  sleep(engineSleepTime);
  resetOldTime(secStartTime);
}
#include <iostream>
#include <sstream>
#include <cstring>
#include <ncurses.h>
#include <string>
#include "include/utils.hpp"
#include "include/loadAssets.hpp"
#include "include/collapse.hpp"
//#include "include/backgroundSprite.hpp"


void loadAssets
(const yx maxyx, const char bgFileName [], std::vector<int> & background,
 const char rulesFileName [], rules & levelRules)
{
  // Initialise background array.
  loadAndParseBackgroundFile(maxyx, bgFileName, background);
  /* Initialise player and non-player sprites (rules file) and initialise
     levelRules array. */
  loadAndParseLevelRulesFile(maxyx, bgFileName, rulesFileName, levelRules,
			     background.size());
}


void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				std::vector<int> & background)
{
  std::string levelBackGround {};
  if(!loadFileIntoString(bgFileName, levelBackGround))
    {
      std::stringstream err {};
      err<<"Error: unable to open \""<<bgFileName<<"\".";
      exit(err.str(), ERROR_OPENING_FILE);
    }
  collapse(levelBackGround, background); //collapse nonColor escape
  //sequences.
  if(background.size() < (maxyx.x * maxyx.y) || (background.size() % maxyx.y) != 0)
    {
      std::stringstream err {};
      err<<"Error: wrong number of characters in background after collapsing "
	"escaped characters. "
	"background size (after processing) < (window length * window height) "
	"or (background size (after processing) % window height) != 0."
	" Where background size = "<<background.size()<<", window length = "
	 <<maxyx.x<<" and window height = "<<maxyx.y<<".";
      exit(err.str(), ERROR_BACKGROUND);
    }
}


void loadAndParseLevelRulesFile(const yx maxyx, const char bgFileName [],
				const char rulesFileName[],
                                rules &levelRules, const size_t bgSize)
{
  std::string rawRules {};
  if(!loadFileIntoString(rulesFileName, rawRules))
    {
      std::stringstream e {};
      e<<"Error: unable to open \""<<rulesFileName<<"\".";
      exit(e.str(), ERROR_OPENING_FILE);
    }

  std::string::const_iterator buffPos {rawRules.begin()};
  parseRulesHeader(maxyx, rulesFileName, levelRules, bgSize, rawRules, buffPos);
  parseRulesMain(maxyx, bgFileName, rulesFileName, levelRules, bgSize, rawRules, buffPos);
}


void parseRulesHeader(const yx maxyx, const char rulesFileName[],
			  rules & levelRules, const size_t bgSize,
		      const std::string & rawRules,
		      std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;

  readStartOfHeader
    (rawRules, buffPos,
     concat("reading start of rules.lev header file \"", rulesFileName, "\""));

  /* Setup keyword actions associations for header section. If there is a
     section that is needed but we don't want to be specifiable in rules.lev
     files then it's entry here should have a default value and the action
     function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> headerKeywordActions
    {keywordAction::headerKeywordAction
     {PLAYER_HEADER_SECTION_SPECIFIER, nullptr, initPlayer, false},
     keywordAction::headerKeywordAction
     {BG_SPRITE_HEADER_SECTION_SPECIFIER, nullptr, initBgSprites, true}};

  std::vector<std::string> targets {};
  std::string targetFound {};
  for(auto keywordAction: headerKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  // Parse player sub sections.
  while(true)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
      
      if(targetFound == "")
	{
	  break;
	}
      
      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)headerKeywordActions.size();
	  foundIter++)
	{
	   if(targetFound == headerKeywordActions[foundIter].keyword)
	     {
	       /* Note here that sections that should only appear once will be
		  marked as found if encountered, however sections that can
		  appear  multiple times will not be marked as found if
		  encountered. */
	       if(headerKeywordActions[foundIter].found)
		 {
		   std::stringstream e {};
		   e<<"Error: reading rules.lev header file \""<<rulesFileName
		    <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		     "reading header section, however this keyword is only "
		     "expected once in this section.\n";
		   exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		 }
	       switch(headerSectionKeywordToId.at(targetFound).order)
		 {
		 case 0:
		   headerKeywordActions[foundIter].found = true;
		   headerKeywordActions[foundIter].headerAction
		     (maxyx, rulesFileName, levelRules, bgSize, rawRules,
		      buffPos);
		   break;
		 case 1:
		   /* We don't set found here because this keyword should have
		      headerKeywordAction.foundMultipleOptional set to true. */
		   headerKeywordActions[foundIter].headerAction
		     (maxyx, rulesFileName, levelRules, bgSize, rawRules,
		      buffPos);
		   break;
		 }

	       /*
		 NOTE THAT WE LEAVE THIS CODE HERE IN CASE IT IS EVER DESIRED TO
		 HAVE A KEYWORD THAT IS FORBIDDEN BUT ASSOCIATED WITH A DEFAULT
		 VALUE IN THIS SECTION. 
		 if(false)
		 {
		 ENCOUNTERED_FORBIDDEN_KEYWORD:
		   std::stringstream e {};
		   e<<"Error: reading rules.lev header file \""<<rulesFileName
		    <<"\". Encountered keyword \""<<targetFound<<"\" when "
		     "reading header section, however this keyword is "
		     "forbidden.\n";
		   exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		   }*/
	     }
	}
    }

  // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: headerKeywordActions)
    {
      if(!(keywordAction.found || keywordAction.foundMultipleOptional))
	{
	  // See if there is a default value for not found.
          /*
	    NOTE THAT HERE SIMILAR TO THE ABOVE WE LEAVE THIS CODE IN CASE IT IS
	    EVER DESIRED TO HAVE A  KEYWORD THAT HAS A DEFAULT VALUE FOR THIS
	    SECTION.  
	    checkForDefaultHeaderValues
	    (headerKeywordActions, keywordAction, playerInitData,
	    buffPos, rulesFileName);*/
	  std::stringstream e {};
	  e<<"Error: expected section\\s \"";
	  for(auto keywordAction: headerKeywordActions)
	    {
	      
	      if(!(keywordAction.found || keywordAction.foundMultipleOptional) &&
		 keywordAction.headerAction != nullptr)
		{
		  e<<keywordAction.keyword<<", ";
		}
	    }
	  e<<"\" in header section. Encountered character \""<<*(--buffPos)
	   <<*(++buffPos)<<*(++buffPos)<<"\", when reading \""<<rulesFileName
	   <<"\" file\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
    }
  
  readEndOfHeader
    (rawRules, buffPos,
     concat("reading end of rules.lev header \"", rulesFileName, "\""));
}


void initPlayer(const yx maxyx, const char rulesFileName[], rules & levelRules,
		const size_t bgSize, const std::string & rawRules,
		std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;

  /* Setup keyword actions associations for player section. This should contain
     an entry for each thing that the player constructor takes. If there is a
     variable that the player constructor needs but we don't want to be
     specifiable in rules.lev files then it's entry here should have a default
     value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> playerHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {SPRITE_FILE_SECTION_HEADER, readStringsSection},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInNNumbers},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_HEALTH_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_GRAV_CONST_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_VERT_V_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_FALL_JMP_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_MAX_JMP_NUM_SECTION_HEADER, nullptr}};

  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
  for(auto keywordAction: playerHeaderKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  playerInitialData playerInitData {};

  // Parse player sub sections.
  for(int targetIter {}; targetIter < (int)playerHeaderKeywordActions.size();
      targetIter++)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);
      
      if(targetFound == "")
	{
	  break;
	}      
      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)playerHeaderKeywordActions.size();
	  foundIter++)
	{
	  if(targetFound == playerHeaderKeywordActions[foundIter].keyword)
	    {
	      if(playerHeaderKeywordActions[foundIter].found)
		{
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section player section, however this "
		    "keyword is only expected once in this section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(playerSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading sprite dir strings from rules.lev header "
			    "file \"", rulesFileName, "\""),
		     &playerInitData.spritePaths);
		 
		  break;
		case 1:
		  playerHeaderKeywordActions[foundIter].found = true;
		  playerHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading single coord section from rules.lev header"
			    " file  \"", rulesFileName, "\""),
		     &playerInitData.coordinate);
		  break;
		case 2:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 3:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 4:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 5:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
                case 6:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 7:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section player section, however this "
		    "keyword is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

  readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ", PLAYER_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat(PLAYER_HEADER_SECTION_SPECIFIER, " section"));
  
  // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: playerHeaderKeywordActions)
    {
      if(!keywordAction.found)
	{
	  // See if there is a default value for not found.
	  checkForDefaultPlayerValues
	    (playerHeaderKeywordActions, keywordAction, playerInitData,
	     buffPos, rulesFileName);
	}
    }

  levelRules.gamePlayer =
    new player(playerInitData.spritePaths, maxyx, bgSize,
	       playerInitData.coordinate, playerInitData.direction,
	       playerInitData.health, playerInitData.gravitationalConstant,
	       playerInitData.maxVerticalVelocity,
	       playerInitData.maxFallingJumpNumber,
	       playerInitData.maxJumpNumber);
}


void initBgSprites(const yx maxyx, const char rulesFileName[],
		   rules & levelRules, const size_t bgSize,
		   const std::string & rawRules,
		   std::string::const_iterator & buffPos)
{
  using namespace levelFileKeywords;
  /* Setup keyword actions associations for background sprite section. This
     should contain an entry for each thing that the bgSprite constructor takes.
     If there is a variable that the bgSprite constructor needs but we don't
     want to be specifiable in rules.lev files then it's entry here should have
     a default value and the action function should be set nullptr. */
  std::vector<keywordAction::headerKeywordAction> bgSpriteHeaderKeywordActions
    {keywordAction::headerKeywordAction
     {SPRITE_FILE_SECTION_HEADER, readStringsSection},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_COORD_SECTION_HEADER, readSingleCoordSectionInZNumbers},
     keywordAction::headerKeywordAction
     {SPRITE_INIT_DIR_SECTION_HEADER, nullptr},
     keywordAction::headerKeywordAction
     {SPRITE_DISPLAY_IN_FORGROUND, readBoolSection}};

  std::vector<std::string> targets {};
  std::string targetFound {};

    readSectionOpeningBracket
    (rawRules, buffPos,
     concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));
  
  for(auto keywordAction: bgSpriteHeaderKeywordActions)
    {
      targets.push_back(keywordAction.keyword);
    }

  bgSpriteInitialData bgSpriteInitData {};

  // Parse background sprite sub sections.
  for(int targetIter {}; targetIter < (int)bgSpriteHeaderKeywordActions.size();
      targetIter++)
    {
      targetFound = skipSpaceUpTo(rawRules, buffPos, targets);

      if(targetFound == "")
	{
	  break;
	}

      /* Target found, now check which object it's associated with and perform
	 targets associated action. */
      for(int foundIter {}; foundIter < (int)bgSpriteHeaderKeywordActions.size();
	  foundIter++)
	{
	  if(targetFound == bgSpriteHeaderKeywordActions[foundIter].keyword)
	    {
	      if(bgSpriteHeaderKeywordActions[foundIter].found)
		{
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" twice when "
		    "reading header sub section background sprite section, "
		    "however this keyword is only expected once in this "
		    "section.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      switch(bgSpriteSectionKeywordToId.at(targetFound).order)
		{
		case 0:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading background sprite dir strings from "
			    "rules.lev header file \"", rulesFileName, "\""),
		     &bgSpriteInitData.spritePaths);
		  break;
		case 1:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading single coord section from rules.lev header"
			    "file  \"", rulesFileName, "\""),
		     &bgSpriteInitData.coordinate);
		  break;
		case 2:
		  goto ENCOUNTERED_FORBIDDEN_KEYWORD;
		case 3:
		  bgSpriteHeaderKeywordActions[foundIter].found = true;
		  bgSpriteHeaderKeywordActions[foundIter].action
		    (rawRules, buffPos,
		     concat("reading boolean section from rules.lev header file"
			    " \"", rulesFileName, "\""),
		     &bgSpriteInitData.displayInForground);
		  break;
		}

	      if(false)
		{
		ENCOUNTERED_FORBIDDEN_KEYWORD:
		  std::stringstream e {};
		  e<<"Error: reading rules.lev header file \""<<rulesFileName
		   <<"\". Encountered keyword \""<<targetFound<<"\" when "
		    "reading header sub section background sprite section, "
		    "however this keyword is forbidden.\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	    }
	}
    }

    readSectionEndingBracket
    (rawRules, buffPos,
     concat("reading header sub section ", BG_SPRITE_HEADER_SECTION_SPECIFIER,
	    " from \"", rulesFileName, "\""),
     concat("a ", BG_SPRITE_HEADER_SECTION_SPECIFIER, " section"));

    // Check that we've encountered all keywords that were searched for.
  for(auto keywordAction: bgSpriteHeaderKeywordActions)
    {
      if(!keywordAction.found)
	{
	  // See if there is a default value for not found.
	  checkForDefaultBgSpriteValues
	    (bgSpriteHeaderKeywordActions, keywordAction, bgSpriteInitData,
	     buffPos, rulesFileName);
	}
    }

  levelRules.bgSprites.push_back
    (new bgSprite(bgSpriteInitData.spritePaths, maxyx, bgSize,
		  bgSpriteInitData.coordinate, bgSpriteInitData.direction,
		  bgSpriteInitData.displayInForground));
}


void readStartOfHeader(const std::string & buff,
		       std::string::const_iterator & buffPos,
		       const std::string & eMsg)
{
  readSectionOpeningBracket(buff, buffPos, eMsg, std::string{"the header"});
}


void readSectionOpeningBracket(const std::string & buff,
			       std::string::const_iterator & buffPos,
			       const std::string & eMsg,
			       const std::string & section)
{
  using namespace levelFileKeywords;
    
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets.push_back(std::string {RULES_HEADER_SECTION_START_DENOTATION});
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound != std::string {RULES_HEADER_SECTION_START_DENOTATION})
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_START_DENOTATION<<"\" to "
	"denote the start of "<<section<<" when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


void readSectionEndingBracket(const std::string & buff,
			      std::string::const_iterator & buffPos,
			      const std::string & eMsg,
			      const std::string & section)
{
  using namespace levelFileKeywords;
    
  std::vector<std::string> targets {};
  std::string targetFound {};

  targets = {std::string {RULES_HEADER_SECTION_END_DENOTATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_SECTION_END_DENOTATION<<"\" to "
	"denote the end of "<<section<<" when "<<eMsg<<". Encountered "<<"\""
       <<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


void readStringsSection(const std::string & buff,
			std::string::const_iterator & buffPos,
			const std::string & eMsg, void * strings)
{
  using namespace levelFileKeywords;
  
  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (buff, buffPos, eMsg, concat("string section"));
  
  // We're in a new section.
  while(true)
    {
      std::string newString {};
	  
      targets.push_back(std::string {STRING_DENOTATION});
      // Skip to start of string.
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      targets.clear();
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \""<<STRING_DENOTATION<<"\" to start a "
	    "string when "<<eMsg<<". Encountered \""<<*buffPos<<"\".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}

      std::string::const_iterator peekBuffPos {buffPos};
      peekBuffPos++;

      if(*buffPos != STRING_DENOTATION)
	{
	  // We don't have an empty string
	  do
	    {
	      if(peekBuffPos == buff.end())
		{
		  std::stringstream e {};
		  e<<"Error: file ended unexpectedly when "<<eMsg<<".\n";
		  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		}
	      else if(*buffPos == STRING_ESC)
		{
		  if(*peekBuffPos != STRING_DENOTATION &&
		     *peekBuffPos != STRING_ESC)
		    {
		      std::stringstream e {};
		      e<<"Error: encountered character (\""<<*peekBuffPos
		       <<"\") other then \""<<STRING_ESC<<"\" or \""
		       <<STRING_DENOTATION<<"\" after escape character \""
		       <<STRING_ESC<<"\" when "<<eMsg<<".\n";
		      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
		    }
		  else
		    {
		      // Skip adding char at buffPos!
		      newString.push_back(*peekBuffPos);
		      peekBuffPos++;
		      if(peekBuffPos == buff.end())
			{
			  std::stringstream e {};
			  e<<"Error: file ended unexpectedly when "<<eMsg<<".\n";
			  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
			}  
		    }
		}
	      else
		{
		  if(*buffPos == STRING_DENOTATION)
		    {
		      // We've finished reading the string.
		      break;
		    }
		  // BuffPos isn't pointing at a STRING_ESC char.
		  newString.push_back(*buffPos);
		}
		  
		  
	      buffPos = peekBuffPos;
	      peekBuffPos++;
	    } while(true);
	}
      ((std::vector<std::string>*)strings)->push_back(newString);
      newString.clear();


      buffPos++;

      targets.push_back(std::string {STRING_SEPARATION});
      targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
      // Skip to string separation.
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      targets.clear();
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \""<<STRING_SEPARATION<<"\" or \""
	   <<RULES_HEADER_SECTION_END_DENOTATION<<"\" to separate strings or "
	    "end the string section when "<<eMsg
	   <<". Encountered \""<<*buffPos<<"\".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
      else if(targetFound == std::string {RULES_HEADER_SECTION_END_DENOTATION})
	{
	  // We've read all the strings in the string section.
	  break;
	}
    }
}


void readSingleCoordSectionInNNumbers(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, false, coord,
			 "natural numbers");
}


void readSingleCoordSectionInZNumbers(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, void * coord)
{
  readSingleCoordSection(buff, buffPos, eMsg, true, coord, "integers");
}


void readSingleCoordSection(const std::string & buff,
			  std::string::const_iterator & buffPos,
			    const std::string & eMsg, const bool useIntegers,
			    void * coord, const std::string typeOfNumber)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets {};
  std::string targetFound {};

  readSectionOpeningBracket
    (buff, buffPos, eMsg,
     concat("single coordinate section (with " , typeOfNumber, ")"));
  
  ((yx*)coord)->y = readSingleNum(buff, buffPos, eMsg, useIntegers);
      
  targets = {std::string {COORD_SEPARATION}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<COORD_SEPARATION<<"\" before second coordinate "
	"component in single coordinate section (with "<<typeOfNumber
       <<") when "<<eMsg<<". Encountered "<<"\""<<*buffPos<<"\"\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  ((yx*)coord)->x = readSingleNum(buff, buffPos, eMsg, useIntegers);

  readSectionEndingBracket
    (buff, buffPos, eMsg,
     concat("single coordinate section (with ", typeOfNumber, ")"));
}


void readBoolSection(const std::string & buff,
		    std::string::const_iterator & buffPos,
		    const std::string & eMsg, void * boolean)
{
  using namespace levelFileKeywords;

  std::string sFalse {"false"}, sTrue {"true"}, nFalse {"0"}, nTrue {"1"};
  std::vector<std::string> targets
    {sFalse, sTrue, nFalse, nTrue};
  std::string targetFound {};

  readSectionOpeningBracket(buff, buffPos, eMsg, concat("bool section"));

  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == sFalse || targetFound == nFalse)
  {
    *((bool *)boolean) = false;
  }
  else if(targetFound == sTrue || targetFound == nTrue)
    {
      *((bool *)boolean) = true;
    }
  else
    {
      std::stringstream e {};
      e<<"Error: expected one of (";
	for(auto target: targets)
	  {
	    e<<target<<", ";
	  }
	e<<") in boolean section. When "<<eMsg<<". Encountered \""
	 <<*buffPos<<"\".\n";
	exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  readSectionEndingBracket(buff, buffPos, eMsg, concat("bool section"));
}


int readSingleNum(const std::string & buff,
			  std::string::const_iterator & buffPos,
		  const std::string & eMsg, const bool useIntegers)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  if(useIntegers)
    {
      targets.push_back("-");
    }
  std::string targetFound {};
  std::stringstream number {};

  // Skip space up until the start of the number.
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected non-negative number when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  --buffPos;
  if(targetFound == "-")
    {
      number<<*buffPos;
      ++buffPos;
    }

  int decPlacesIter {};
  // Read in number.
  while(isNum(*buffPos))
    {
      number<<*buffPos;
      decPlacesIter++;
      buffPos++;
    }
  
  // Check if number was too long.
  if(decPlacesIter > MAX_COORD_LEN)
    {
      std::stringstream e {};
      e<<"Error: number \""<<number.str()<<"\" too long (longer than \""
       <<MAX_COORD_LEN<<"\") when "<<eMsg
       <<". Encountered \""<<*buffPos<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }

  int numberRet;
  number>>numberRet;
  return numberRet;
}


void readEndOfHeader(const std::string & buff,
		     std::string::const_iterator & buffPos,
		     const std::string & eMsg)
{
  using namespace levelFileKeywords;

  std::vector<std::string> targets {};
  std::string targetFound {};

  constexpr int nestingLevel {1};

  for(int iter {}; iter < nestingLevel; ++iter)
    {
      targets.push_back(std::string {RULES_HEADER_SECTION_END_DENOTATION});
      targetFound = skipSpaceUpTo(buff, buffPos, targets);
      if(targetFound == "")
	{
	  std::stringstream e {};
	  e<<"Error: expected \"";
	  for(int iter {}; iter < nestingLevel; ++iter)
	    {
	      e<<RULES_HEADER_SECTION_END_DENOTATION;
	    }
	  e<<"\" when "
	   <<eMsg<<". Encountered \""<<*(--buffPos)<<*(++buffPos)
	   <<*(++buffPos)<<"\".\n";
	  exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
	}
    }

  targets = {std::string {RULES_HEADER_END_DENOTATION[1]}};
  targetFound = skipSpaceUpTo(buff, buffPos, targets);
  if(*(buffPos -= 2) != RULES_HEADER_END_DENOTATION[0] || targetFound == "")
    {
      std::stringstream e {};
      e<<"Error: expected \""<<RULES_HEADER_END_DENOTATION<<"\" when "
       <<eMsg<<". Encountered \""<<*buffPos<<*(++buffPos)<<*(++buffPos)<<"\".\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
  buffPos += 2;
}


void parseRulesMain(const yx maxyx, const char bgFileName [],
		    const char rulesFileName[], rules & levelRules,
		    const size_t bgSize,
		    const std::string & rawRules,
		    std::string::const_iterator & buffPos)
{
  // Main body of rules file should start on a new line (account for if.)
  buffPos++;			
  const size_t expectedLineLength {bgSize / maxyx.y};
  int lineNumber {};
  int lineLength {};
  
  while(buffPos < rawRules.end())
    {
      if(*buffPos == levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR)
	{
	  // We've encountered a run-length encoding character.
	  const char ruleChar {*(++buffPos)};
	  ++buffPos;

	  checkRuleChar
	    (ruleChar,
	     concat("parsing main section of \"", rulesFileName, "\" (on line ",
		    lineNumber, " of main section)"));

	  const int runLength {readSingleNum
	    (rawRules, buffPos,
	     concat("reading run-length encoding length as a result of "
		    "encountering run-length encoding character \"",
		    levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR,
		    "\" while parsing main section of \"", rulesFileName,
		    "\""), false)};
	  --buffPos;

	  for(int lengthIter {}; lengthIter < runLength; ++lengthIter)
	    {
	      levelRules.coordRules.push_back(ruleChar);
	    }
	  lineLength += runLength;
	}
      else
	{
	  checkRuleChar(*buffPos,
			concat("parsing main section of \"",
			       rulesFileName,  "\" (on line ", lineNumber,
			       " of main section)"));
	  levelRules.coordRules.push_back(*buffPos);
	  lineLength++;
	}
      
      buffPos++;
      if(*buffPos == '\n')
	{
	  if(lineLength != expectedLineLength)
	    {
	      std::stringstream e {};
	      e<<"Error: reading rules.lev header file \""<<rulesFileName
	       <<"\". Encountered line of length ("<<lineLength<<") (on line "
	       <<lineNumber<<" of main section) when reading main section of "
		"file. Expected a line of length ("<<expectedLineLength<<").";

	      exit(e.str().c_str(), ERROR_BACKGROUND);
	    }
	  lineLength = 0;
	  lineNumber++;
	  buffPos++;
	}
    }

  if(lineLength != expectedLineLength)
    {
      std::stringstream e {};
      e<<"Error: reading rules.lev header file \""<<rulesFileName
       <<"\". Encountered line of length ("<<lineLength<<") (on line "
       <<lineNumber<<") when reading main section of file. Expected a line "
	"of length ("<<expectedLineLength<<").";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }

  if(levelRules.coordRules.size() != bgSize)
    {
      std::stringstream e {};
      e << "Error: reading rules.lev header file \"" << rulesFileName
	<< "\". Size ("<<levelRules.coordRules.size()<<") of main section of "
	"file not equal to size ("<<bgSize<<") of background file \""
	<<bgFileName<<"\".";
      exit(e.str().c_str(), ERROR_BACKGROUND);
    }
}


void checkRuleChar(const char potentialRule, const std::string eMsg)
{
  for(char rule: boarderRuleChars::CHARS)
    {
      if(potentialRule == rule || potentialRule == ' ')
	{
	  return;
	}
    }
  std::stringstream e {};
  e<<"Error: found that character \""<<potentialRule<<"\" is not a space, new "
    "line, \""<<levelFileKeywords::RULES_MAIN_RUNLENGTH_BEGIN_CHAR<<"\" or in "
    "the set of rule characters (";
  for(char rule: boarderRuleChars::CHARS)
    {
      e<<rule<<", ";
    }
  e<<") when "<<eMsg<<'.';
  exit(e.str().c_str(), ERROR_CHARACTER_RANGE);
}


void checkForDefaultPlayerValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 playerHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::playerInitialData & playerInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName [])
{
  using namespace levelFileKeywords;

  if(!playerSectionKeywordToId.at(keywordAction.keyword).defaultVal)
    {
      goto DEFAULT;
    }
  
  switch(playerSectionKeywordToId.at(keywordAction.keyword).order)
    {
    case 0:
      playerInitData.spritePaths = defaultValues::player::spritePaths;
      break;
    case 1:
      playerInitData.coordinate = defaultValues::player::coordinate;
      break;
    case 2:
      playerInitData.direction = defaultValues::player::direction;
      break;
    case 3:
      playerInitData.health = defaultValues::player::health;
      break;
    case 4:
      playerInitData.gravitationalConstant =
	defaultValues::player::gravitationalConstant;
      break;
    case 5:
      playerInitData.maxVerticalVelocity =
	defaultValues::player::maxVerticalVelocity;
      break;
    case 6:
      playerInitData.maxFallingJumpNumber =
	defaultValues::player::maxFallingJumpNumber;
      break;
    case 7:
      playerInitData.maxJumpNumber = defaultValues::player::maxJumpNumber;
      break;
    default:
    DEFAULT:
      std::stringstream e {};
      e<<"Error: expected section\\s \"";
      for(auto keywordAction: playerHeaderKeywordActions)
	{
	  if(!keywordAction.found && keywordAction.action != nullptr)
	    {
	      e<<keywordAction.keyword<<", ";
	    }
	}
      e<<"\" in header sub section player. Encountered character \""
       <<*buffPos<<"\", when reading \""<<rulesFileName<<"\" file\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}


void checkForDefaultBgSpriteValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 bgSpriteHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
 levelFileKeywords::bgSpriteInitialData &bgSpriteInitData,
 std::string::const_iterator &buffPos, const char rulesFileName[])
{
  using namespace levelFileKeywords;


  if(!bgSpriteSectionKeywordToId.at(keywordAction.keyword).defaultVal)
    {
      goto DEFAULT;
    }

  switch(bgSpriteSectionKeywordToId.at(keywordAction.keyword).order)
    {
    case 0:
      bgSpriteInitData.spritePaths = defaultValues::bgSprites::spritePaths;
      break;
    case 1:
      bgSpriteInitData.coordinate = defaultValues::bgSprites::coordinate;
      break;
    case 2:
      bgSpriteInitData.direction = defaultValues::bgSprites::direction;
      break;
    case 3:
      bgSpriteInitData.displayInForground =
	defaultValues::bgSprites::displayInForground;
      break;
    default:
    DEFAULT:
      std::stringstream e {};
      e<<"Error: expected section\\s \"";
      for(auto keywordAction: bgSpriteHeaderKeywordActions)
	{
	  if(!keywordAction.found && keywordAction.action != nullptr)
	    {
	      e<<keywordAction.keyword<<", ";
	    }
	}
      e<<"\" in header sub section backgroundSprite. Encountered character \""<<*buffPos
       <<"\", when reading \""<<rulesFileName<<"\" file\n";
      exit(e.str().c_str(), ERROR_RULES_LEV_HEADER);
    }
}
#include <sstream>
#include <string>
#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include "include/loadAssets.hpp"
#include "include/checkBoundsOfBounds.hpp"
#include "include/levelRules.hpp"
#include "include/draw.hpp"
#include "include/colorS.hpp"
#include "include/drawExit.hpp"
#include "include/sprite.hpp"
#include "include/player.hpp"

  // TMP ======================================== VVVVVVV
#include <iostream>
  // TMP ======================================== ^^^^^^^


// Values returned by branchOnInput
enum gameFuncRetCodes
  {
    NO_INPUT_ENTERED,		// The player did not enter any input.
    INPUT_ENTERED,		// The player entered input.
    M_QUIT_GAME,		// The player has decided to quit the game.    
    LEVEL_COMPLEAT		// The player has compleated the level.
  };


void menu(const yx maxyx, int * unprocessedDrawBuffer);
/* Where the horror happens
   (returns a game menu switch option.) :) */
int gameLoop(int * unprocessedDrawBuffer,
	     const std::vector<int> & background,
	     rules & levelRules, const yx maxyx);


int main()
{
  yx maxyx;			// Holds the window size.
  initialiseCurses(maxyx);	// Start and setup ncurses
  // Allocate memory for drawBuffer.
  int * unprocessedDrawBuffer = new int [maxyx.y * maxyx.x];
  menu(maxyx, unprocessedDrawBuffer);
  
  endwin(); //end curses mode!
  delete [] unprocessedDrawBuffer;
  return 0;
}


void menu(const yx maxyx, int * unprocessedDrawBuffer)
{
  std::vector<int> background {};	// Hold's the background
  /* Hold's the "rules" for the current level. (see physics.h and
     rules.lev.txt.) */
  rules levelRules {};
  /* Note this should be done in the menu or loop or some sub function
     called from within it since multiple level's can be played. It is
     placed here right now only for testing and development purposes. */
  
  loadAssets(maxyx, "assets/level1/level1.backgound.lev", background,
	     "assets/level1/level1.rules.lev", levelRules);
  
  bool run = true;
  while(run)
    {
      switch(gameLoop
	     (unprocessedDrawBuffer, background, levelRules, maxyx))
	{
	case M_QUIT_GAME:
	  run = false;
	  break;
	case LEVEL_COMPLEAT:
	  run = false;		// TEMPORARY!!!!!!!!!!!!!!!
	  break;
	}
    }

  drawExit(maxyx);  
}


int gameLoop(int * unprocessedDrawBuffer, const std::vector<int> & background,
	     rules & levelRules, const yx maxyx)
{
  /* Position seems to be re-initialized on each iteration of the following
     while loop. We feel like this is not what should be happening but we also
     know that we are probably wrong. */
  int position {};
  std::__1::chrono::steady_clock::time_point secStartTime
    {std::chrono::high_resolution_clock::now()};
  
  while(true)
    {
      int input {};

      const size_t backgroundLen {background.size() /
	maxyx.y};

      input = getch();
      switch(input)
	{
	case ESC_CHAR:
	  return M_QUIT_GAME;
	  break;
	case LEVEL_COMPLEAT:
	  return LEVEL_COMPLEAT;
	  break;
	}

      levelRules.physics(player::directionChars(input), position, maxyx,
			 backgroundLen, secStartTime);
      draw(unprocessedDrawBuffer, background, levelRules.gamePlayer,
	   levelRules.bgSprites, maxyx, position);
    }
}
#include "include/nonPlayerSprite.hpp"
#include "include/draw.hpp"

#include <curses.h>


void nonPlayerSprite::draw(int * unprocessedDrawBuffer, const bool updateSlice,
	    const int bgPos)
{
  // Only display sprite if part of it is in the visible region.
  if((position.x + maxBottomRightOffset.x) >= bgPos && position.x <=
     (bgPos + maxyx.x -1) && (position.y + maxBottomRightOffset.y) >= 0
     && (position.y < maxyx.y))
    {
      if(updateSlice)
	{
	  currentTime = std::chrono::high_resolution_clock::now();
	  if(std::chrono::duration_cast
	     <std::chrono::milliseconds>(currentTime - startTime).count() >
	     spriteS[direction].cycleSpeed)
	    {
	      startTime = std::chrono::high_resolution_clock::now();
	      currentSliceNumber++; // Move to next slice
	      /* -1 because indexing from 0, so currentSliceNumber shouldn't
		 be bigger then size() -1 */
	      if(currentSliceNumber >
		 (spriteS[direction].spriteSlices.size() -1))
		{ /* We have displayed all the slices so the value should
		     wrape arround. */
		  currentSliceNumber = 0;
		}
	    }      
	}
      
      for(int sliceLine{}; sliceLine <
	    spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
	  ++sliceLine)
	{      
	  for(int sliceLineIter{}; sliceLineIter <
		spriteS[direction].spriteSlices[currentSliceNumber].
		slice[sliceLine].sliceLine.size(); ++sliceLineIter)
	    { // Move curser to the right position.
	      if(position.y + sliceLine >= 0)
		{
		  if(position.y + sliceLine < maxyx.y)
		    {
		      int xPosRel;
		      xPosRel = {position.x + spriteS[direction].
			spriteSlices[currentSliceNumber].slice[sliceLine].
			offset + sliceLineIter};
		      int xPosAbs;
		      xPosAbs = {xPosRel - bgPos};

		      /* Sprite can be partially in the visible region, so only
			 print if char is in window. */
		      if(xPosRel >= bgPos && xPosRel <= (bgPos + maxyx.x -1))
			{
			  int ch;
			  ch = spriteS[direction].
			    spriteSlices[currentSliceNumber].slice[sliceLine].
			    sliceLine[sliceLineIter];
			  
			  if(ch != DRAW_NO_OP)
			    {
			      
			      unprocessedDrawBuffer[((position.y + sliceLine) *
						     maxyx.x) + xPosAbs] = ch;
			    }
			}
		    }
		  else
		    {
		      // We've drawn to the last line of the window.
		      break;
		    }
		}
	    }
	}
    }
}
//#include <stdexcept>
#include <sstream>
#include <curses.h>
#include <thread>
#include <chrono>
#include "include/player.hpp"
#include "include/colorS.hpp"
#include "include/draw.hpp"



void player::checkInitialPosIsInRangeOfLevel
(std::vector<std::string> & spritePaths, const yx maxBottomRightOffset,
 const size_t bgSize, const yx pos)
{
  const long bgLen {maxyx.x};
  if(pos.x < 0 || (pos.x + maxBottomRightOffset.x) > bgLen -1 ||
     pos.y < 0 || (pos.y + maxBottomRightOffset.y) > maxyx.y -1)
    {
      std::stringstream e {};
      e<<"Error: initial position for player sprite with file/s (";
      for(std::string name: spritePaths)
	{
	  e<<"\""<<name<<"\", ";
	}
      char eat;
      e<<") is out of range. ("<<pos.y<<','<<pos.x<<") given for position, but"
	" sprite has maximum size ("<<maxBottomRightOffset.y + 1<<','
       <<maxBottomRightOffset.x + 1<<") and window has size ("<<maxyx.y<<','
       <<bgLen<<"). Remember coords start at 0, are in the form (y,x) and the "
	"player sprite must be completely in the window.\n";
      exit(e.str().c_str(), ERROR_SPRITE_POS_RANGE);
    }
}


player::player
(std::vector<std::string> spritePaths, const yx max, const size_t bgSize,
 const yx pos, const sprite::directions dir, const int h, const double g,
 const double v, const unsigned maxFallingJmpNum, const unsigned maxJmpNum)
  : sprite(spritePaths, max, bgSize, pos, dir), health(h),
    gravitationalConstant(g), maxVertVelocity(v),
    maxFallingJumpNum(maxFallingJmpNum), maxJumpNum(maxJmpNum)
{
  /* We pass maxyx.x * maxyx.y as the third argument because it will be divided
     by maxyx.y to calculate the background length. */
  checkInitialPosIsInRangeOfLevel(spritePaths, maxBottomRightOffset, bgSize,
				  pos);
  if(gravitationalConstant > 0)
    {
      std::stringstream err {};
      err<<"Error: the gravitational g constant can't be positive, where g was "
	"found to be set to "<<gravitationalConstant<<".";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  else if(maxVertVelocity < 1)
    {
      std::stringstream err {};
      err<<"Error: the maximum vertical velocity for the player must be at "
	"least than 1 (the unit is in characters (position) per second.) "<<v
	 <<" was provided.";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
  else if(maxFallingJmpNum > maxJmpNum)
    {
      std::stringstream err {};
      err<<"Error: maxFallingJmpNum ("<<maxFallingJmpNum<<") can't be more then"
	" maxJmpNum. ("<<maxJmpNum<<").";
      exit(err.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
    }
}


/* Returns dir as the corresponding direction, if there is none returns
   DIR_NONE. */
sprite::directions player::convertDirectionCharsToDirections
(const directionChars dir)
{
  directions ret {DIR_NONE};
  switch(dir)
    {
    case UP_CHAR:
      ret = DIR_UP;
      break;
    case RIGHT_CHAR:
      ret = DIR_RIGHT;
      break;
    case DOWN_CHAR:
      ret = DIR_DOWN;
      break;
    case LEFT_CHAR:
      ret = DIR_LEFT;
      break;
    }
  return ret;
}


void player::updatePosRel(const sprite::directions dir)
{ /* Update's position of sprite in a relative fashion with reference to the
     sprite and update's direction. */
  checkDirection(dir);
  yx p {getNewPos(dir)};	// GetNewPos will check if dir is valid!
  position = p;
  if(direction != dir)
    {				// Change direction.
      resetCurrentSliceNum();
      direction = spriteAnimationDirections[dir];
    }
}


bool player::startJumping
(const int bgPosition, yx maxyx, const std::vector<char> & coordRules)
{
  bool retVar {false};
  if(jumpNum < maxJumpNum)
    {
      jumpNum++;
      vertVelocity = -gravitationalConstant;
      jumping = jumpingUp;
      retVar = true;

      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	{
	  for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
	    {
	      char rule {};
	      if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
		 rule == boarderRuleChars::BOARDER_CHAR)
		{
		  // We're going to hit something.
		  goto RETURN;
		}
	    }
	  // We don't want to hit the top of the level.
	  if(position.y == 0)
	    {
	      goto RETURN;
	    }
	  // We're not going to hit anything, so jump!
	  updatePosRel(sprite::DIR_UP);
	}
    }
  else
    {
      /* We must call this here because this function is called (INSTEAD OF HANDLEJUMPINGANDFALLING())  when
         sprite::DIR_UP is pressed and if we can't perform a new jump we must
         continue the current jump / fall. */
      handleJumpingAndFalling(bgPosition, maxyx, coordRules);
    }

 RETURN:
  return retVar;
}


void player::handleJumpingAndFalling(const int bgPosition, const yx & maxyx,
				     const std::vector<char> & coordRules)
{
  if(jumping == notJumping)
    {
      handleFalling(bgPosition, maxyx, coordRules);
    }
  else
    {
      handleJumping(bgPosition, maxyx, coordRules);
    }

  
 RETURN:
  return;			// Required by RETURN label I guess.
}


void player::handleFalling
(const int bgPosition, const yx &maxyx, const std::vector<char> &coordRules)
{
  using namespace boarderRuleChars;
  
  if((position.y + maxBottomRightOffset.y) == (maxyx.y -1))
    {
      // We're at the bottom of the level.
      return;
    }
  else
    {
      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, true, false))
	{
	  char rule {};
	  if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
	     (rule == BOARDER_CHAR ||
	      rule == PLATFORM_CHAR))
	    {
	      // There's a rule character below stopping us from falling.
	      return;
	    }
	}
    }
  
  jumpNum = maxJumpNum -maxFallingJumpNum;
  vertVelocity = gravitationalConstant;
  jumping = jumpingDown;

  // We're jumping down.
  handleFallingSimple(bgPosition, maxyx, coordRules);
}


void player::handleJumping
(const int bgPosition, const yx & maxyx, const std::vector<char> & coordRules)
{
  if(jumping == jumpingUp)
    {
      if(vertVelocity <= maxVertVelocity)
	{
	  vertVelocity -= gravitationalConstant;
	}
      else
	{
	  jumping = jumpingDown;
	  vertVelocity += gravitationalConstant;
	}
    }
  else if(jumping == jumpingDown)
    {
      if(vertVelocity > -maxVertVelocity)
	{
	  vertVelocity += gravitationalConstant;
	}
    }

  if(vertVelocity > 0)
    {
      // We are jumping up.
      for(int jumps {(int)vertVelocity}; jumps > 0; jumps--)
	{
	  for(auto coord: this->getXAbsRangeAsStrs(bgPosition, false, false))
	    {
	      char rule {};
	      if(getCoordRule(coord, coordRules, maxyx.y, rule) &&
		 rule == boarderRuleChars::BOARDER_CHAR)
		{
		  // We're going to hit something.
		  return;
		}
	    }
	  if(position.y == 0)
	    {
	      return;
	    }
	  updatePosRel(sprite::DIR_UP);
	}
    }
  else
    {
      // We're jumping down.
      handleFallingSimple(bgPosition, maxyx, coordRules);
    }
}


void player::handleFallingSimple
(const int bgPosition, const yx & maxyx, const std::vector<char> & coordRules)
{
  for(int jumps {(int)-vertVelocity}; jumps > 0; jumps--)
    {
      for(auto coord: this->getXAbsRangeAsStrs(bgPosition, true, false))
	{
	  char rule {};
	  if(getCoordRule(coord, coordRules, maxyx.y, rule))
	    {
	      // We're going to hit something (stop jumping!)
	      vertVelocity = 0;
	      jumping = notJumping;
	      jumpNum = 0;
	      return;
	    }
	}
      /* This is a simpler check but probably much less common, so we put it
	 second. */
      if((position.y + maxBottomRightOffset.y) == (maxyx.y -1))
	{
	  // We're going to hit the bottom of the level (stop jumping!)
	  vertVelocity = 0;
	  jumping = notJumping;
	  jumpNum = 0;
	  return;
	}
      updatePosRel(sprite::DIR_DOWN);
    }

}


void player::draw(int * unprocessedDrawBuffer, const bool updateSlice)
{
  for(size_t sliceLine{}; sliceLine <
	spriteS[direction].spriteSlices[currentSliceNumber].slice.size();
      ++sliceLine)
    {      
      for(size_t sliceLineIter{}; sliceLineIter <
	    spriteS[direction].spriteSlices[currentSliceNumber].
	    slice[sliceLine].sliceLine.size();
	  ++sliceLineIter)
	{
	  int ch;
	  ch = spriteS[direction].spriteSlices[currentSliceNumber].
	    slice[sliceLine].sliceLine[sliceLineIter];

	  if(ch != DRAW_NO_OP)
	    {
	      // Add character (if not DRAW_NO_OP.)
	      unprocessedDrawBuffer
		[((position.y + sliceLine) * maxyx.x) +
		 position.x + spriteS[direction].
		 spriteSlices[currentSliceNumber].slice[sliceLine].offset +
		 sliceLineIter] = ch;
	    }

	  if(updateSlice)
	    {
	      currentTime = std::chrono::high_resolution_clock::now();
	      if(std::chrono::duration_cast<std::chrono::milliseconds>
		 (currentTime - startTime).count() >
		 spriteS[direction].cycleSpeed)
		{
		  startTime = std::chrono::high_resolution_clock::now();
		  currentSliceNumber++; // Move to next slice.
		  /* -1 because indexing from 0, so currentSliceNumber shouldn't
		     be bigger then size() -1. */
		  if(currentSliceNumber >
		     (spriteS[direction].spriteSlices.size() -1))
		    {
		      /* We have displayed all the slices so the value should
			 wrape arround. */
		      currentSliceNumber = 0;
		    }
		}      
	    }
	}
    }
}
#include <stdexcept>
#include <ncurses.h>
#include <string>
#include "include/slice.hpp"
#include "include/utils.hpp"


std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x)
{
  if((offset + x) > (buff.size() / y))
    { // Check for error condition (can't scroll past end of background file).
      exit(std::string("error in (slice.cpp)->(std::vector<int> getSlice(std::vector<int> "
	   "buff, unsigned long offset, int y,int x)): (offset + x) > (array / "
		       "y)!"), ERROR_GENERIC_RANGE_ERROR);
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
	  if(iter >= spriteFile.size())
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
  endwin();
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
      char eat;
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
  return ((x + maxBottomRightOffset.x) < size_t(maxyx.x - xBound));
}


bool sprite::rightOfWindowInnerLeftMargin(const int x, const int xBound)
{ // Return true if we are to the right of the windows left inner margin.
    checkBoundValue(xBound);
    return (size_t(xBound) < x);
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
#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/utils.hpp"


namespace boarderRuleChars
{
  /* NOTE: when adding boarder rule characters you must also add them to the
     CHARS array and add the extern declarations to utils.hpp. */
  // The player cannot move through boarder chars.
  constexpr char BOARDER_CHAR = 'b';
  /* Same as boarder chars (except the player can move through them if
     moving up.) */
  constexpr char PLATFORM_CHAR = 'p';
  const std::vector<char> CHARS {BOARDER_CHAR, PLATFORM_CHAR};
};


void sleep(const unsigned long long t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


bool isNum(const char c)
{
  return (c >= '0' && c <= '9') ? true: false;
}


bool checkRange(const int a, const int min, const int max)
{ 
  return (a >= min && a < max) ? true : false;
}


bool inSingleDigitRange(const int a, const int offset)
{
  constexpr int SINGLE_DIGIT_MIN {};
  constexpr int SINGLE_DIGIT_MAX {9};
  if(((a - offset) < SINGLE_DIGIT_MIN) || ((a - offset) > SINGLE_DIGIT_MAX))
    return false;
  return true;
}


bool loadFileIntoString(const char name [], std::string & buff)
{
  std::ifstream file {};
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


std::string skipSpaceUpTo(const std::string & buff,
			  std::string::const_iterator & buffPos,
			  std::vector<std::string> & targets)
{
  std::string targetFound {};
  std::string::const_iterator outerPeekPos {buffPos};
  
  if(outerPeekPos != buff.end())
    {
      sort(targets.begin(), targets.end()); // Sort alphabetically.
      stable_sort(targets.begin(), targets.end(),
		  [](const std::string & first, const std::string & second)
		  {
		    return first.size() > second.size();
		  });			// Stable sort by length in desc order.

      // If the first char isn't white space.
      if(*outerPeekPos != ' ' &&
	 *outerPeekPos != '\n' &&
	 *outerPeekPos != '\r' &&
	 *outerPeekPos != '\t')
	{
	  // Check for targets at buffPos.
	  targetFound = findTargetInBuff(outerPeekPos, targets);
	  for(int iter {}; iter < targetFound.size(); ++iter)
	    {
	      outerPeekPos++;
	    }
	}
      else
	{
	  // skip spaces.
	  while(outerPeekPos != buff.end() && (*outerPeekPos == ' ' ||
					       *outerPeekPos == '\n' ||
					       *outerPeekPos == '\r' ||
					       *outerPeekPos == '\t'))
	    {
	      outerPeekPos++;
	    }

	  // Check for targets again.
	  if(outerPeekPos != buff.end())
	    {
	      targetFound = findTargetInBuff(outerPeekPos, targets);
	      for(int iter {}; iter < targetFound.size(); ++iter)
		{
		  outerPeekPos++;
		}
	    }
	}
    }

  buffPos = outerPeekPos;
  return targetFound;
 }


static std::string findTargetInBuff
(const std::string::const_iterator & outerPeekPos,
 const std::vector<std::string> & targets)
{
  std::string targetFound {};
  std::string::const_iterator peekPos {};
  // Iterate over targets.
  for(int targetsIter {}; targetsIter < targets.size();
      ++targetsIter)
    {
      peekPos = outerPeekPos;
      bool found {true};
      
      // Iterate over characters in target.
      for(int targetIter {}; targetIter < targets[targetsIter].size();
	  ++targetIter)
	{
	  if(*peekPos != targets[targetsIter][targetIter])
	    {
	      found = false;
	      break;
	    }
	  peekPos++;
	}
      if(found)
	{
	  targetFound = targets[targetsIter];
	  goto RETURN;
	}
    }

 RETURN:
  return targetFound;
}


/* Calls endwin() then print's e to std::cerr and finally call's exit() with status */
void exit(const std::string & e, const int status)
{
  endwin();
  std::cerr<<e<<'\n';
  exit(status);
}
#ifndef BACKGROUND_SPRITE_HPP_
#define BACKGROUND_SPRITE_HPP_
#include "nonPlayerSprite.hpp"


class bgSprite : public nonPlayerSprite
{
public:
  /* Draw sprite after player sprite? Note that this doesn't effect the sprite
     draw routine and must be check and appropriately handled in the main draw
     routine. */
  const bool displayInForground; 

  
  bgSprite(std::vector<std::string> spriteFileNames, const yx max,
	   const size_t bgSize, const yx pos, const directions dir,
	   const bool dispInFg) :
    nonPlayerSprite(spriteFileNames, max, bgSize, pos, dir),
    displayInForground(dispInFg)
  {
    
  }  
};

#endif
/* should check that the bounds file that has been read in is the same length as
the background file after it has been collapsed into int's. The length should be
a comparison of number of char's to number of int's */
#ifndef CHECKBOUNDSOFBOUNDS_H_
#define CHECKBOUNDSOFBOUNDS_H_
#include <string>
#include <vector>

/* should return an integer that represents the difference in "size" between the
   background file and the boarder file (relative to the boarder file */
int checkBoundsOfBounds(const std::string boarder, const std::vector<int> background); 

#endif
#ifndef COLLAPS_H_
#define COLLAPS_H_
#include <string>
#include <vector>
#include <map>

class chMap
{ // this class converts to and from colour offsets.
 private:
  std::map<std::string, int> strMap;  // stores special character types to int code mappings.
  std::vector<std::string> characters // special characters types.
    {"ACS_ULCORNER", "ACS_LLCORNER", "ACS_LRCORNER", "ACS_LTEE", "ACS_RTEE",
     "ACS_BTEE", "ACS_TTEE", "ACS_HLINE", "ACS_VLINE", "ACS_PLUS", "ACS_S1",
     "ACS_S3", "ACS_S7", "ACS_S9", "ACS_DIAMOND", "ACS_CKBOARD", "ACS_DEGREE",
     "ACS_PLMINUS", "ACS_BULLET", "ACS_LARROW", "ACS_RARROW", "ACS_DARROW",
     "ACS_UARROW", "ACS_BOARD", "ACS_LANTERN", "ACS_BLOCK", "ACS_LEQUAL",
     "ACS_GEQUAL", "ACS_PI", "ACS_NEQUAL", "ACS_STERLING" };
 public:
 chMap();
 int find(const std::string str);
};


/* Code for value of transparent space is 1 higher then 159 * 64 which is above
   the space of all ACS and ASCII characters whether coloured or not. */
constexpr int TRANS_SP {10176};
/* "Collapse's" rawBuff and stores into ret. copies all characters not part of
   an escape sequence into ret (one char goes into one int (thus ret may
   actually be larger then buff (but I say it is collapsed for reasons explained
   shortly)). All escape sequences are not copied rather they are used as an
   index into a chMap object and the return value (an int that should be unique)
   is stored into one value of ret instead of all the characters that make up
   the escape sequence. nonColor escape sequences start with /nc. */
void collapse(const std::string & rawBuff,
	      std::vector<int> & ret);

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
#ifndef DRAW_H_
#define DRAW_H_
#include <vector>
#include "utils.hpp"
#include "sprite.hpp"
#include "levelRules.hpp"


#define DRAW_NO_OP 10176	/* This character should be skipped */
extern const int yHeight;


void draw(int * unprocessedDrawBuffer, const std::vector<int> & buff,
	  player * playerSprite, std::vector<bgSprite *> & bgSprites,
	  const yx maxyx, const unsigned long offset);
/* Draws background at current position into unprocessedDrawBuffer. */
void drawBackground(int * unprocessedDrawBuffer, const std::vector<int> &buff,
		    const yx maxyx, const unsigned long offset);
void drawDrawBuffer(int *unprocessedDrawBuffer, const yx maxyx);
// Set's or clears the colour code based on the value of charCodeWithColor.
void setColor(const int charCodeWithColor);
/* Pushes sucessive characters from unprocessedDrawBuffer (starting at
   buffIndex) into contiguouscolorchars untill either reaching the end of
   unprocessedDrawBuffer, reaching a character that has a different colour or
   an ACS character. If a the end of unprocessedDrawBuffer is reached or a
   character with a different colour is reached returns false with
   contiguousColorChars populated and buffIndex pointing to the character after
   the last that was pushed into contiguousColorChars. If an ACS
   character is encountered returns true with acsChar set to the character
   found and with contiguousColorChars and buffIndex set as they are with the
   situation where an ACS character isn't found. */
inline bool getContiguouslyColordString
(const int * const unprocessedDrawBuffer, int & buffIndex, const yx maxyx,
 std::string & contiguousColorChars, int & acsChar);
void printAcs(const int acsCode, const bool inColor);
bool inColorRange(const int ch); /* Checks whethere ch is a colour character or not. */
int getColor(const int ch);	 /* Returns colour code encoded in ch. */
#endif
#ifndef DRAW_EXIT_H
#define DRAW_EXIT_H


void drawExit(const yx maxyx);


#endif

#ifndef INITCURSES_H_
#define INITCURSES_H_

#include <string>
#include <ncurses.h>
#include <sstream>
#include "utils.hpp"


void initialiseCurses(yx & maxyx);
void initColorPairs();


#endif
#ifndef PHYSICS_H_
#define PHYSICS_H_
#include <map>
#include <string>
#include <vector>
#include "sprite.hpp"
#include "player.hpp"
#include "backgroundSprite.hpp"


class rules
{
public:
  // The player cannot pass widthin this many character's of the window boarder's (y, x).
  const yx PLAYER_MOVEMENT_INNER_MARGIN {0, 52};
  static constexpr size_t second {1};
  // Contains position based rules.
  std::vector<char> coordRules {};
  //For sprites (holds sprite data (slices) and the rule for the sprite.)
  struct spriteInfo
  { // Sprite data to go here (when I do it.)
    sprite * sp {}; // This has to be a pointer because sprite is an ABC.
    std::string rule {};
  };
  // /* Sprites (multiple sprites can map to the same coord, thus the vector.) */
  // std::map<std::string, std::vector<spriteInfo>> spriteCoords {};
  player * gamePlayer; /* Info about user controlled sprite (AKA player.) */
  std::vector<bgSprite *> bgSprites;
  /* The player cannot pass widthin this many character's of the left and right
     window boarders (y, x). */
  const yx playerMovementInnerLRBoarder {0, 44};
  
private:
  const size_t millisecondsInSec {1000*second};
  // The duration of time we call sleep() for (in milliseconds.)
  const size_t engineSleepTime {32};
  const double sleepTimeAsAPercentageOfASecond {double(engineSleepTime / millisecondsInSec)};

  /* Set's oldTime to the current time if
     (oldTime - (the current time) >= second). */
  void resetOldTime(std::__1::chrono::steady_clock::time_point & oldTime);
  // check level agains sprite
  char intersection(const std::string & boundsInteraction,
		    const std::vector<int> spChoords);
  // check player against sprite
  char intersection(const std::vector<int> playerSpChoords,
		    const std::vector<int> spChoords);
  // check level against sprite
  char nearPass(const std::string & boundsInteraction,
		const std::vector<int> spChoords);
  // check player against sprite
  char nearPass(const std::vector<int> playerSpChoords,
		const std::vector<int> spChoords);
  // Moves the player 
  void movePlayer(sprite::directions input,
		  int & position, const yx maxyx, const size_t backgroundLength);
  /* No functions that change the position of the player should be called after
     this one for a given frame. */
  /* Calls handleFinalPlayerMovementAndWindowAndMarginInteractions after
     checking that the player won't go outside of the level after it's position
     is updated to peekPos. */
  void handleFinalPlayerMovementAndWindowAndMarginInteractionsSafe
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength);
  void handleFinalPlayerMovementAndWindowAndMarginInteractions
  (const sprite::directions newDir, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* Moves the player when interacting with the inner margin (NOTE THAT THIS
     FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
     THE INNER MARGIN AND NOT OUTSIDE THE BOUNDS OF THE LEVEL AS DEFINED BY THE
     BACKGROUND.) */
  void movePlayerWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerRightWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  /* NOTE THAT THIS FUNCTION ASSUMES THAT IT IS ALREADY KNOWN THAT PEEKPOS IS IN
   THE INNER MARGIN */
  void movePlayerLeftWhenInteractingWithInnerMargin
  (const sprite::directions input, int & position, const yx maxyx,
   const size_t backgroundLength, const yx peekPos,
   const int REACHED_INNER_MARGIN_X);
  // /* Returns number of characters untill the player bits a boarder character
  //    (if moving up). Will return -1 if there are no boarder characters above */
  // int getClosestBoarderCharAbove(const int position, const bool directContact);
  /* Handles collision with boarder characters when the player is moving down
     Input should be the player input direction and the current absolute
     position of the player sprite in the x dimension. Returns updated
     direction. */
  sprite::directions handleGroundCollision(const int position,
					   const int backgroundHeight);
  /* Handles collision with boarder characters when the player sprite is moving
     right. If there is only one character to the bottom right then the player
     will continue moving in the right direction but be moved up by one
     character unless the player has a y position of 0, in which case the
     player will be stopped. The player sprite will also be stopped if there is
     more then one character directly to the right of it. */
  sprite::directions handleRightCollision(const int position,
					  const int backgroundHeight);
  // // Analogous to handleRightCollision but of course for leftward movement.
  sprite::directions handleLeftCollision(const int position,
					 const int backgroundHeight);

#ifdef DEBUG
  // Where maxX should be the maximum size of the window and not the level.
  void printRuleChars(const int position, const int maxY, const int maxX);
#endif

public:
  void physics(const player::directionChars input, int & position, const yx maxyx,
	       const size_t backgroundLength,
	       std::__1::chrono::steady_clock::time_point & secStartTime);
  ~rules()
  {
    delete(gamePlayer);
  }
};


#endif
#ifndef LOADLEVEL_H_
#define LOADLEVEL_H_


#include <vector>
#include <string>
//#include <fstream>
#include "levelRules.hpp" // For rules.


// constexpr char BOARDER_CHAR {'b'}; // Player character's cannot pass through coordinate's marked as this.
// /* Player character's are killed uppon intersection with coordinate's marked as
//    this. */
// constexpr char KILL_CHAR {'k'};
// constexpr char DEGRADE_HEALTH_CHAR {'d'}; // Player character's loose health uppon intersection with coordinate's
// constexpr char DEGRADE_HEALTH_BOARDER_CHAR {'i'}; // Combination of DEGRADE_HEALTH_CHAR and BOARDER_CHAR.
// /* Player character's "win" the level uppon intersection with coordinates
//    marked. */
// constexpr char END_LEV_CHAR {'e'};
// // Player character's gain health point's when comming into contact
// constexpr char LIFE_POWER_UP_CHAR {'l'};

/* This function should initialise the argument background that is passed to it.
   This variable should then contain the "grphical data" of the level. It should
   initialise the levelRules argument that is passed to it, This variable should
   then contain the "rules and non player sprites" as well as the player sprite
   of the level. The bgFileName and rulesFileName are the names of the files
   that should be read in and parsed to initialise the background and levelRules
   arguments. */
void loadAssets
(const yx maxyx, const char bgFileName [], std::vector<int> & background,
 const char rulesFileName [], rules & levelRules);
void loadAndParseBackgroundFile(const yx maxyx, const char bgFileName [],
				std::vector<int> & background);
/* Load rules file, parse header, extract sprite coordinates and sprite name/s
   and process sprites. extract sprite rule after processing each sprite. Header
   should contain file location and starting position of player sprite and the
   player should be initialised using this info. The header may also contain
   other information. When a sprite coordinate, sprite and rule triple has been
   extracted add sprite and rule using sprite coordinate as key to a vector in
   the spriteCoords map in the form of a spriteInfo struct */
void loadAndParseLevelRulesFile(const yx maxyx, const char bgFileName [],
				const char rulesFileName[],
                                rules &levelRules, const size_t bgSize);
// Where rawRules holds the contents of a rules.lev file.
void parseRulesHeader(const yx maxyx, const char rulesFileName[],
			  rules & levelRules, const size_t bgSize,
		      const std::string & rawRules,
		      std::string::const_iterator & buffPos);
void initPlayer(const yx maxyx, const char rulesFileName[], rules &levelRules,
		const size_t bgSize, const std::string &rawRules,
                std::string::const_iterator &buffPos);
/* This function should be called for each background sprite section that's
   encountered. */
void initBgSprites(const yx maxyx, const char rulesFileName[], rules & levelRules,
		   const size_t bgSize, const std::string & rawRules,
		   std::string::const_iterator & buffPos);
/* Attempts to read the start of the header in a rules.lev file. */
void readStartOfHeader(const std::string &buff,
                       std::string::const_iterator &buffPos,
                       const std::string &eMsg);
/* Attempts to read the bracket at the start of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionOpeningBracket(const std::string &buff,
                               std::string::const_iterator &buffPos,
                               const std::string &eMsg,
                               const std::string &section);
/* Attempts to read the bracket at the end of a section. Calls exit with eMsg 
   and section if there is an error. */
void readSectionEndingBracket(const std::string & buff,
			      std::string::const_iterator & buffPos,
			      const std::string & eMsg,
			      const std::string & section);
/* Attempts to read the strings from a string section in a rules.lev file.
   Should be called when a series of strings is expected. Buff is the buffer
   where the strings should be located and buffPos is the position to start
   reading from. EMsg will be embedded in any error message/s the function spits
   out and should say something about the context in which readStringsSection()
   was called. Returns the strings that were read. */
void readStringsSection(const std::string & buff,
                   std::string::const_iterator & buffPos,
                   const std::string & eMsg, void * retObj);
/* Attempts to read one coordinate from a section in buff starting at buffPos.
   Emsg will be embedded in any error message/s the function spits out and
   should say something about the context in which the function was
   called. Returns the coordinate read. Only reads natural numbers (inclusive
   of 0.)*/
void readSingleCoordSectionInNNumbers(const std::string &buff,
                                      std::string::const_iterator &buffPos,
                                      const std::string &eMsg, void *coord);
/* Same as readSingleCoordSectionInNNumbers() with the exception that it can
   read integers. */
void readSingleCoordSectionInZNumbers(const std::string &buff,
                                      std::string::const_iterator &buffPos,
                                      const std::string &eMsg, void *coord);
/* This function should be called through readSingleCoordSectionInNNumbers() or
   readSingleCoordSectionInZNumbers() */
void readSingleCoordSection(const std::string &buff,
                            std::string::const_iterator &buffPos,
                            const std::string &eMsg, const bool useIntegers,
                            void *coord, const std::string typeOfNumber);
/* Attempts to read a boolean for a section in buff starting at buffPos. Emsg
   will be embedded in any error message/s the function spits out and should
   say something about the context in which the function was called. Returns
   the bool read. Can read a boolean in a string or integer format i.e. "true" /
   "false" or 1 / 0. */
void readBoolSection(const std::string & buff,
		    std::string::const_iterator & buffPos,
		    const std::string & eMsg, void * boolean);
/* Attempts to read a number starting at buffPos (will skip any space before the
   number.) */
int readSingleNum(const std::string &buff, std::string::const_iterator &buffPos,
                  const std::string &eMsg, const bool useIntegers);
// Verifies that the header of a .rules.lev file is present.
void readEndOfHeader(const std::string &buff,
                     std::string::const_iterator &buffPos,
                     const std::string &eMsg);
void parseRulesMain(const yx maxyx, const char bgFileName[],
                    const char rulesFileName[], rules &levelRules,
                    const size_t bgSize, const std::string &rawRules,
                    std::string::const_iterator &buffPos);
/* Checks if argument is an element of boarderRuleChars::CHARS (see utils.cpp).
   Or if the argument is a space character. If either of these is true then
   returns. Otherwise calls exit() */
void checkRuleChar(const char potentialRule, const std::string eMsg);


/* I.e. level can't be more then MAX_COORD_LEN chars long (or rather a player
   cannot be started at a position with a number with more places then this. */
constexpr int MAX_COORD_LEN{10};
namespace levelFileKeywords
{
  namespace keywordAction
  {
    struct headerKeywordAction
    {
      headerKeywordAction
      (const std::string & kword, void (* a)
       (const std::string & buff, std::string::const_iterator & buffPos,
	const std::string & eMsg, void * retObj),
       void (*hA)(const yx maxyx, const char rulesFileName[],
		  rules &levelRules, const size_t bgSize,
		  const std::string &rawRules,
		  std::string::const_iterator &buffPos) = nullptr,
       const bool fMO = false)
	: keyword(kword), action(a), headerAction(hA), foundMultipleOptional(fMO)
      { }

      const std::string keyword;
      // Found this keyword already (for keywords that can only appear once.).
      bool found{false};
      /* Set to true in the constructor if keyword is optional and can appear
	 multiple times. */
      const bool foundMultipleOptional;
      /* If action needs to return anything the address of a variable of the
	 appropriate type should be passed to the function via retObj. If the
	 function doesn't encounter any error (in which case it should print a
	 message and exit) *retObj should be populated with what is essential the
	 return value of the function. */
      void (*action)(const std::string &buff,
		     std::string::const_iterator &buffPos,
		     const std::string &eMsg, void *retObj);
      /* This function should be used as the action instead of the above if we
	 are at the top level of the header (in terms of sections). */
      void (*headerAction)(const yx maxyx, const char rulesFileName[],
			   rules &levelRules, const size_t bgSize,
			   const std::string &rawRules,
			   std::string::const_iterator &buffPos);
    };
  };
  // Each new section should start with a header char followed by this char.
  constexpr char RULES_HEADER_SECTION_START_DENOTATION	{'('};
  constexpr char RULES_HEADER_SECTION_END_DENOTATION	{')'};
  constexpr char RULES_HEADER_END_DENOTATION [] {"\n#"};
  // Header section header.
  const std::string PLAYER_HEADER_SECTION_SPECIFIER {"player"};
  const std::string BG_SPRITE_HEADER_SECTION_SPECIFIER {"backgroundSprite"};
  // Header sub section headers.
  // Sub section headers for general player and sprites.
  const std::string SPRITE_FILE_SECTION_HEADER		{"sprites"};
  const std::string SPRITE_INIT_COORD_SECTION_HEADER	{"initialCoordinate"};
  // Sub section headers for player sprite.
  const std::string SPRITE_INIT_DIR_SECTION_HEADER	{"initialDirection"};
  const std::string SPRITE_HEALTH_SECTION_HEADER	{"initialHealth"};
  const std::string SPRITE_GRAV_CONST_SECTION_HEADER	{"gravConst"};
  const std::string SPRITE_MAX_VERT_V_SECTION_HEADER	{"maxVelocity"};
  const std::string SPRITE_MAX_FALL_JMP_SECTION_HEADER	{"maxJumpsAfterFall"};
  const std::string SPRITE_MAX_JMP_NUM_SECTION_HEADER	{"maxJumps"};
  // Sub section headers for background sprite.
  const std::string SPRITE_DISPLAY_IN_FORGROUND         {"displayInForground"};
  // Used to store the order of a keyword and whether it has a default value.
  struct orderAndDefaultVal
  {
    int order;
    bool defaultVal;
  };
  /* Used to map keywords to unique int values so appropriate action can be
     taken for those keywords when parsing specific section section. KEY ORDER
     (first value of orderAndDefaultVal) OF THESE OBJECTS SHOULD MATCH switches
     that use these objects.! */
  const std::map<std::string, orderAndDefaultVal> headerSectionKeywordToId {
    {PLAYER_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {0, false}},
    {BG_SPRITE_HEADER_SECTION_SPECIFIER,	orderAndDefaultVal {1, false}}
  };
  const std::map<std::string, orderAndDefaultVal> playerSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
    {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
    {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
    {SPRITE_HEALTH_SECTION_HEADER,	orderAndDefaultVal {3, true}},
    {SPRITE_GRAV_CONST_SECTION_HEADER,	orderAndDefaultVal {4, true}},
    {SPRITE_MAX_VERT_V_SECTION_HEADER,	orderAndDefaultVal {5, true}},
    {SPRITE_MAX_FALL_JMP_SECTION_HEADER, orderAndDefaultVal {6, true}},
    {SPRITE_MAX_JMP_NUM_SECTION_HEADER,	orderAndDefaultVal {7, true}}
  };
  const std::map<std::string, orderAndDefaultVal> bgSpriteSectionKeywordToId {
    {SPRITE_FILE_SECTION_HEADER,	orderAndDefaultVal {0, false}},
    {SPRITE_INIT_COORD_SECTION_HEADER,	orderAndDefaultVal {1, false}},
    {SPRITE_INIT_DIR_SECTION_HEADER,	orderAndDefaultVal {2, true}},
    {SPRITE_DISPLAY_IN_FORGROUND,       orderAndDefaultVal {3, true}}
  };
  namespace defaultValues
  {
    /* These default values can only be used if defaultVal is set to true in
       orderAndDefaultVal for the values corresponding section in it's
       associated XXXSectionKeywordToId map (see above). */
    namespace player
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx coordinate {0, 1};
      const sprite::directions direction {sprite::DIR_NONE};
      const int health {16};
      const double gravitationalConstant {-0.38};
      const double maxVerticalVelocity {1.9};
      const unsigned maxFallingJumpNumber {1};
      const unsigned maxJumpNumber {3};
    }
    namespace bgSprites
    {
      const std::vector<std::string> spritePaths {{""}};
      const yx coordinate {20, 62};
      const sprite::directions direction {sprite::DIR_NONE};
      const bool displayInForground {false};
    }
  }
  /* This struct should be populated with the values that the player will
     eventually be initialised with. */
  struct playerInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
    sprite::directions direction {};
    int health {};
    double gravitationalConstant {};
    double maxVerticalVelocity {};
    unsigned maxFallingJumpNumber {};
    unsigned maxJumpNumber {};
  };
  /* This struct should be populated with the values read in for use with the
     constructor of the next background sprite to be created. */
  struct bgSpriteInitialData
  {
    std::vector<std::string> spritePaths {};
    yx coordinate {};
    sprite::directions direction {};
    bool displayInForground {};
  };
  // Misc.
  constexpr char STRING_DENOTATION		{'\"'};
  constexpr char STRING_SEPARATION		{','};
  constexpr char STRING_ESC			{'\\'};
  constexpr char COORD_SEPARATION		{','};
  constexpr char DIR_START_ABS			{'/'};
  constexpr char DIR_START_REL			{'.'};
  /* The character used for escape sequences (within a string) in .rules.lev
     files. */
  constexpr char ESCAPE_CHAR {'\\'};
  constexpr char COORD_SEPERATION {','}; // Separating character between coordinates.
  constexpr char NULL_BYTE {'\0'};
  /* If this character is encountered in the main section of a rules.lev file
     the character 2 places after it should be an ASCII number. After this
     number there can be a string of contiguous ASCII numbers (up to some
     maximum) that together represent some larger number. This number is the
     number of times the ASCII character before the number should be repeated. */
  constexpr char RULES_MAIN_RUNLENGTH_BEGIN_CHAR {'R'};
}


/* Should be called after checking for all header sections related to the
   player. Checks if all sections were found. If a section is encountered that
   wasn't found, then we check if it has a default value. If so we apply the
   default value. If it doesn't have a default value then call exit()*/
void checkForDefaultPlayerValues(
				 std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
				 playerHeaderKeywordActions,
				 levelFileKeywords::keywordAction::headerKeywordAction &keywordAction,
				 levelFileKeywords::playerInitialData &playerInitData,
				 std::string::const_iterator &buffPos, const char rulesFileName[]);
void checkForDefaultBgSpriteValues
(std::vector<levelFileKeywords::keywordAction::headerKeywordAction>
 bgSpriteHeaderKeywordActions,
 levelFileKeywords::keywordAction::headerKeywordAction & keywordAction,
 levelFileKeywords::bgSpriteInitialData & bgSpriteInitData, 
 std::string::const_iterator & buffPos,
 const char rulesFileName []);

#endif
#ifndef NON_PLAYER_SPRITE_HPP_
#define NON_PLAYER_SPRITE_HPP_
#include "sprite.hpp"


class nonPlayerSprite: public sprite
{
public:
  nonPlayerSprite
  (std::vector<std::string> & spritePaths, const yx max, const size_t bgSize,
   const yx pos, const directions dir) :
    sprite(spritePaths, max, bgSize, pos, dir)
  { }
  void draw(int * unprocessedDrawBuffer, const bool updateSlice,
	    const int bgPos);
};


#endif
#ifndef PLAYER_H_
#define PLAYER_H_
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include "sprite.hpp"
#include "utils.hpp"


class player: public sprite
{
  int health;
  const double gravitationalConstant;
  /* Negated for negative velocities (1 = 1 character per frame.). */
  const double maxVertVelocity;	// Should be a minimum of 1.
  double vertVelocity;	// Used for jumping.
  enum jumpingStates
    {
      notJumping,
      jumpingUp,
      jumpingDown		// AKA falling.
    };
  // Should hold only take on values from jumpingStates.
  int jumping {notJumping};
  /* Used to count current jump number (e.g., triple jump.) Where 0 is not
     jumping. */
  unsigned jumpNum {};
  // If the player starts falling (when not jumping) how many jumps can they do?
  const unsigned maxFallingJumpNum;
  const unsigned maxJumpNum;

  // These values are used for collision checking in the physics code.
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     NOTE THAT WE INTEND TO HAVE THESE VALUES INITIALLY POPULATED FROM A SPRITE
     FILE IN THE FUTURE (That is the "direct" offsets should be set.)
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

  /* Direct offsets are used when we want to calculate an "overlap" and "one
     off's" are used when we wan't to calculate adjacency. We put the
     aforementioned collision value types in quotes because their names are only
     really true when the direct offsets are all 0. */
  const int topCollisionDirectOffset {0};
  const int topCollisionOneOffOffset {topCollisionDirectOffset -1};
  const int bottomCollisionDirectOffset {0};
  const int bottomCollisionOneOffOffset {bottomCollisionDirectOffset + 1};
  const int rightCollisionDirectOffset {-1};
  const int rightCollisionOneOffOffset {rightCollisionDirectOffset + 1};
  const int leftCollisionDirectOffset {1};
  const int leftCollisionOneOffOffset {leftCollisionDirectOffset -1};


  // This function is not intended for use in player (and thus is private.)
  virtual void getCurrentBoundaryCoordinates(std::vector<int> & spCoords) {}
  void checkInitialPosIsInRangeOfLevel
  (std::vector<std::string> & spritePaths, const yx maxBottomRightOffset,
   const size_t bgSize, const yx pos);
  
public:
  enum directionChars
    { /* Input character to direction mapping.
         '---W---'
	 |...^...|
	 A<--|-->D
	 |...v...|                                            
	 .---S---. */
      UP_CHAR ='w',
      LEFT_CHAR ='a',
      RIGHT_CHAR ='d',
      DOWN_CHAR ='s',
    };

  player
  (std::vector<std::string> spritePaths, const yx max, const size_t bgSize,
   const yx pos, const sprite::directions dir, const int h, const double g,
   const double v, const unsigned maxFallingJmpNum, const unsigned maxJmpNum);
  
  virtual ~player() {};
  // Unlike sprite player needs to handle input direction characters.
  static directions convertDirectionCharsToDirections(const directionChars dir);
  static bool isDirectionCharInputValid(const int input);
  virtual void updatePosRel(const sprite::directions dir);
  /* Starts jump (by altering various variables) and moves the player up X
     characters, where position is the absolute x position of the player and X
     is dictated by (int)gravitationalConstant and only as long as the player
     will not hit any boarder characters in coordRules or the top of the level.
     Returns true if the the player started a new jump (this will only happen if
     maxJumpNum hasn't been reached.) If the player didn't start a new jump then
     keepJumping should be called (but only if the player can move down). */
  bool startJumping
  (const int bgPosition, yx maxyx, const std::vector<char> & coordRules);
  /* Keeps jumping if the player is jumping. That is as long as the player will
     not collide with any boarder characters or the bottom or top of the level.
     If the player is falling keep falling unless the player is above a boarder
     character or the bottom of the level. If the player isn't above any baorder
     character and isn't at the bottom of the level then start falling. */
  void handleJumpingAndFalling
  (const int bgPosition, const yx & maxyx,
   const std::vector<char> & coordRules);
  bool isJumping() {return jumping != notJumping;};
  
private:
  void handleFalling
  (const int bgPosition, const yx & maxyx,
   const std::vector<char> & coordRules);
  void handleFallingSimple
  (const int bgPosition, const yx & maxyx,
   const std::vector<char> & coordRules);
  void handleJumping
  (const int bgPosition, const yx & maxyx,
   const std::vector<char> & coordRules);
  /* Calculates all the points between the absolute position of the left +
     leftCollisionOffset and the absolute position of the right +
     rightCollisionOffset. Return value is a vector of strings of the pos's.
     Position should be the absolute x position in the level and bottomSide
     dictates whether to calculate y from the bottom of the sprite or the top of
     the sprite directContact dictates whether to use
     bottomCollisionDirectOffset or bottomCollisionOneOffOffset */
  std::vector<yx> getXAbsRangeAsStrs(const int position,
					      const int bottomSide,
					      const bool directContact)
  {
    const int absLeftPos {this->position.x + leftCollisionDirectOffset + position};
    const int absRightPos {this->position.x + maxBottomRightOffset.x +
      rightCollisionDirectOffset + position};

    const int collisionOffset {bottomSide ?
      (directContact ? bottomCollisionDirectOffset: bottomCollisionOneOffOffset) :
      (directContact ? topCollisionDirectOffset: topCollisionOneOffOffset)};
    const int y {(bottomSide ? this->maxBottomRightOffset.y: 0) + collisionOffset + this->position.y};

    std::vector<yx> retCoords {};
    for(int pos {absLeftPos}; pos <= absRightPos; pos++)
      {
	retCoords.push_back(yx(y, pos));
      }

    return retCoords;
  }


  /* Calculates all the points between the absolute position of the top right +
     topCollisionOffset and the absolute position of the bottom right +
     bottomCollisionOffset. Return value is a vector of strings of the pos's. */
  std::vector<yx> getYAbsRangeAsStrs(const int position, const bool rightSide,
				     const bool directContact)
  {
    const int absTopPos {this->position.y + topCollisionDirectOffset};
    const int absBottomPos {this->position.y + maxBottomRightOffset.y +
      bottomCollisionDirectOffset};

    const int collisionOffset {rightSide ?
      (directContact ? rightCollisionDirectOffset: rightCollisionOneOffOffset):
      (directContact ? leftCollisionDirectOffset: leftCollisionOneOffOffset)};
    const int x {(rightSide ?
		  this->maxBottomRightOffset.x: 0) +
    collisionOffset + position + this->position.x};

    
    std::vector<yx> retCoords {};
    for(int pos {absTopPos}; pos <= absBottomPos; pos++)
      {
	retCoords.push_back(yx(pos, x));
      }
    
    return retCoords;
  }
  
  
public:
  
  std::vector<yx>
  getBottomXAbsRangeAsStrsForOneOffContact(const int position)
  {
    return getXAbsRangeAsStrs(position, true, false);
  }


  std::vector<yx>
  getRightYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, true, false);
  }

  
  std::vector<yx>
  getLeftYAbsRangeAsStrsForOneOffContact(const int position)
  {
    return   getYAbsRangeAsStrs(position, false, false);
  }

  
  /* Since the player sprite can never go off screen we declare a simpler draw
     function for it here */
  void draw(int * unprocessedDrawBuffer, const bool updateSlice);
};

#endif
#ifndef SLICE_H_
#define SLICE_H_
#include <vector>

/* returns vector<int> of size y * x from buff (starting at offset). Note buff
   must be y high (conceptually) and at least x long (offset + x cannot go
   past end of (array / y) and offset should not be negative (however at the
   current time offset is an unsigned long so there is no required check). This
   should be checked for in the physics code and should cause an exception in
   this code.) */
std::vector<int> getSlice(const std::vector<int> buff, const unsigned long offset, const int y, const int x);

#endif
#ifndef SPRITE_HPP_
#define SPRITE_HPP_
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
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
  enum sliceLineIndex
    {				// Used as index into sliceLine vector.
      SLICE_LINE_ONE,
      SLICE_LINE_TWO
    };
  
protected:
    const yx maxyx;		// Window size.
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
    std::vector<int> sliceLine;	// Holds line of slice.
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
  /* The constructor reads the sprite file located at spriteFileName and
     converts it's contents to the internal data structure needed by sprite. */
  sprite(std::vector<std::string> & spritePaths, const yx max,
	 const size_t bgSize, const yx pos, const directions dir);
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
  /* Checks that the sprite at pos is in the level (at least one character must
     be in the range of the level
     [(0,0), (backgroundHeight, backgroundLength)].) Exit with error message
     otherwise. GetMaxYXOffset() should be called before this function to set 
     maxBottomRightOffset. */ 
  void checkInitialPosIsInRangeOfLevel
  (std::vector<std::string> & spritePaths, const yx maxBottomRightOffset,
   const size_t bgSize, const yx pos);
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
  bool inLevelY(const int y, const int bgYLen);
  bool inLevelX(const int x, const int bgXLen);
  bool leftOfWindowInnerRightMargin(const int x, const int xBound,
				    const yx maxyx);
  bool rightOfWindowInnerLeftMargin(const int x, const int xBound);
  // /* Checks that y and x are in range (0 to screen height and width), by calling
  //    checkRange(const int, const int). */
  // bool inWindow(const int y, const int x);
  /* Returns the of position of the sprite after moving one character (including
     diagonal movement) in the direction dir */
  yx peekAtPos(const directions dir);
  // Returns the sprite position.
  yx getPos() {return position;}
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
#ifndef COMMON_H_
#define COMMON_H_


#include <string>
#include <vector>
#include <sstream>


struct yx
{
  yx() {}  
  yx(const int y, const int x)
  {
    this->y = y;
    this->x = x;
  }

  bool operator==(const yx & rhs)
  {
    if(y != rhs.y || x != rhs.x)
      {
	return false;
      }
    return true;
  }

  bool operator!=(const yx & rhs)
  {
    return !operator==(rhs);
  }

  int y;
  int x;
};


// The window must be these dimensions.
constexpr int yHeight {48}, xWidth {170};
constexpr int ASCII_CH_MAX {127};
constexpr int MONO_CH_MAX {158};
constexpr int COLOR_CH_MAX {63};
constexpr int ASCII_NUMBER_OFFSET {48};
constexpr char ESC_CHAR {27};
//constexpr int BACKGROUND_HEIGHT {33};


enum errorsCodes {                     /* Error codes. */
                   ERROR_WIN_PARAM,    // Window not initialised - there was a
                                       // problem with the window parameters.
                   ERROR_CURSOR_PARAM, // Cursor parameters out of range.
                   ERROR_SPRITE_POS_RANGE,  // Sprite position out of range.
                   ERROR_POS_CH_RANGE,      // Character position out of range.
                   ERROR_INVALID_DIRECTION, // Direction character found not to
                                            // be valid.
                   ERROR_OPENING_FILE,      // Error opening file.
                   ERROR_CHARACTER_RANGE,   // Character out of range.
                   ERROR_COLOR_CODE_RANGE,  // Color code out of range.
                   ERROR_GENERIC_RANGE_ERROR,
                   ERROR_BACKGROUND,          // Malformed BG file.
                   ERROR_RULES_LEV_HEADER,    // Header of .level.rules file is
                                              // malformed.
                   ERROR_RULES_STRING_FIELDS, // There was an error in a field
                                              // containing string's.
                   ERROR_MALFORMED_STRING, // We encountered a malformed string.
                   ERROR_MALFORMED_COORDINATE, // We have encountered a
                                               // malformed coordinate.
                   ERROR_DUPLICATE_COORDINATE, // We have encountered a
                                               // duplicate coordinate.
                   ERROR_MALFORMED_COORDINATE_CHAR_FIELD, // We have encountered
                                                          // a coordinate
                                                          // character field
                                                          // that is malformed.
                   ERROR_BAD_LOGIC // There was an error in the code.
};


namespace boarderRuleChars
{
  /* NOTE: when adding boarder rule characters you must add then to utils.cpp as
     well as adding the corresponding extern declarations here. */
  extern const char BOARDER_CHAR;
  extern const char PLATFORM_CHAR;
  extern const std::vector<char> CHARS;
};


void sleep(const unsigned long long t);
bool isNum(const char c);
/* Returns false if a is not range [min, max). */
bool checkRange(const int a, const int min, const int max);
/* Return's true if a - offset is within the range [SINGLE_DIGIT_MIN, SINGLE_DIGIT_MAX].
   Return's false otherwise. */
bool inSingleDigitRange(const int a, const int offset);
// Returns false if unable to open file at path name.
bool loadFileIntoString(const char name[], std::string &buff);
/* Advances buffPos (past white space) until it reads one past a sequence of
   characters that matches a string in targets, where buffPos points to
   somewhere in buff and the strings in targets will be checked in order of the
   longest string to the shortest and where any two strings are the same length
   it will check the one that comes first alphabetically. Returns the string
   that was matched. Returns an empty string if no strings in targets were
   matched. BuffPos may be advanced up to buff.end() (inclusive). */
std::string skipSpaceUpTo(const std::string &buff,
                          std::string::const_iterator &buffPos,
                          std::vector<std::string> &targets);
/* Finds a target from targets in the buffer pointed to by
   outerPeekPos. Returns the target found. If no target was found returns "". */
static std::string
findTargetInBuff(const std::string::const_iterator &outerPeekPos,
		 const std::vector<std::string> &targets);
/* Calls endwin() then print's e to std::cerr and finally call's exit() with
   status */
void exit(const std::string &e, const int status);


template <typename T>
std::string concat(T newStr)
{
  return newStr;
}

/* This template function in combination with the one above is designed to
   concatenate it's arguments into a single string in the order in which they
   are given. The first argument must be an std::string, the rest of the
   arguments can be anything that std::stringstream can handle. The template
   function will return the concatenated string. This these template functions
   probably aren't very efficient since they create a new stringstream for
   every argument after the first, however it is only really designed for
   building error messages and so shouldn't be on the critical path. */
template <typename T, typename... Args>
std::string concat(std::string newStr,
		      const T & newStrCompRaw, const Args &... args)
{
  std::stringstream newStrCompsS {};
  newStrCompsS<<newStrCompRaw;
  newStr += newStrCompsS.str();
  return concat(newStr, args...);
}



/* Increment's i by n, if i equals iEnd before being incremented n times we call
   exit(str, eNum). */
template <typename T_A, typename T_B> auto
getAdvancedIter(T_A i, const T_B iEnd, const size_t n, const std::string & eMsg)
  -> T_A
{
  for(size_t iter {}; iter < n -1; ++iter)
    {
      i++;
      if(*i == *iEnd)
	{
	  exit(eMsg, ERROR_GENERIC_RANGE_ERROR);
	}
    }
  return i;
}


template<typename T>
bool getCoordRule(const int y, const int x, const std::vector<T> & rules,
		  int bgHeight, T & coordRulesRet)
{
  bool ret {false};
  int linearAddress = {y * ((int)rules.size() / bgHeight) + x};
  if(linearAddress < rules.size())
    {
      coordRulesRet = rules[linearAddress];
      for(auto rule: boarderRuleChars::CHARS)
	{
	  if(coordRulesRet == rule)
	    {
	      ret = true;
	      break;
	    }
	}
    }
  return ret;
}


template<typename T>
bool getCoordRule(const yx & pos, const std::vector<T> & rules, int bgHeight,
		  T & coordRulesRet)
{
  return getCoordRule(pos.y, pos.x, rules, bgHeight, coordRulesRet);
}


#endif
