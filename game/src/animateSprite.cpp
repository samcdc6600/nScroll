#include "include/animateSprite.hpp"


void animateSprite::velocity::setYComponentsToZero()
{
  distTravelled.y = 0;
  comps.y = 0;
}


void animateSprite::velocity::setXComponentsToZero()
{
  distTravelled.x = 0;
  comps.x = 0;
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


/* Note that falling has a default value of false. */
void animateSprite::velocity::addToYComp
(const double a, const bool falling)
{
  if(comps.y + a > (falling? maxYVelocityFalling: maxVelocity))
    {
      comps.y = (falling? maxYVelocityFalling: maxVelocity);
    }
  else if(comps.y + a < (falling? -maxYVelocityFalling: -maxVelocity))
    {
      comps.y = (falling? -maxYVelocityFalling: -maxVelocity);
    }
  else
    {
      comps.y += a;
    }
}


void animateSprite::velocity::addToXComp
(const double a)
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


/* Note that maximumVelocity has a default value of maxVelocity and falling has
   a default value of false. */
void animateSprite::velocity::addToYCompUnconstrained(const double a)
{
  comps.y += a;
}


int animateSprite::velocity::getAndSetDistTravelledInY()
{  
  int ret {};
  // Get distance travelled since this func was last called.
  distTravelled.y +=
    comps.y * timers.getTimeSinceLastYUpdate() * yVelocitySailFactor;
  timers.resetTimeSinceLastYUpdate();

  if(distTravelled.y > 1)
    {
      ret = 1;
      distTravelled.y -= 1;
    }
  else if(distTravelled.y < -1)
    {
      ret = -1;
      distTravelled.y += 1;
    }

  return ret;
}


int animateSprite::velocity::getAndSetDistTravelledInX()
{
  int ret {};
  // Get distance travelled since this func was last called.
  distTravelled.x += comps.x * timers.getTimeSinceLastXUpdate();
  /* TODO: resetTimeSinceLastXUpdate should not be called here! (see other note
     in handleHorizontalMovementsWhenStopping()! */
  timers.resetTimeSinceLastXUpdate();
  
  if(distTravelled.x > 1)
    {
      ret = 1;
      distTravelled.x -= 1;
    }
  else if(distTravelled.x < -1)
    {
      ret = -1;
      distTravelled.x += 1;
    }

  return ret;
}


yx animateSprite::velocity::getPotentialDistTravelled() const
{
  yx ret {};
  // Get distance travelled since this func was last called.
  velocityComps potentialDistTravelled {distTravelled.y, distTravelled.x};
  potentialDistTravelled.y += comps.y * timers.getTimeSinceLastYUpdate() * yVelocitySailFactor;
  potentialDistTravelled.x += comps.x * timers.getTimeSinceLastYUpdate();
  
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


yx animateSprite::addRulesBufferOffset
    (const yx vPRelPos, const yx vPS, const yx cRBS) const
{
  yx ret {vPRelPos};
  /* Only add offset if second stage coord rules buffer is larger than view
     port in y. It should also be larger in x if this is the case and the size
     should already have been verified to be an odd multiple. */
  if(cRBS.y > vPS.y)
    {
      ret.y += ((cRBS.y - vPS.y) / 2);
      ret.x += ((cRBS.x - vPS.x) / 2);
    }

   return ret;
}
