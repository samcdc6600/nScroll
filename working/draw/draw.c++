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
