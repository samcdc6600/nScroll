#ifndef DRAW_H_
#define DRAW_H_
#include <vector>
#include "utils.hpp"
#include "sprite.hpp"
#include "levelRules.hpp"


#define DRAW_NO_OP 10176	/* This character should be skipped */
extern const int yHeight;


void draw(const std::vector<int> & buff,
	  player * playerSprite, std::vector<bgSprite *> bgSprites,
	  const yx maxyx, const unsigned long offset);
/* Places curser at (0,0) and iterates over the current background slice, calling draw for each character. */
void drawBackground(const std::vector<int> & buff, const yx maxyx, const unsigned long offset);
/* Checks that y and x are in range and then calls mvprintw(y, x, "") */
void setCursor(const int y, const int x, const yx maxyx);
/* Does the actual drawing (e.g. calls printw() or addch() and set's colour appropriately. 
   NOTE: I had to rename this function "drawCh" from "draw" because I was getting a segmentation fault when
   calling it from sprie::draw in sprite.cpp. Sprite does not contain a member function with the same signature as
   this function, so I'm really not sure what the problem is. I suspect it has something to do with sprite::draw
   being a memeber function and maybe I had unwittingly strayed into undefined behaviour :O. In any case I think
   drawCh is a more apt name anyway :)*/
void drawCh(int ch);
bool inColorRange(const int ch); /* Checks whethere ch is a colour character or not. */
int getColor(const int ch);	 /* Returns colour code encoded in ch. */
#endif
