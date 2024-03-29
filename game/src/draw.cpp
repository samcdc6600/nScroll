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
setColorMode colorMode {};


void calculateChunksToCheck
(const backgroundData & background, std::vector<yx>  & chunksToCheck);


void draw
(const backgroundData & background, player * gamePlayer,
 const std::map<std::string, std::vector<bgSprite *>> & bgSprites)
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

  std::vector<bgSprite *> forgroundBgSprites;

  drawBgBackgroundBgSprites(background, bgSprites,
			  forgroundBgSprites);
  gamePlayer->draw(background, true);
  drawFgBgSprites(background, forgroundBgSprites); 
  printDrawBuffer(background.secondStageDrawBuffer, background.chunkSize);
  refresh();
}


void drawBgBackgroundBgSprites
(const backgroundData & background,
 const std::map<std::string, std::vector<bgSprite *>> & bgSprites,
 std::vector<bgSprite *> & forgroundBgSprites)
{
  std::vector<yx> chunksToCheck {};

  calculateChunksToCheck(background, chunksToCheck);

  for(yx chunkKey: chunksToCheck)
    {
      try
	{
	  for(bgSprite * sprite: bgSprites.at(createChunkCoordKey(chunkKey)))
	    {
	      if(!sprite->displayInForground)
		{
		  sprite->draw(background.secondStageDrawBuffer, true,
			       background.getViewPortPosition());
		}
	      else
		{
		  forgroundBgSprites.push_back(sprite);
		}
	    }
	}
      catch(const std::out_of_range & err)
	{
	  // No bg sprites in this chunk. Do nothing.
	}
    }
}


void calculateChunksToCheck
(const backgroundData & background, std::vector<yx>  & chunksToCheck)
{
  /* This function can probably be simplified... */
  yx vPPRelativeChunk
    {convertCharCoordToChunkCoord
     (background.chunkSize, background.getViewPortPosition())};  
  const yx viewPortChunkAlignment
    {background.getViewPortPosition() % background.chunkSize};

  /* If we are only 1 character from the chunk below we want to use the
     coordinates of the below chunk as no sprite from the chunk above us should
     be visible given that sprites should not be larger than one chunk. */
  if(background.getViewPortPosition().y < 0 ?
     abs(viewPortChunkAlignment.y) == 1:
     viewPortChunkAlignment.y == background.chunkSize.y -1)
    {
      vPPRelativeChunk.y += 1;
    }
  /* Here we follow the same logic as above only for the x axis. */
  if(background.getViewPortPosition().x < 0 ?
     abs(viewPortChunkAlignment.x) == 1:
     viewPortChunkAlignment.x == background.chunkSize.x -1)
    {
      vPPRelativeChunk.x += 1;
    }
  
  /* Coordinates of chunks we need to check for sprites in. Always includes the
     chunk the view port origin is in (at least almost always, see above.) */
  chunksToCheck.push_back(vPPRelativeChunk);
  
  /* No matter what the alignment of the VPP is we always need to check the
     chunks 1 left of the VPP relative chunk, 1 above and 1 to the left and
     1 above it. */
  chunksToCheck.push_back(yx{vPPRelativeChunk.y -1, vPPRelativeChunk.x});
  chunksToCheck.push_back(yx{vPPRelativeChunk.y, vPPRelativeChunk.x -1});
  chunksToCheck.push_back(yx{vPPRelativeChunk.y -1, vPPRelativeChunk.x -1});
  
  /* If the view port position is misaligned (relative to a chunk) in y then we
     need to check two extra chunks. One to the left and below the VPP relative
     chunk (the chunk returned by convertCharCoordToChunkCoord
     (background.chunkSize, background.getViewPortPosition())) and one below
     the VPP relative chunk. This however is not the case if the top of the
     VPP is only one char off of the top of the next chunk. */
  if((background.getViewPortPosition().y < 0 ?
     (abs(viewPortChunkAlignment.y) != 1):
     (abs(viewPortChunkAlignment.y) != background.chunkSize.y -1)) &&
     viewPortChunkAlignment.y != 0)
    {
      chunksToCheck.push_back(yx{vPPRelativeChunk.y +1, vPPRelativeChunk.x -1});
      chunksToCheck.push_back(yx{vPPRelativeChunk.y +1, vPPRelativeChunk.x});
    }
  
  /* If the view port position is misaligned (relative to a chunk) in x then we
     need to check two extra chunks. One above and to the right of the VPP
     relative chunk (the chunk returned by convertCharCoordToChunkCoord
     (background.chunkSize, background.getViewPortPosition())) and one to the
     right of the VPP relative chunk. This however is not the case if the VPP is
     only one char off of the next chunk to the right. */
  if((background.getViewPortPosition().x < 0 ?
     (abs(viewPortChunkAlignment.x) != 1):
     (abs(viewPortChunkAlignment.x) != background.chunkSize.x -1)) &&
     viewPortChunkAlignment.x != 0)
    {
      chunksToCheck.push_back(yx{vPPRelativeChunk.y -1, vPPRelativeChunk.x +1});
      chunksToCheck.push_back(yx{vPPRelativeChunk.y, vPPRelativeChunk.x +1});
    }
  
  /* If the VPP is misaligned in both axis we need to add both of the chunks
     described in the previous comments about misalignment as well as the chunk
     one down and to the right. */
  if(background.getViewPortPosition().y < 0 ?
     abs(viewPortChunkAlignment.y) != 1 && abs(viewPortChunkAlignment.x) != 1:     
     ((abs(viewPortChunkAlignment.x) != background.chunkSize.x -1) &&
      viewPortChunkAlignment.x != 0) &&
     (abs(viewPortChunkAlignment.y) != background.chunkSize.y -1) &&
     viewPortChunkAlignment.y != 0)
    {
      chunksToCheck.push_back(yx{vPPRelativeChunk.y +1, vPPRelativeChunk.x +1});
    }
}


void drawFgBgSprites
(const backgroundData & background, const std::vector<bgSprite *> & bgSprites)
{
  for(bgSprite * sprite: bgSprites)
    {
      sprite->draw(background.secondStageDrawBuffer, true,
		   background.getViewPortPosition());
    }
}


void printDrawBuffer
(backgroundData::chunkElementBaseType * secondStageDrawBuffer, const yx viewPortSize)
{
  setColorMode color {};
  mvprintw(0, 0, "");

  for(int iter {}; iter < (viewPortSize.y * viewPortSize.x);
      ++iter)
    {
      std::string contiguousColorChars;
      backgroundData::chunkElementBaseType acsCode;
      bool foundAcsCode;

      color.setColorFromChar(secondStageDrawBuffer[iter]);
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


inline bool getContiguouslyColordString
(const backgroundData::chunkElementBaseType * const secondStageDrawBuffer,
 int & buffIndex, const yx viewPortSize, std::string & contiguousColorChars,
 backgroundData::chunkElementBaseType & acsCode)
{
  setColorMode colorMode {};
  
  auto getChar = [& colorMode](const int ch, bool & aCS)
  {
    // Remove colour information (first color starts at 1, so we remove 1.)
    int rawCh {ch - ((colorMode.getColor(ch) -1) * MONO_CH_MAX)};
    aCS = false;

    if(rawCh > ASCII_CH_MAX && rawCh <= MONO_CH_MAX)
      {
	// We have an ACS char.
	aCS = true;
      }
    else if(rawCh < 0 || rawCh > ASCII_CH_MAX)
      {
	exit(concat("Error: encountered character (", (int)rawCh, ") that is lass"
		    " than 0 or greater than ", MONO_CH_MAX,". after having color "
		    "info removed."),
	     ERROR_CHARACTER_RANGE);
      }
  
    return rawCh;
  };
  

  const int startColorCode =
    colorMode.getColor(secondStageDrawBuffer[buffIndex]);

  for( ; buffIndex < (viewPortSize.y * viewPortSize.x) &&
	 colorMode.getColor(secondStageDrawBuffer[buffIndex]) == startColorCode;
       buffIndex++)
    {
      bool foundACSCode {};
      backgroundData::chunkElementBaseType ch
	{getChar(secondStageDrawBuffer[buffIndex], foundACSCode)};

      if(!foundACSCode)
	{
	  // We have an ASCII char.
	  contiguousColorChars.push_back(ch);
	}
      else
	{
	  // We have an ACS char. 
	  acsCode = ch;
	  return true;
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
