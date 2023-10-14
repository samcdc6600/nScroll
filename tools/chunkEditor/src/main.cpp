#include "include/initCurses.hpp"
#include "include/utils.hpp"
#include "include/editMode.hpp"
#include "include/appendExtractDelete.hpp"
#include "include/mapView.hpp"


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
    ("Welcome to the chunk editor.", viewPortSize, printCharSpeed,
     afterPrintSleep);
  enterMode(mode, argv, viewPortSize);
    
  endwin();
  
  return 0;
}


int verifyCmdArgsAndGetMode(const int argc, const char * argv [])
{
  int modeFound {};
  /* If argc == argNumForModes_1_and_3 check for form like:
     chunked x.bgchunk x.crchunk
     else check for form like
     chunked x.bgchunk x.background.lev
     (and again for rules.)
     
     If argc == argNumForMode_4 check for form like:
     chunked x.background.lev x.bgchunk y x
     (and again for rules.)

     If argc == argNumForMode_6 check for form like:
     chunked x.background.lev
     (and again for rules.)
     Or check for -h for the 2nd arg. */
  /* Print help message and exit if arguments in argv don't correspond to
     either mode 1 or mode 3. */
  auto testMode1And3Args = [argv, & modeFound]() -> void
  {
    bool found {false};
    if(checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION) &&
       checkForPostfix(argv[2], COORD_RULES_CHUNK_FILE_EXTENSION))
      {
	found = true;
	modeFound = 1;
      }
    else if(checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION) &&
	    checkForPostfix(argv[2], BACKGROUND_FILE_EXTENSION))
      {
	found = true;
	modeFound = 3;
      }
    else if(checkForPostfix(argv[1], COORD_RULES_CHUNK_FILE_EXTENSION) &&
	    checkForPostfix(argv[2], COORD_RULES_FILE_EXTENSION))
      {
	found = true;
	modeFound = 3;
      }
    if(!found)
      {
	// Argument values don't correspond to mode 1 or 2.
	printHelp(argv);
	exit(ERROR_INVALID_CMD_ARGS);
      }
  };


  /* Print help message and exit if arguments in argv don't correspond to mode
     4. */
  auto testMode2And4Args = [argv, & modeFound]() -> void
  {
    if(checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION) &&
       checkForPostfix(argv[2], BACKGROUND_CHUNK_FILE_EXTENSION) &&
       checkForPostfix(argv[3], COORD_RULES_CHUNK_FILE_EXTENSION) &&
       checkForPostfix(argv[4], COORD_RULES_CHUNK_FILE_EXTENSION))
      {
	modeFound = 2;
      }
    else if((checkForPostfix(argv[1], BACKGROUND_FILE_EXTENSION) &&
	checkForPostfix(argv[2], BACKGROUND_CHUNK_FILE_EXTENSION)) ||
       (checkForPostfix(argv[1], COORD_RULES_FILE_EXTENSION) &&
	checkForPostfix(argv[2], COORD_RULES_CHUNK_FILE_EXTENSION)))
      {
	const std::string yArg {argv[3]}, xArg {argv[4]};
	auto yIter {yArg.begin()}, xIter {xArg.begin()};
	/* This isn't the best solution as printHelp() will not be called and
	   also there can be non number characters after a number, but for now
	   it will be fine. ReadSingleNum() will print an error and exit message
	   if no number is found. */
	readSingleNum(yArg, yIter, "trying to read 3rd argument (chunk y "
		      "position)", true);
	readSingleNum(xArg, xIter, "trying to read 4th argument (chunk x "
		      "position)", true);
	modeFound = 4;
      }
    else
      {
	// Argument number corresponds to mode 2 or 4, but values don't.
	printHelp(argv);
	exit(ERROR_INVALID_CMD_ARGS);
      }
  };


  auto testMode5Args = [argv, & modeFound]() -> void
  {
    if(checkForPostfix(argv[1], BACKGROUND_FILE_EXTENSION) ||
       checkForPostfix(argv[1], COORD_RULES_FILE_EXTENSION))
      {
	const std::string yArg {argv[2]}, xArg {argv[3]};
	auto yIter {yArg.begin()}, xIter {xArg.begin()};
	/* This isn't the best solution as printHelp() will not be called and
	   also there can be non number characters after a number, but for now
	   it will be fine. ReadSingleNum() will print an error and exit message
	   if no number is found. */
	readSingleNum(yArg, yIter, "trying to read 2nd argument (chunk y "
		      "position)", true);
	readSingleNum(xArg, xIter, "trying to read 3rd argument (chunk x "
		      "position)", true);
	modeFound = 5;
      }
    else
      {
	// Argument number corresponds to mode 5, but values don't.
	printHelp(argv);
	exit(ERROR_INVALID_CMD_ARGS);
      }
  };
      
  
  constexpr int argNumForModes_1_and_3 {3}, argNumForModes_2_and_4 {5},
    argNumForMode_5 {4}, argNumForMode_6 {2};

  switch(argc)
    {
    case argNumForModes_1_and_3:
      testMode1And3Args();
      break;
    case argNumForModes_2_and_4:
      testMode2And4Args();
      break;
    case argNumForMode_5:
      testMode5Args();
      break;
    case argNumForMode_6:
      if((!checkForPostfix(argv[1], BACKGROUND_FILE_EXTENSION) &&
	  !checkForPostfix(argv[1], COORD_RULES_FILE_EXTENSION)))
	{
	  // Argument number corresponds to mode 6, but value doesn't.
	  printHelp(argv);
	  exit(ERROR_INVALID_CMD_ARGS);
	}
      else
	{
	  modeFound = 6;
	}
      break;
    default:
      // Incorrect number of argument supplied.
      printHelp(argv);
      exit(ERROR_INVALID_CMD_ARGS);
      break;
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
          "\n\n    1st mode:\n\tThe first mode is known as Single Chunk Edit "
          "Mode. If the files have the\n\textensions \"",
          BACKGROUND_CHUNK_FILE_EXTENSION, "\" and \"",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          "\" respectively and the paths are\n\tvalid. The program will enter "
          "Chunk Edit Mode. In this mode the\n\tbackground chunk file and the "
          "rules chunk file can be edited and saved.\n\tThe files can be "
          "pre-existing files or they can be new files. If the\n\tfiles are "
          "pre-existing their pre-existing contents will be editable. If\n\tthe"
          " files are new the user will be asked to supply a chunk coordinate"
          "\n\tfor the files (If just one of the files is new the chunk "
          "coordinate will\n\tbe read from the pre-existing file.) The "
          "coordinate of the chunks can\n\talso be changed in this mode."
          "\n\t    Example:"
          "\n\t\t",
          argv[0], " superGood", BACKGROUND_CHUNK_FILE_EXTENSION, " superGood",
          COORD_RULES_CHUNK_FILE_EXTENSION,

          "\n\n    2nd mode:"
          "\n\tThe second mode is an alternative form of Single Chunk Edit "
          "mode. \n\tInstead of supplying the names of one \"",
          BACKGROUND_CHUNK_FILE_EXTENSION, "\" file and one \"",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          "\"\n\tfile the names of two \"",
          BACKGROUND_CHUNK_FILE_EXTENSION, "\" and two \"",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          "\" files must be\n\tsupplied. This second set of files will be "
	  "viewable as a reference in\n\tthe editor when editing the first set "
	  "of file. Where the second set are\n\tthe files that appear after a "
	  "file with the same extension in the\n\tcommand."
	  "\n\t    Example:\n\t\t",
	  argv[0], " superGood", BACKGROUND_CHUNK_FILE_EXTENSION,
	  " superGoodReference", BACKGROUND_CHUNK_FILE_EXTENSION, " superGood",
          COORD_RULES_CHUNK_FILE_EXTENSION, " superGoodReference",
          COORD_RULES_CHUNK_FILE_EXTENSION,
	  "\n\n    3rd mode:"
          "\n\tThe third mode is "
          "known as Append Mode. In this mode the contents of a \n\t\"",
          BACKGROUND_CHUNK_FILE_EXTENSION,
          "\" file will be appended to a "
          "\"",
          BACKGROUND_FILE_EXTENSION,
          "\" file.\n\tOr alternatively the "
          "contents of a \"",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          "\" file will"
          "be appended to a\n\t\"",
          COORD_RULES_FILE_EXTENSION,
          "\" file.\n\t"
          "Which action is performed depends of the file extensions of the"
          "\n\targuments. If the file extensions of the arguments are \"",
          BACKGROUND_CHUNK_FILE_EXTENSION, "\" and\n\t\"",
          BACKGROUND_FILE_EXTENSION,
          "\" respectively then "
          "the first action will be performed.\n\tHowever if the file "
          "extensions of the arguments are \"",
          COORD_RULES_CHUNK_FILE_EXTENSION, "\" and\n\t\"",
          COORD_RULES_FILE_EXTENSION,
          "\" respectively then the second action"
          " will be performed.\n\t    Example:\n\t\t", argv[0],
	  " superGood", BACKGROUND_CHUNK_FILE_EXTENSION,
	  " splendid", BACKGROUND_FILE_EXTENSION,
	  "\n\n"
          "    4th mode:\n\tThis mode is known as Extraction Mode. In this "
          "mode a chunk is removed\n\tfrom a ",
          BACKGROUND_FILE_EXTENSION, " file or a ", COORD_RULES_FILE_EXTENSION,
          " file and placed in a\n\t", BACKGROUND_CHUNK_FILE_EXTENSION,
          " file or a ", COORD_RULES_CHUNK_FILE_EXTENSION, " file. A ",
          BACKGROUND_FILE_EXTENSION,
          " file must be\n\tspecified as the first "
          "argument. This should be followed by a ",
          BACKGROUND_CHUNK_FILE_EXTENSION,
          "\n\tfile and finally by the desired y and x coordinate of the "
	  "chunk to be\n\textracted.\n\tOr alternatively a ",
	  COORD_RULES_FILE_EXTENSION,
          " file must be specified as the first\n\t"
          "argument. This should be followed by a ",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          " file and finally by the\n\tdesired y and x coordinate of the "
	  "chunk to be extracted.\n\t    Example:\n\t\t", argv[0],
	  " splendid", BACKGROUND_FILE_EXTENSION, " superGood",
	  BACKGROUND_CHUNK_FILE_EXTENSION, " 11 -3"
	  "\n\n"
	  "    5th mode:\n\tThis mode is known as Delete Mode. Chunks can be "
	  "removed from \"", BACKGROUND_FILE_EXTENSION, "\"\n\tand \"",
	  COORD_RULES_FILE_EXTENSION, "\" files. A valid chunk coordinate for "
	  "a chunk to be removed\n\tshould be supplied."
	  "\n\t    Example:\n\t\t", argv[0], " splendid",
	  BACKGROUND_FILE_EXTENSION, " 11 -3"
	  "\n\n"
	  "    6th mode:\n\tThis mode is known as map View Mode. In this mode "
	  "a ", BACKGROUND_FILE_EXTENSION, "\n\tfile or a ",
	  COORD_RULES_FILE_EXTENSION, " file can be viewed in a zoomed out "
	  "mammer,\n\twith one character corresponding to 1 chunk. If the "
	  "layout is not\n\tviewable in this way within the view port. The "
	  "user may use the arrow\n\tkeys to move around. To enter this mode "
	  "the user must supply a file as\n\tthe soul argument. The file must "
	  "either be a ", BACKGROUND_FILE_EXTENSION, " file. Or a\n\t",
	  COORD_RULES_FILE_EXTENSION, " file.\n\t    Example:\n\t\t", argv[0],
	  " splendid", BACKGROUND_FILE_EXTENSION,
	  "\n\n"
	  "    7th mode:\n\tThis mode is known as Help Mode and does what it "
	  "says on the tin."
	  "\n\t	Example:\n\t\t", argv[0], " -h"
	  "\n")};
  endwin();
  printMessageNoWin(help, printCharSpeed, afterPrintSleep);
}


void enterMode(const int mode, const char * argv [], const yx viewPortSize)
{  
  switch(mode)
    {
    case 1:
      // Chunk edit mode.
      progressivePrintMessage
	("Starting Single Chunk Edit Mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      editMode(argv[1], argv[2], "", "", viewPortSize, false);
      progressivePrintMessage
	("Finished editing.", viewPortSize, printCharSpeed, afterPrintSleep);
      break;
    case 2:
      // Chunk edit mode with reference chunks.
      progressivePrintMessage
	("Starting Single Chunk Edit Mode with reference chunks.",
	 viewPortSize, printCharSpeed,
	 afterPrintSleep);
      editMode(argv[1], argv[3], argv[2], argv[4], viewPortSize, true);
      progressivePrintMessage
	("Finished editing.", viewPortSize, printCharSpeed, afterPrintSleep);
      break;
    case 3:
      // Append mode.
      progressivePrintMessage
	("Starting Append mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      append(argv[1], argv[2], viewPortSize);
      progressivePrintMessage
	("Finished appending.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      break;
    case 4:
      // Extraction mode.
      progressivePrintMessage
	("Starting Extraction Mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      extract(argv[1], argv[2], argv[3], argv[4], viewPortSize);
      progressivePrintMessage
	("Finished extracting.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      break;
    case 5:
      // Delete mode.
      progressivePrintMessage
	("Starting Delete Mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      deleteChunk(argv[1], argv[2], argv[3], viewPortSize);
      progressivePrintMessage
	("Finished deleting.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      break;
    case 6:
      // Map view mode.
      refresh();
      progressivePrintMessage
	("Starting Map View Mode.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
      mapView(argv[1], viewPortSize);
            progressivePrintMessage
	("Finished map viewing.", viewPortSize, printCharSpeed,
	 afterPrintSleep);
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
