#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include "include/editMode.hpp"
#include "include/appendExtractDelete.hpp"


/* Verifies that the CMD args in argv conform to one of the modes (see the
   function definition) and then returns the mode they conform to. If they are
   found not to conform to a mode printHelp() will be called and the program
   will be terminated. */
int verifyCmdArgsAndGetMode(const int argc, const char * argv []);
void printHelp(const char *argv[]);
void enterMode(const int mode, const char * argv [], const yx viewPortSize);


int main(const int argc, const char * argv [])
{
  /* Holds the window size (currently (should hole the view port size in the
     future.) */
  yx viewPortSize;
  // Start and setup ncurses.
  initialiseCurses(viewPortSize);
  const int mode {verifyCmdArgsAndGetMode(argc, argv)};

  progressivePrintMessage
    ("Welcome to the sprite editor.", viewPortSize, printCharSpeed,
     afterPrintSleep);
  enterMode(mode, argv, viewPortSize);
    
  endwin();
  
  return 0;
}


int verifyCmdArgsAndGetMode(const int argc, const char * argv [])
{
  int modeFound {};

  auto testMode1Args = [argv, & modeFound]() -> bool
  {
    bool found {false};
    if(checkForPostfix(argv[1], SPRITE_FILE_EXTENSION))
      {
	found = true;
	modeFound = 1;
      }
    return found;
  };
      
  constexpr int argNumForModes_1 {2};
  bool argsValid {false};
  
  switch(argc)
    {
    case argNumForModes_1:
      argsValid = testMode1Args();
      break;
    }
  if(!argsValid)
    {
      printHelp(argv);
      exit(ERROR_INVALID_CMD_ARGS);
    }

  return modeFound;
}


void printHelp(const char * argv [])
{
  std::string help
    {
      concat(
          "===================================================================="
          "============\n",
          argv[0], " Help:\n\t",
          argv[0],
          " has seven modes. They are as follows."
          "\n\n    1st mode:"
	  "\n\tThe first mode is known as Sprite Edit Mode. If the file has the"
	  "\n\textension \"", SPRITE_FILE_EXTENSION, "\" and the path is valid "
	  "the program will enter Sprite"
	  "\n\tEdit Mode."
	  "\n\t    Example:"
	  "\n\t\t",
          argv[0], " superGood", SPRITE_FILE_EXTENSION,
	  "\n")};
  endwin();
  printMessageNoWin(help, printCharSpeed, afterPrintSleep);
}


void enterMode(const int mode, const char * argv [], const yx viewPortSize)
{  
  switch(mode)
    {
    case 1:
      // Sprite Edit Mode.
      progressivePrintMessage
	("Starting Sprite Edit Mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      editMode(argv[1], viewPortSize);
      progressivePrintMessage
	("Finished editing.", viewPortSize, printCharSpeed, afterPrintSleep);
      break;
    default:
      exit(concat("Error: reached default case statement in enterMode(). Where,"
		  " mode = ", mode, ". Note that this is a program logic "
		  "error."), ERROR_BAD_LOGIC);
      break;
    }

  progressivePrintMessage
    ("Farewell", viewPortSize, printCharSpeed,
     afterPrintSleep);
}
