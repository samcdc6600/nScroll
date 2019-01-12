#include "physics.h"
#include "../sprite/sprite.h"
#include "levelInteractionHandler.h"

void physics(const std::string & boundsInteraction, std::vector<sprite *> spArray)
{
  /*  for(auto sprite : spArray)
    {
      std::vector<int> spCoords {};
      //      sprite->getHardBoundaryCoordinates(spCoords);

      {
      char chCase {'n'};
      if((chCase = intersection(boundsInteraction, spCoords)) != 'n')
	{//if there is an intersection between the sprite and the bounds specified in boundsinteraction
	  switch(chCase)
	    {
	    case 'b'://intersection with boundary
	      break;
	    case 'u'://intersection with push up boundary
	      break;
	      //	    default:
	      //error
	    }
	}
      else
	if((chCase = nearPass(boundsInteraction, spCoords)) != 'n')
	  {//if there is a near pass between the sprite and the bounds specified in boundsInteraction
	    switch(chCase)
	      {
	      case 'b'://intersection with boundary
		break;
	      case 'u'://intersection with push up boundary
		break;
		//	      default:
		//error
	      }
	  }
      }
      }*/
}
