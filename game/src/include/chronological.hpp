#ifndef TIME_HPP_
#define TIME_HPP_


#include <ctime>
#include "utils.hpp"


class chronological
{
  /* NOTE THAT CONST VARIABLES APPARENTLY CANNOT BE CHANGED WITHIN THE BODY OF
     AN ASSIGNMENT OPERACTOR DEFINED BY AN OBJECT. AS SUCH WE HAVE COMMENTED OUT
     THE CONST QUALIFIER ON A NUMBER OF VARIABLES WE WOULD LIKE TO BE
     CONST. THESE VARIABLES OF COURSE SHOULD NOT BE CHANGE ANYWHERE THEY
     WOULDN'T BE ALLOWED TO BE CHANGED IF THEY WERE CONST, OF COURSE WITH THE
     EXCEPTION OF THE BODY OF THE ASSIGNMENT OPERATOR. */
public:
  /* TickTime holds the time delta (in milliseconds) at which an event should
     take place. */
  /* const */ long double tickTime;
  
private:
  bool started {false};
  /* const */ std::string toSeeIfError;
  /* const */ bool tickTimeSet {false};
  /* NOTE THAT USING steady_clock WILL RESULT IN THE PROGRAM TIME BEING USED.
     WHERE AS USING system_clock WILL RESULT IN SYSTEM TIME BEING USED. We
     wan't to use the system_time this is because it will always be closer to
     the real time. */
  std::__1::chrono::system_clock::time_point tLast;
  const long double scaleFactor {1};


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
  chronological():
    tickTime(0), toSeeIfError("")
  {
  }
  
  
  chronological(const long double tTime, const std::string & ifError):
    tickTime(tTime), toSeeIfError(ifError), tickTimeSet(true)
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
    tLast = std::chrono::system_clock::now();
  }

  
  void start(std::__1::chrono::system_clock::time_point startTime)
  {
    started = true;
    tLast = startTime;
  }


  /* Can be used to get the last time of a master chronological object so
     that other chronological object may be fully initialised with the same
     start time (using start()). */
  std::__1::chrono::system_clock::time_point getLastTime() const
  {
    checkIfFullyInitialised("getCurrentTime()");
    
    return tLast;
  }
  
  
  bool startNextTick()
  {    
    checkIfFullyInitialised("startNextTick()");

    std::__1::chrono::system_clock::time_point currentTime
    {std::chrono::system_clock::now()};
    
  if((duration_cast<std::chrono::duration<long double>>
      (currentTime - tLast)).count() >= (tickTime / scaleFactor))
    {
      static long callCount {};
      callCount++;
      if(callCount > 7000)
	std::cout<<"callCount = "<<callCount<<'\n';

      std::cout<<(duration_cast<std::chrono::duration<long double>>
		  (currentTime - tLast)).count()<<'\t'<<(tickTime / scaleFactor)<<'\n';

    
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

    std::__1::chrono::system_clock::time_point currentTime
    {std::chrono::system_clock::now()};
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


  static constexpr double milliSecToSec(const double a) {return a / 1000;}
};


/* This struct contains some main timers for the game (note that timers should
   probably not be nested.) */
struct timers
{
  /* physics should always be the same as draw time or some multiple slower. */
  chronological physics {};
  chronological drawTime {};
};


#endif
