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
private:
  bool started {false};
  /* const */ bool tickTimeSet {false};
  /* const */ std::string toSeeIfError;
  std::__1::chrono::steady_clock::time_point tLast;
  const long double scaleFactor {1000};


  /* Most if not every public function belonging to this object (apart from any
     constructor and start() must call this function as their first action. */
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
    else if(!started)
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
};


/* This struct contains all timers for the game (note that timers should not be
   nested.) */
struct timers
{
  chronological movePlayer {};
  chronological drawTime {};
};


#endif
