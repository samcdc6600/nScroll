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
    /* T holds the last updated time for this object, tLast holds the time before
     that and tickTime holds the time delta at which an event should take
     place. */
  long t, tLast;
  /* const */ long double scaleFactor {0.001};


  /* Most if not every public function belonging to this object (apart from any
     constructor and start() must call this function as their first action. */
  void checkIfFullyInitialised(std::string funcName)
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
  
  
  long double getDelta()
  {
    t = time(nullptr);
    return (long double)(t - tLast) / scaleFactor;
  }


  void update()
  { 
    tLast = t;
    t = time(nullptr);
  }

  
public:
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
    /* T holds the last updated time for this object, tLast holds the time before
     that and tickTime holds the time delta at which an event should take
     place. */
    t = rhs.t;
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
    t = time(nullptr);
    tLast = t;
  }

  void start(long startTime)
  {
    started = true;
    t = startTime;
    tLast = t;
  }

  long getCurrentTime()
  {
    checkIfFullyInitialised("getCurrentTime()");
    
    return t;
  }
  
  
  bool startNextTick()
  {
    checkIfFullyInitialised("startNextTick()");
	
    if(getDelta() > tickTime)
      {
	update();
	return true;
      }
    else
      {
	return false;
      }
  }
};


// This struct conatins all timers for the game.
struct timers
{
  chronological allPhysics {};
};


#endif
