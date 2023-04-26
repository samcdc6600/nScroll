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


void draw
(backgroundData & background,
 player * gamePlayer, std::vector<bgSprite *> & bgSprites)
{
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
  // std::vector<bgSprite *> drawInForground;

  // for(auto bgS: bgSprites)
  //   {
  //     if(!bgS->displayInForground)
  // 	{
  // 	  bgS->draw(secondStageDrawBuffer, true, viewPortPosition);
  // 	}
  //     else
  // 	{
  // 	  /* We think this will probably be faster than searching the whole list
  // 	     again if the list is of a large size (and taking into consideration
  // 	     that most sprites will probably be behind the player). */
  // 	  drawInForground.push_back(bgS);
  // 	}
  //   }  
  gamePlayer->draw(background, true);
  // for(auto bgSF: drawInForground)
  //   {
  //     bgSF->draw(secondStageDrawBuffer, true, viewPortPosition);
  //   }
 
  printDrawBuffer(background.secondStageDrawBuffer, background.chunkSize);
  refresh();
}


void printDrawBuffer
(backgroundData::chunkElementBaseType * secondStageDrawBuffer, const yx viewPortSize)
{   
  mvprintw(0, 0, "");

  for(int iter {}; iter < (viewPortSize.y * viewPortSize.x);
      ++iter)
    {
      std::string contiguousColorChars;
      unsigned short acsCode;
      bool foundAcsCode;

      setColor(secondStageDrawBuffer[iter]);
      foundAcsCode = getContiguouslyColordString
	(secondStageDrawBuffer, iter, viewPortSize,
	 contiguousColorChars, acsCode);

      if(contiguousColorChars.size() != 0)
	{
	  // Print ASCII chars.
	  printw(contiguousColorChars.c_str());
	}
      if(foundAcsCode)
	{
	  // Print ACS char.
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
(const backgroundData::chunkElementBaseType * const secondStageDrawBuffer,
 int & buffIndex, const yx viewPortSize, std::string & contiguousColorChars,
 unsigned short & acsCode)
{
  const unsigned short startColorCode =
    getColor(secondStageDrawBuffer[buffIndex]);
  // const int startIndex {buffIndex};


  for( ; buffIndex < (viewPortSize.y * viewPortSize.x) &&
	 getColor(secondStageDrawBuffer[buffIndex]) == startColorCode;
       buffIndex++)
    {
      unsigned short ch;
      ch = secondStageDrawBuffer[buffIndex];

      // Remove colour information (if any) from ch.
      if(inColorRange(ch))
	{
	  ch -= (getColor(ch) * (MONO_CH_MAX + 1));
	}

      if(ch <= ASCII_CH_MAX)
	{
	  // We have an ASCII char.
	  contiguousColorChars.push_back(ch);
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
