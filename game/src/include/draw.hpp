#ifndef DRAW_HPP_
#define DRAW_HPP_
#include <vector>
#include "sprite.hpp"
#include "levelRules.hpp"
#include "background.hpp"
#include "chronological.hpp"


#define DRAW_NO_OP 10176	/* This character should be skipped */
extern const int yHeight;


void draw
(const backgroundData & background, player * gamePlayer,
 const std::map<std::string, std::vector<bgSprite *>> & bgSprites);
/* Draws background sprites with displayInForground set to false. Saves
   references to background sprites with displayInForground set to true in
   forgroundbgsprites. A sprite must be in a region where it may be visible
   (sprites should be no larger than the view port) to have it's draw function
   called or have a reference to it saved in forgroundBgSprites */
void drawBgBackgroundBgSprites
(const backgroundData & background,
 const std::map<std::string, std::vector<bgSprite *>> & bgSprites,
 std::vector<bgSprite *> & forgroundBgSprites);
/* Draws sprites pointed to by bgSprites. Should be called after the player
   sprite is drawn. */
void drawFgBgSprites
(const backgroundData & background, const std::vector<bgSprite *> & bgSprites);
// Draws background at current position into secondStageDrawBuffer.
void printDrawBuffer
(backgroundData::chunkElementBaseType * secondStageDrawBuffer, const yx viewPortSize);
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
(const backgroundData::chunkElementBaseType * const secondStageDrawBuffer,
 int & buffIndex, const yx viewPortSize, std::string & contiguousColorChars,
 backgroundData::chunkElementBaseType & acsCode);
void printAcs(const int acsCode, const bool inColor);


#endif
