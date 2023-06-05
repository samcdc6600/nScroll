#ifndef TIME_HPP_
#define TIME_HPP_


#include <ctime>
#include "utils.hpp"


class chronological
{
  /* NOTE THAT THIS OBJECT IS INTENDED TO BE USED IN TWO WAYS. With the first
     way tick time should be set and functions such as getDeltaSinceLastReset()
     and resetTimer() should not be called. This way is intended for timing
     things that should have regular intervals. With the second way tick
     related function should not be called and the object is intended for use
     in timing things that don't necessarily have to be regular.  */
  /* NOTE THAT CONST VARIABLES APPARENTLY CANNOT BE CHANGED WITHIN THE BODY OF
     AN ASSIGNMENT OPERACTOR DEFINED BY AN OBJECT. AS SUCH WE HAVE COMMENTED OUT
     THE CONST QUALIFIER ON A NUMBER OF VARIABLES WE WOULD LIKE TO BE
     CONST. THESE VARIABLES OF COURSE SHOULD NOT BE CHANGE ANYWHERE THEY
     WOULDN'T BE ALLOWED TO BE CHANGED IF THEY WERE CONST, OF COURSE WITH THE
     EXCEPTION OF THE BODY OF THE ASSIGNMENT OPERATOR. */
private:
  bool started {false};
  /* const */ bool tickTimeSet {false};
  /* const */ std::string toSeeIfError;
  std::__1::chrono::steady_clock::time_point tLast;
  const long double scaleFactor {1000};


  /* Many public functions belonging to this object (apart from any constructor
     and start() must call this function as their first action. If the public
     member function uses tickTime then it must call this function. */
  void checkIfFullyInitialised(std::string funcName) const
  {
    if(!tickTimeSet)
      {
	/* If tickTimeSet is false toSeeIfError equals "" and so this must be
	   first. */
	exit(concat("Error (game logic) ", funcName, " called on a "
		    "chronological object without a tick time! All "
		    "chronological objects which have ", funcName, " called on "
		    "them must be initialised with a constructor that takes "
		    "arguments or be on the left hand side of an assignment "
		    "where the right hand side object has been initialised with"
		    " a constructor that takes arguments."),
	     ERROR_BAD_LOGIC);
      }
    else
      {
	checkIfPartiallyInitialised(funcName);
      }
  }


  /* Public member functions that  */
  void checkIfPartiallyInitialised(std::string funcName) const
  {
    if(!started)
      {
	exit(concat("Error (game logic) ", funcName, " called on a "
		    "chronological object without calling start() on that "
		    "object first! See \"", toSeeIfError, "\" for possible "
		    "cause of error."),
	     ERROR_BAD_LOGIC);
      }
  }

  
public:
  /* TickTime holds the time delta (in milliseconds) at which an event should
     take place. */
  /* const */ long double tickTime;


  chronological():
    toSeeIfError(""), tickTime(0)
  {
  }
  
  
  chronological(const long double tTime, const std::string & ifError):
    toSeeIfError(ifError), tickTime(tTime), tickTimeSet(true)
  {
  }

  
  chronological & operator=(const chronological & rhs)
  {
    if(this == &rhs)
      {
	return * this;
      }
    tickTime = rhs.tickTime;
    started = rhs.started;
    tickTimeSet = rhs.tickTimeSet;
    toSeeIfError = rhs.toSeeIfError;
    tLast = rhs.tLast;
    /* NOTE THAT THERE IS NO NEED FOR THIS. IT SHOULD ALWAYS BE THE SAME AND IS
       ALWAYS ALREADY DEFINED.
       scaleFactor = rhs.scaleFactor;
     */
    return *this;
  }

  
  void start()
  {
    started = true;
    tLast = std::chrono::steady_clock::now();
  }

  
  void start(std::__1::chrono::steady_clock::time_point startTime)
  {
    started = true;
    tLast = startTime;
  }


  /* Can be used to get the last time of a master chronological object so
     that other chronological object may be fully initialised with the same
     start time (using start()). */
  std::__1::chrono::steady_clock::time_point getLastTime() const
  {
    checkIfFullyInitialised("getCurrentTime()");
    
    return tLast;
  }


  /* Returns the delta between the current time and the last time
     tLast was reset (in seconds). */
  double getDeltaSinceLastReset() const
  {
    checkIfPartiallyInitialised("getDeltaSinceLastReset()");
    
    std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::steady_clock::now()};
    return (duration_cast<std::chrono::duration<long double>>
	    (currentTime - tLast)).count();
  }

  /* Used when updating can be done at any time and is not based on a specific
     tick time. I.e. don't mix with startNextTick()! */
  void resetTimer()
  {
    checkIfPartiallyInitialised("resetTimer()");
    
    tLast = {std::chrono::steady_clock::now()};
  }
  
  
  bool startNextTick()
  {
    checkIfFullyInitialised("startNextTick()");

    std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::steady_clock::now()};
  if((duration_cast<std::chrono::duration<long double>>
      (currentTime - tLast)).count() >= (tickTime / scaleFactor))
    {
      tLast = currentTime;
      return true;
    }
  else
    {
      return false;
    }
  }

  /* If it is time to start the next tick timeElapsed is set to the time that had
     elapsed since that last time a tick was started (in milliseconds).  */
  bool startNextTick(double & timeElapsed)
  {
    checkIfFullyInitialised("startNextTick(double & timeElapsed)");

    std::__1::chrono::steady_clock::time_point currentTime
    {std::chrono::steady_clock::now()};
  if((duration_cast<std::chrono::duration<long double>>
      (currentTime - tLast)).count() >= (tickTime / scaleFactor))
    {
      timeElapsed =
	(duration_cast<std::chrono::duration<long double>>
	 (currentTime - tLast)).count() / scaleFactor;
      tLast = currentTime;
      return true;
    }
  else
    {
      return false;
    }
  }
};


/* This struct contains some main timers for the game (note that timers should
   probably not be nested.) */
struct timers
{
  chronological movePlayer {};
  chronological drawTime {};
};


#endif
