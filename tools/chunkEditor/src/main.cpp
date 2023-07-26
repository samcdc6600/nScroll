#include "include/initCurses.hpp"
#include "include/utils.hpp"


constexpr int printCharSpeed {35};
constexpr int afterPrintSleep {900};


/* The program should take either the name of a background chunk file and the
   name of a rules chunk file or it should take the name of a background chunk
   file and a background file or it should take the name of a rules chunk file
   and a rules file. */
void verifyCmdArgs(const int argc, const char * argv []);
void printHelp(const char * argv []);
void readInChunkFile();


int main(const int argc, const char * argv [])
{
  /* Holds the window size (currently (should hole the view port size in the
     future.) */
  yx viewPortSize;
  // Start and setup ncurses.
  std::vector<int> pairIndexes {initialiseCurses(viewPortSize)};

  verifyCmdArgs(argc, argv);

  int bgChunk [yHeight][xWidth] {};
  char rulesChunk [yHeight][xWidth] {};

  // std::ofstream bgChunkFile
  printMessage("Welcome to the chunk editor.", viewPortSize, printCharSpeed,
	       afterPrintSleep);

  // TODO
 // if()
 //    {
 //  readInChunkFile();
 //    }


  endwin();
  
  return 0;
}


void verifyCmdArgs(const int argc, const char * argv [])
{
  /* If argc == argNumForModes_1_and_2 check for form like:
     chunked x.bgchunk x.crchunk
     else check for form like
     chunked x.bgchunk x.background.lev
     (and again for rules.)
     
     If argc == argNumForMode_3 check for form like:
     chunked x.background.lev x.bgchunk y x
     (and again for rules.)

     If argc == argNumForMode_4 check for form like:
     chunked x.background.lev
     (and again for rules.) */
  /* Print help message and exit if arguments in argv don't correspond to
     either mode 1 or mode 2. */
  auto testMode1And2Args = [argv]() -> void
  {
    bool found {false};
    if(checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION) &&
       checkForPostfix(argv[2], COORD_RULES_CHUNK_FILE_EXTENSION))
      {
	found = true;
      }
    else if(checkForPostfix(argv[1], BACKGROUND_CHUNK_FILE_EXTENSION) &&
	    checkForPostfix(argv[2], BACKGROUND_FILE_EXTENSION))
      {
	found = true;
      }
    else if(checkForPostfix(argv[1], COORD_RULES_CHUNK_FILE_EXTENSION) &&
	    checkForPostfix(argv[2], COORD_RULES_FILE_EXTENSION))
      {
	found = true;
      }
    if(!found)
      {
	// Argument values don't correspond to mode 1 or 2.
	printHelp(argv);
	exit(ERROR_INVALID_CMD_ARGS);
      }
  };


  /* Print help message and exit if arguments in argv don't correspond to mode
     3. */
  auto testMode3Args = [argv]() -> void
  {
    if((!checkForPostfix(argv[1], BACKGROUND_FILE_EXTENSION) ||
	!checkForPostfix(argv[2], BACKGROUND_CHUNK_FILE_EXTENSION)) &&
       (!checkForPostfix(argv[1], COORD_RULES_FILE_EXTENSION) ||
	!checkForPostfix(argv[2], COORD_RULES_CHUNK_FILE_EXTENSION)))
      {
	// Argument number corresponds to mode 3, but values don't.
	printHelp(argv);
	exit(ERROR_INVALID_CMD_ARGS);
      }
    else
      {
	// Argument number corresponds to mode 3, but values don't.
	const std::string yArg {argv[3]}, xArg {argv[4]};
	auto yIter {yArg.begin()}, xIter {xArg.begin()};
	/* This isn't the best solution as printHelp() will not be called and also
	   there can be non number characters after a number, but for now it will
	   be fine. ReadSingleNum() will print an error and exit message if no
	   number is found. */
	readSingleNum(yArg, yIter, "trying to read 3rd argument (chunk y "
		      "position)", true);
	readSingleNum(xArg, xIter, "trying to read 4th argument (chunk x "
		      "position)", true);
      }
  };
      
  
  constexpr int argNumForModes_1_and_2 {3}, argNumForMode_3 {5},
    argNumForMode_4 {2};

  if(argc != argNumForModes_1_and_2 && argc != argNumForMode_3 &&
     argc != argNumForMode_4)
    {
      // Incorrect number of argument supplied.
      printHelp(argv);
      exit(ERROR_INVALID_CMD_ARGS);
    }
  else if(argc == argNumForModes_1_and_2)
    {
      testMode1And2Args();
    }
  else if(argc == argNumForMode_3)
    {
      testMode3Args();
    }
  else if(argc == argNumForMode_4 &&
	  (!checkForPostfix(argv[1], BACKGROUND_FILE_EXTENSION) &&
	   !checkForPostfix(argv[1], COORD_RULES_FILE_EXTENSION)))
    {
      // Argument number corresponds to mode 4, but value doesn't.
      printHelp(argv);
      exit(ERROR_INVALID_CMD_ARGS);
    }
}


void printHelp(const char * argv [])
{
  std::string help
    {
      concat(
          "", argv[0], " Help:\n\t", argv[0],
          " requires two arguments. Both "
          "arguments must be valid file\n\tpaths. ",
          argv[0],
          " has four "
          "modes depending on the arguments."
          "\n\n    1st Mode:\n\tIf the files have the extensions \"",
          BACKGROUND_CHUNK_FILE_EXTENSION, "\" and \"",
          COORD_RULES_CHUNK_FILE_EXTENSION,
          "\"\n\trespectively "
          "and the paths are valid. The program will enter chunk edit\n\t"
          "mode. In this mode the background chunk file and the rules chunk "
          "file\n\tcan be edited and saved. The files can be pre-existing "
          "files or they can\n\tbe new files. If the files are pre-existing "
          "their pre-existing contents\n\twill be editable. If the files are "
          "new the user will be asked to supply\n\ta chunk coordinate for the"
          " files (If just one of the files is new the\n\tchunk coordinate "
          "will be read from the pre-existing file.)\n\t    Example:"
	  "\n\t\t", argv[0], " superGood", BACKGROUND_CHUNK_FILE_EXTENSION,
	  " superGood", COORD_RULES_CHUNK_FILE_EXTENSION,
	  "\n\n    2nd Mode:"
          "\n\tThe second mode is "
          "known as append mode. In this mode the contents of a \n\t\"",
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
          "    3rd Mode:\n\tThis mode is known as extraction mode. In this "
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
	  BACKGROUND_CHUNK_FILE_EXTENSION, " 11, -3"
	  "\n\n"
	  "    4th Mode:\n\tThis mode is known as map view mode. In this mode "
	  "a ", BACKGROUND_FILE_EXTENSION, "\n\tfile or a ",
	  COORD_RULES_FILE_EXTENSION, " file can be viewed in a zoomed out "
	  "mammer,\n\twith one character corresponding to 1 chunk. If the "
	  "layout is not\n\tviewable in this way within the view port. The "
	  "user may use the arrow\n\tkeys to move around. To enter this mode "
	  "the user must supply a file as\n\tthe soul argument. The file must "
	  "either be a ", BACKGROUND_FILE_EXTENSION, " file. Or a\n\t",
	  COORD_RULES_FILE_EXTENSION, " file.\n\t    Example:\n\t\t", argv[0],
	  " splendid", BACKGROUND_FILE_EXTENSION)};
  endwin();
  printMessageNoWin(help, printCharSpeed / 20, afterPrintSleep);
}


void readInChunkFile()
{
}
