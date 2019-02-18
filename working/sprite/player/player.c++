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
void player::updatePosRel(char ch)
{//update's position of sprite in a relative fashion with reference to the sprite and update's direction.
   try
     {
       switch(ch)
	 {
	 case LEFT_UP:
	 case LEFT_UP_UPPER://left up
	   if(!inScreenBounds(--position.y, --position.x))
	     {			// Reposition if we've gone out of bounds.
	       ++position.y;
	       ++position.x;
	     }
	   else
	     if(direction != DIR_LEFT)//if we are changind the direction
	       {
		 resetCurrentSliceNum(DIR_RIGHT);//we don't wan't to cause a seg fault
		 direction = DIR_LEFT;//change sprite
	       }	   
	   break;
	 case UP:
	 case UP_UPPER://up
	   if(!inScreenBounds(--position.y, position.x))
	     {
	       ++position.y;
	     }
	   else
	     if(direction != DIR_UP)
	       {
		 resetCurrentSliceNum(DIR_LEFT);
		 direction = DIR_UP;
	       }
	   break;
	 case RIGHT_UP:
	 case RIGHT_UP_UPPER://right up
	   if(!inScreenBounds(--position.y, ++position.x))
	     {
	       ++position.y;
	       --position.x;
	     }
	   else
	     if(direction != DIR_RIGHT)
	       {
		 resetCurrentSliceNum(DIR_LEFT);
		 direction = DIR_RIGHT;
	       }
	   break;
	 case LEFT:
	 case LEFT_UPPER://left
	   if(!inScreenBounds(position.y, --position.x))
	     {
	       ++position.x;
	     }
	   else
	     if(direction != DIR_LEFT)
	       {	       
		 resetCurrentSliceNum(DIR_RIGHT);
		 direction = DIR_LEFT;
	       }
	   break;
	 case RIGHT:
	 case RIGHT_UPPER://right
	   if(!inScreenBounds(position.y, ++position.x))
	     {
	       --position.x;
	     }
	   else
	     if(direction != DIR_RIGHT)
	       {
		 resetCurrentSliceNum(DIR_LEFT);
		 direction = DIR_RIGHT;
	       }
	   break;
	 case LEFT_DOWN:
	 case LEFT_DOWN_UPPER://left down
	   if(!inScreenBounds(++position.y, --position.x))
	     {
	       --position.y;
	       ++position.y;
	     }
	   else
	     if(direction != DIR_LEFT)
	       {
		 resetCurrentSliceNum(DIR_RIGHT);
		 direction = DIR_LEFT;
	       }
	   break;
	 case DOWN:
	 case DOWN_UPPER://down
	   if(!inScreenBounds(++position.y, position.x))
	     {
	       --position.y;
	     }
	   else
	     if(direction != DIR_DOWN)
	       {
		 resetCurrentSliceNum(DIR_DOWN);
		 direction = DIR_DOWN;
	       }
	   break;
	 case RIGHT_DOWN:
	 case RIGHT_DOWN_UPPER://right down
	   if(!inScreenBounds(++position.y, ++position.x))
	     {
	       --position.y;
	       --position.x;
	     }
	   else
	     if(direction != DIR_RIGHT)
	       {
		 resetCurrentSliceNum(DIR_LEFT);
		 direction = DIR_RIGHT;
	       }
	   break;

	 default:
	   std::stringstream errorMsg;
	   errorMsg<<"in sprite/sprite.cpp, "
		   <<"void sprite::updatePosRel(char ch), ch out of range."
		   <<"range = [q, w, e, a, d, z, s, x] & upper case forms."
		   <<"ch = "<<ch;
	   throw std::logic_error(errorMsg.str());
	 }
     }
   catch(std::logic_error errorMsg)
     {
       mvprintw(0, 0, "%s", errorMsg.what());
       refresh();
       std::this_thread::sleep_for(std::chrono::milliseconds(19999));
       endwin();
       exit(-1);
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
