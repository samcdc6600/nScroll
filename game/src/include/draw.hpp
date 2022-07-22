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
