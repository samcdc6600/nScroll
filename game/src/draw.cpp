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


// Class handels the setting of color when printing characters (used in draw.cpp) argument is the default color pair.
setColorMode colorMode{56}; //If argument to object constructor is changed it must also be changed in main.cpp.



void draw(const std::vector<int> & buff,
	  player * playerSprite, std::vector<bgSprite *> & bgSprites,
	  const yx maxyx, const unsigned long offset)
{
  drawBackground(buff, maxyx, offset);

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
	  bgS->draw(true, offset);
	}
      else
	{
	  /* We think this will probably be faster than searching the whole list
	     again if the list is of a large size (and taking into consideration
	     that most sprites will probably be behind the player). */
	  drawInForground.push_back(bgS);
	}
    }
  playerSprite->draw(true);
  for(auto bgSF: drawInForground)
    {
      bgSF->draw(true, offset);
    }
  
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
  if(checkRange(y, 0, maxyx.y) && checkRange(x, 0, maxyx.x))
    {
      mvprintw(y, x, "");
    }
  else
    {
      std::stringstream e {};
      e<<"Error invalid cursor position ("<<y<<", "<<x<<") encountered.";
      exit(e.str().c_str(), ERROR_BAD_LOGIC);
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
