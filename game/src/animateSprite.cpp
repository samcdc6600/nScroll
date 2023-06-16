#include "include/animateSprite.hpp"


void animateSprite::velocity::setComponentsToZero()
{
  distTravelled.x = 0, distTravelled.y = 0;
  comps.x = 0, comps.y = 0;
}


void animateSprite::velocity::velocityTowardsZeroInX(const double a)
{
  if(comps.x != 0)
    {
      if(comps.x > 0)
	{
	  comps.x -= a;
	  comps.x = comps.x < 0 ? 0: comps.x;
	}
      else
	{
	  comps.x += a;
	  comps.x = comps.x > 0 ? 0: comps.x;
	}
    }
}


void animateSprite::velocity::addToYComp(const double a)
{
  if(comps.y + a > maxVelocity)
    {
      comps.y = maxVelocity;
    }
  else if(comps.y + a < -maxVelocity)
    {
      comps.y = -maxVelocity;
    }
  else
    {
      comps.y += a;
    }
}


void animateSprite::velocity::addToXComp(const double a)
{
  if(comps.x + a > maxVelocity)
    {
      comps.x = maxVelocity;
    }
  else if(comps.x + a < -maxVelocity)
    {
      comps.x = -maxVelocity;
    }
  else
    {
      comps.x += a;
    }
}


yx animateSprite::velocity::getAndSetDistTravelled()
{
  // std::cout<<"distTravelled.x = "<<distTravelled.x<<'\n';
  // // std::cout<<"toSeconds = "<<toSeconds<<'\n';
  // std::cout<<"timeElapsed = "<<lastXUpdate.getDeltaSinceLastReset()<<'\n';
  yx ret {};
  // Get distance travelled since this func was last called.
  distTravelled.y += comps.y *
    (lastYUpdate.getDeltaSinceLastReset());
  distTravelled.x += comps.x *
    (lastXUpdate.getDeltaSinceLastReset());
  // Reset distance timers.
  lastYUpdate.resetTimer();
  lastXUpdate.resetTimer();
  
  if(distTravelled.y > 1)
    {
      ret.y = 1;
      distTravelled.y -= 1;
    }
  else if(distTravelled.y < -1)
    {
      ret.y = -1;
      distTravelled.y += 1;
    }
  if(distTravelled.x > 1)
    {
      ret.x = 1;
      distTravelled.x -= 1;
    }
  else if(distTravelled.x < -1)
    {
      ret.x = -1;
      distTravelled.x += 1;
    }

  return ret;
}


yx animateSprite::velocity::getPotentialDistTravelled() const
{
  yx ret {};
  // Get distance travelled since this func was last called.
  velocityComps potentialDistTravelled {distTravelled.y, distTravelled.x};
  potentialDistTravelled.y += comps.y *
    (lastYUpdate.getDeltaSinceLastReset());
  potentialDistTravelled.x += comps.x *
    (lastXUpdate.getDeltaSinceLastReset());
  
  if(potentialDistTravelled.y > 1)
    {
      ret.y = 1;
    }
  else if(potentialDistTravelled.y < -1)
    {
      ret.y = -1;
    }
  if(potentialDistTravelled.x > 1)
    {
      ret.x = 1;
    }
  else if(potentialDistTravelled.x < -1)
    {
      ret.x = -1;
    }

  return ret;
}


void animateSprite::updatePosRel(const sprite::directions dir)
{ /* Update's position of sprite in a relative fashion with reference to the
     sprite and update's direction. */
  checkDirection(dir);
  yx p {getNewPos(dir)};	// GetNewPos will check if dir is valid!


  positionVPRel = p;
  if(direction != dir)
    {			// Change sprite direction animation.
      resetCurrentSliceNum();
      direction = spriteAnimationDirections[dir];
    }
}
