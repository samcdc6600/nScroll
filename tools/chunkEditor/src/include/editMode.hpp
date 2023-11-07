#ifndef EDITMODE_HPP_
#define EDITMODE_HPP_


#include "utils.hpp"
#include "colorS.hpp"


namespace editingSettings
{
  constexpr int loopSleepTimeMs {5};
  /* Sleep for this time when exiting a sub menu so that the character the user
     is pressing isn't read in as an input to the "screen" "above" the
     menu. This is especially important as the character my be editChars::quit.
  */
  constexpr int editSubMenuSleepTimeMs {subMenuSleepTimeMs};
  constexpr std::chrono::milliseconds cursorBlinkTimeMs {700};
  constexpr std::chrono::milliseconds showLastChunkAfterUpdateIndexFor {2500};
  constexpr int afterFileErrorPrintSleep {3500};
  constexpr int afterIncorrectCoordInputSleep {4000};
  constexpr int afterGeneralMessageSleep {1750};
  /* If the cursor crosshair is turned on it will change to a new random color
     at intervals of this length. */
  constexpr std::chrono::milliseconds crosshairTransitionTimeMs {2250};
  /* Time that the currently selected region will be showed for in millisecond
     when selecting a new region. */
  constexpr std::chrono::milliseconds dispSelectionSelectingTimeMs {475};
  /* Time that the current selection number will be displayed for after
     changing the current selection. */
  constexpr std::chrono::milliseconds displaySelectionNoForMs
    {showLastChunkAfterUpdateIndexFor};
  extern setColorMode colorMode;
  // Color used for help menu.
  constexpr int helpColor {helpColorPair};
  /* This color pair is used when a character is not yet in the position in
     question. */
  constexpr int invalidCharColorPair {bgRunLengthSequenceSignifier -1};
  constexpr int blackBgColorPair {15};
  constexpr char initialCursorChar {' ' - lowerUnusedASCIIChNum};
  /* TODO: think of better name for this and also maybe replace es to
     help char with it. */
  constexpr int validColorNumber {helpColor};
  constexpr int bgCharsRingBufferSize {24};
  constexpr int selectionRingBufferSize {24};
  
  /* See editMode.cpp for
     setColorMode colorMode {colorParams::defaultColorPair}; */
  namespace editChars
  {
    /* Where a cR prefix means that the command should be specific to choord
       rules editing mode and a prefix of br means that the command should be
       specific to background editing mode. */
    constexpr char quit			{'q'};
    constexpr char cursorUp		{'w'};
    constexpr char cursorDown		{'s'};
    constexpr char cursorLeft		{'a'};
    constexpr char cursorRight		{'d'};
    constexpr char performActionAtPos	{' '};
    constexpr char toggleBetweenCRandBg	{'t'};
    constexpr char toggleReferenceChunkView	{'R'};
    constexpr char cREraseCRChar       	{'e'};
    constexpr char cRSetCRCharToBorder	{'b'};
    constexpr char cRSetCRCharToPlatform	{'p'};
    constexpr char bGToggleCharacterSelection	{'c'};
    constexpr char bgNextCurrentChar	{'n'};
    constexpr char bgLastCurrentChar	{'l'};
    constexpr char redo			{'r'};
    constexpr char undo			{'u'};
    constexpr char bgGetCharAtPos      	{'g'};
    /* In line draw mode the current bg / cr character is set to the current
       cursor char (this way the user can draw lines without having to
       constantly hit performActionAtPos. */
    constexpr char toggleLineDrawMode  	{'L'};
    constexpr char floodFill		{'f'};
    constexpr char selectSelection     	{'S'};
    constexpr char pasteSelection	{'P'};
    /* Rotate selection by 90 degrees. */
    constexpr char rotateSelection	{'r'};
    constexpr char mirrorSelection	{'m'};
    constexpr char nextSelection	{'n'};
    constexpr char lastSelection	{'l'};
    // 'i' for info.
    constexpr char bgShowUnsetChars	{'i'};
    constexpr char toggleCrosshairCursor	{'k'};
    constexpr char changeCoordinates    {'C'};
    constexpr char saveChunks		{'o'};
    constexpr char toggleHelpMsg       	{'h'};
  };

  namespace rulesChars
  {
    constexpr char nullRule	{' '};
    constexpr char boarder	{'b'};
    constexpr char platform	{'p'};
  }

  namespace helpMsgs
  {
    const std::string mainViewingBg
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn the main editing mode (viewing a background chunk.)\t\t\t\t\t",
	editChars::toggleHelpMsg,	": to toggle this message.\t\t\t",
	editChars::cursorUp,		": to move the cursor up.\t\t\t",
	editChars::cursorDown,	": to move the cursor down.\t\t\t",
	editChars::cursorLeft,	": to move the cursor left.\t\t\t",
	editChars::cursorRight,	": to move the cursor right.\t\t\t",
	"\"", editChars::performActionAtPos, "\": to perform an action at the current "
	"cursor position. The specific action performed depends on the context. "
	"The most important action is to change the character under the cursor "
	"to the same character as the cursor.\t\t\t",
	editChars::toggleBetweenCRandBg, ": to toggle between background and character "
	"rules chunks.\t\t\t",
	editChars::toggleReferenceChunkView, ": to toggle the reference chunk view (this is "
	"only relevant when in mode 2, see -h for more info.)\t\t\t",
	editChars::bGToggleCharacterSelection, ": to select a new character color and "
	"character for the cursor character.\t\t\t",
	editChars::bgNextCurrentChar,	": to cycle forward (next) through the recent "
	"cursor characters.\t\t\t",
	editChars::bgLastCurrentChar, ": to cycle backwards (last) through the recent "
	"cursor characters.\t\t\t",
	editChars::redo,		": to cycle the chunk buffer position forward (redo). ",
	"This cycles forward through the chunk buffer. Every time a chunk is "
	"altered the chunk at the current position in the buffer is unchanged "
	"and a copy of that chunk is saved in the next buffer position where the "
	"change is made. The current position is updated to the position of this "
	"new altered copy.\t\t\t",
	editChars::undo,		": to cycle the chunk buffer position back (undo). "
	"This cycles back through the chunk buffer. Every time a chunk is "
	"altered the chunk at the current position in the buffer is unchanged "
	"and a copy of that chunk is saved in the next buffer position where the "
	"change is made. The current position is updated to the position of this "
	"new altered copy.\t\t\t",
	editChars::bgGetCharAtPos,	": to set the cursor character to the background chunk "
	"character below it. This is only applicable if the character at the "
	"cursor pos has already been set.\t\t\t",
	editChars::toggleLineDrawMode, ": to toggle line drawing. When line drawing is "
	"turned on any character the user moves the cursor over will be set to "
	"the current cursor character. This way lines can be drawn without "
	"having to constantly press \"", editChars::performActionAtPos, "\".\t\t\t",
	editChars::floodFill,	": to do a flood fill using the cursor character and "
	"starting at the current cursor pos.\t\t\t",
	editChars::selectSelection,	": to enter selection mode, where an area (selection) "
	"can be selected for copying and later pasting.\t\t\t",
	editChars::pasteSelection,	": to enter past mode, where ",
	selectionRingBufferSize, " of the last selections made can "
	"be pasted.\t\t\t",
	editChars::bgShowUnsetChars,	": to print noise in locations where characters aren't "
	"set. This helps when trying to find unset background chunk characters. "
	"Note that the chunks cannot be saved if any of the background chunk "
	"characters are unset. Also note that when editing a new background "
	"chunk file all characters are initially unset.\t\t\t",
	editChars::toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
	editChars::changeCoordinates, ": to change coordinates. This changes the "
	"coordinates of both the background chunk and rules character chunk as "
	"they are a set.\t\t\t",
	editChars::saveChunks,	": to save (output) both chunks.\t\t\t",
	editChars::quit,		": to quite the program.")};
    const std::string mainViewingCR
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn the main editing mode (viewing a character rules chunk.)\t\t\t\t\t",
	editChars::toggleHelpMsg,	": to toggle this message.\t\t\t",
	editChars::cursorUp,		": to move the cursor up.\t\t\t",
	editChars::cursorDown,	": to move the cursor down.\t\t\t",
	editChars::cursorLeft,	": to move the cursor left.\t\t\t",
	editChars::cursorRight,	": to move the cursor right.\t\t\t",
	"\"", editChars::performActionAtPos, "\": to perform an action at the current "
	"cursor position. The specific action performed depends on the context. "
	"The most important action is to change the character under the cursor "
	"to the same character as the cursor.\t\t\t",
	editChars::toggleBetweenCRandBg, ": to toggle between background and character "
	"rules chunks.\t\t\t",
	editChars::toggleReferenceChunkView, ": to toggle the reference chunk view (this is "
	"only relevant when in mode 2, see -h for more info.)\t\t\t",
	editChars::cREraseCRChar,	": to set the cursor character to the rules erase "
	"character. This allows for the erasure of rules characters from the chunk.\t\t\t",
	editChars::cRSetCRCharToBorder, ": to set the cursor character to the boarder "
	"character.\t\t\t",
	editChars::cRSetCRCharToPlatform, ": to set the cursor character to the platform "
	"character.\t\t\t",
	editChars::redo,		": to cycle the chunk buffer position forward (redo). ",
	"This cycles forward through the chunk buffer. Every time a chunk is "
	"altered the chunk at the current position in the buffer is unchanged "
	"and a copy of that chunk is saved in the next buffer position where the "
	"change is made. The current position is updated to the position of this "
	"new altered copy.\t\t\t",
	editChars::undo,		": to cycle the chunk buffer position back (undo). "
	"This cycles back through the chunk buffer. Every time a chunk is "
	"altered the chunk at the current position in the buffer is unchanged "
	"and a copy of that chunk is saved in the next buffer position where the "
	"change is made. The current position is updated to the position of this "
	"new altered copy.\t\t\t",
	editChars::toggleLineDrawMode, ": to toggle line drawing. When line drawing is "
	"turned on any character the user moves the cursor over will be set to "
	"the current cursor character. This way lines can be drawn without "
	"having to constantly press \"", editChars::performActionAtPos, "\".\t\t\t",
	editChars::toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
	editChars::changeCoordinates, ": to change coordinates. This changes the "
	"coordinates of both the background chunk and rules character chunk as "
	"they are a set.\t\t\t",
	editChars::saveChunks,	": to save (output) both chunks.\t\t\t",
	editChars::quit,		": to quite the program.")};
    const std::string refViewingBg
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn the reference chunk view mode (viewing a background chunk).\t\t\t\t\t",
	editChars::toggleHelpMsg,	": to toggle this message.\t\t\t",
	editChars::cursorUp,		": to move the cursor up.\t\t\t",
	editChars::cursorDown,	": to move the cursor down.\t\t\t",
	editChars::cursorLeft,	": to move the cursor left.\t\t\t",
	editChars::cursorRight,	": to move the cursor right.\t\t\t",
	editChars::toggleBetweenCRandBg, ": to toggle between background and character "
	"rules chunks.\t\t\t",
	editChars::toggleReferenceChunkView, ": to toggle the reference chunk view (this is "
	"only relevant when in mode 2, see -h for more info.)\t\t\t",
	editChars::bgGetCharAtPos,	": to set the cursor character to the background chunk "
	"character below it.\t\t\t",
	editChars::selectSelection,	": to enter selection mode, where a an area (selection) "
	"can be selected for copying and later pasting.\t\t\t",
	editChars::toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
	editChars::quit,		": to quite the program.")};
    const std::string refViewingCR
      {concat
       ("~H~E~L~P~!~\nIn the reference chunk view mode "
	"(viewing a character rules chunk).\n",
	editChars::toggleHelpMsg,		":    to toggle this "
	"message.\n",
	editChars::cursorUp, ", ", editChars::cursorLeft, ", ",
	editChars::cursorDown, " or ", editChars::cursorRight, ":    "
	"to move the cursor down up, left, down or right respectively.\n",
	editChars::toggleBetweenCRandBg,	":    to toggle between "
	"background and character rules chunks.\n",
	editChars::toggleReferenceChunkView,	":    to toggle the reference "
	"chunk view (this is only relevant when in mode 2, see -h for more "
	"info.)\n",
	editChars::toggleCrosshairCursor,	":    to toggle crosshair "
	"cursor.\n",
	editChars::quit,			":    to quite the program.")};
    const std::string showUnsetBgChars
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn show unset background characters "
	"mode.\t\t\t\t\t",
	"If a new chunk is being edited this mode will display random "
	"colours "
	"in any positions that have not yet had a character assigned to "
	"them. "
	"This is useful because the background and corresponding character "
	"rules chunk cannot be saved unless the background chunk has had a "
	"character assigned to each position in the chunk.\t\t\t",
	editChars::toggleHelpMsg, ": to toggle this message.\t\t\t",
	editChars::quit,       	": to return to the main editing mode.")};
    const std::string selectAndCopySelection
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn selection mode.\t\t\t\t\t",
	editChars::toggleHelpMsg,	": to toggle this message.\t\t\t",
	editChars::cursorUp,		": to move the cursor up.\t\t\t",
	editChars::cursorDown,	": to move the cursor down.\t\t\t",
	editChars::cursorLeft,	": to move the cursor left.\t\t\t",
	editChars::cursorRight,	": to move the cursor right.\t\t\t",
	" \"", editChars::performActionAtPos, " \": to select / deselect the character "
	"under the cursor.\t\t\t",
	editChars::toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
	editChars::toggleLineDrawMode, ": to toggle line drawing. When line drawing is "
	"turned on any character the user moves the cursor over will be marked "
	"to be copied (unless it is already marked in which case it will be "
	"unmarked.) This way lines can be drawn "
	"without having to constantly press \"", editChars::performActionAtPos,
	"\".\t\t\t",
	editChars::floodFill,	": to do a flood fill starting at the current cursor "
	"pos.\t\t\t",
	editChars::selectSelection, ": to save the current selection and exit the "
	"selection mode.\t\t\t",
	editChars::quit,		": to quit selection mode and return to the main "
	"editing mode.")
      };
    const std::string pasteSelection
      {concat
       ("\t~H~E~L~P~!~\t\t\t\t\t\tIn paste mode.\t\t\t\t\t",
	editChars::toggleHelpMsg,	": to toggle this message.\t\t\t",
	editChars::cursorUp,		": to move the cursor up.\t\t\t",
	editChars::cursorDown,	": to move the cursor down.\t\t\t",
	editChars::cursorLeft,	": to move the cursor left.\t\t\t",
	editChars::cursorRight,	": to move\n the cursor right.\t\t\t",
	" \"", editChars::performActionAtPos, " \": to paste the current selection.\t\t\t",
	editChars::rotateSelection,	": to rotate the selection right 90 degrees.\t\t\t",
	editChars::mirrorSelection,	": to mirror the selection horizontally\t\t\t",
	editChars::nextSelection,	": to cycle to the next selection (if any)\t\t\t",
	editChars::lastSelection,	": to cycle to the last selection (if any)\t\t\t",
	editChars::toggleCrosshairCursor, ": to toggle crosshair cursor.\t\t\t",
	editChars::quit,		": to quit paste selection mode and return to the main "
	"editing mode.")
      };
    const std::string getBgCharFgColorFromUser
      {"\tPlease select a foreground color for the character."};
    const std::string getBgCharBgColorFromUser
      {"\tPlease select a background color for the character."};
    const std::string getBgCharFromUser
      {concat("\tPlease select a character.\t")};
  }
}


/* We need this to return an array as opposed to just a pointer (basically we
   return this struct and then we can access data and it will be an array.) */
template <typename T1, int yHeight, int xWidth>
struct array2D {
  T1 data[yHeight][xWidth];
};


/* This class is used to store chunks for editing. It is designed to store
   multiple copies so that undo / redo can be implemented. The class exposes
   more than it probably should so it has to be used properly (see the comments
   for the member functions.) */
template<typename T, int yHeight, int xWidth>
class chunk
{
private:
  static constexpr int undoBuffers		{1024};
  array2D<T, yHeight, xWidth> tmpChunk	       	{};
  array2D<T, yHeight, xWidth> chunks[undoBuffers] {};
  int currentChunk				{undoBuffers -1};
  const std::chrono::milliseconds showLastChunkIndexFor {};
  std::chrono::steady_clock::time_point lastCurrentChunkUpdate {};
  

  void copyToTmp()
  {
    for(int yIter {}; yIter < yHeight; ++yIter)
      {
	for(int xIter {}; xIter < xWidth; ++xIter)
	  {
	    tmpChunk.data[yIter][xIter] =
	      chunks[currentChunk].data[yIter][xIter];
	  }
      }
  }
    
  void copyFromTmp()
  {
    for(int yIter {}; yIter < yHeight; ++yIter)
      {
	for(int xIter {}; xIter < xWidth; ++xIter)
	  {
	    chunks[currentChunk].data[yIter][xIter] =
	      tmpChunk.data[yIter][xIter];
	  }
      }
  }

public:
  chunk(const T filler,
	const std::chrono::milliseconds showLastCurrentChunkIndexFor):
    showLastChunkIndexFor(showLastCurrentChunkIndexFor),
    lastCurrentChunkUpdate(std::chrono::steady_clock::now())
  {
    for(int yIter {}; yIter < yHeight; ++yIter)
      {
	for(int xIter {}; xIter < xWidth; ++xIter)
	  { 
	    tmpChunk.data[yIter][xIter] = filler;
	  }
      }

    for(int chunkIter {}; chunkIter < undoBuffers; ++chunkIter)
      {
	for(int yIter {}; yIter < yHeight; ++yIter)
	  {
	    for(int xIter {}; xIter < xWidth; ++xIter)
	      {
		chunks[chunkIter].data[yIter][xIter] = filler;;
	      }
	  }
      }
  }
  
  /* Should be used when the chunk is needed for display, but not for
     modification. */
  array2D<T, yHeight, xWidth> & getChunk()
  {
    return chunks[currentChunk];
  }

  // This function should be called before modifying the chunk.
  array2D<T, yHeight, xWidth> & advanceBeforeModify()
  {
    // Backup chunk.
    copyToTmp();
    // Advance.
    currentChunk < undoBuffers -1 ? currentChunk++:
      currentChunk = 0;
    // Copy over new current chunk.
    copyFromTmp();
    // Return new copy for modification.
    return chunks[currentChunk];
  }

  // This function essentially implements a "redo" function.
  void forward()
  {
    currentChunk < undoBuffers -1 ? currentChunk++:
      currentChunk = 0;
    lastCurrentChunkUpdate = std::chrono::steady_clock::now();
  }

  // This function essentially implements an "undo" function.
  void backward()
  {
    currentChunk == 0 ?
      currentChunk = undoBuffers - 1:
      currentChunk--;
    lastCurrentChunkUpdate = std::chrono::steady_clock::now();
  }

  void printIndexIfChanged(const int y, const int x)
  {
    if(std::chrono::steady_clock::now() - lastCurrentChunkUpdate <
       showLastChunkIndexFor)
      {
	editingSettings::colorMode.setColor(editingSettings::helpColor);
	mvprintw(y, x, "%d", currentChunk);
	refresh();
      }
  }
};


/* Read in and decompress files if they exist, else ask user for coordinates.
 Finally call editModeProper(). */
void editMode
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 const std::string refBgChunkFileName, const std::string refCRChunkFileName,
 const yx chunkSize, const bool usingReferences);


#endif
