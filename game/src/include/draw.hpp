#ifndef DRAW_HPP_
#define DRAW_HPP_
#include <vector>
//#include "utils.hpp"
#include "sprite.hpp"
#include "levelRules.hpp"
#include "background.hpp"


#define DRAW_NO_OP 10176	/* This character should be skipped */
extern const int yHeight;


void draw
(backgroundData::drawBufferType * secondStageDrawBuffer,
 backgroundData & background, player * playerSprite,
 std::vector<bgSprite *> & bgSprites);
// //Draws background at current position into secondStageDrawBuffer.
void printDrawBuffer
(backgroundData::drawBufferType * secondStageDrawBuffer, const yx viewPortSize);
// Set's or clears the colour code based on the value of charCodeWithColor.
void setColor(const int charCodeWithColor);
/* Pushes sucessive characters from secondStageDrawBuffer (starting at
   buffIndex) into contiguouscolorchars untill either reaching the end of
   secondStageDrawBuffer, reaching a character that has a different colour or
   an ACS character. If a the end of secondStageDrawBuffer is reached or a
   character with a different colour is reached returns false with
   contiguousColorChars populated and buffIndex pointing to the character after
   the last that was pushed into contiguousColorChars. If an ACS
   character is encountered returns true with acsChar set to the character
   found and with contiguousColorChars and buffIndex set as they are with the
   situation where an ACS character isn't found. */
inline bool getContiguouslyColordString
(const backgroundData::drawBufferType * const secondStageDrawBuffer,
 int & buffIndex, const yx viewPortSize, std::string & contiguousColorChars,
 unsigned short & acsCode);
void printAcs(const int acsCode, const bool inColor);
/* Checks whethere ch is a colour character or not. */
bool inColorRange(const int ch);
int getColor(const int ch);	 /* Returns colour code encoded in ch. */
#endif
