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
    constexpr char border	{'b'};
    constexpr char platform	{'p'};
  }

  namespace helpMsgs
  {
    const std::string mainViewingBg
      {
	concat
	(impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	 impctCol2PrntSeq, "In the main editing mode (viewing a background "
	 "chunk.)" , hlpColrPrntSeq, "\n",
	 hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	 ":    to toggle this message.\n",
	 hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	 hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	 hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	 hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	 "to move the cursor up, left, down or right respectively.\n",
	 "\"", hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq,
	 "\":    to perform an action at the current cursor position. The "
	 "specific action performed depends on the context. The most important "
	 "action is to change the character under the cursor to the same "
	 "character as the cursor.\n",
	 hiltColPrntSeq, editChars::toggleBetweenCRandBg, hlpColrPrntSeq,
	 ":    to toggle between background and character rules chunks.\n",
	 hiltColPrntSeq, editChars::toggleReferenceChunkView, hlpColrPrntSeq,
	 ":    to toggle the reference chunk view (this is only relevant when "
	 "in mode 2, see -h for more info.)\n",
	 hiltColPrntSeq, editChars::bGToggleCharacterSelection, hlpColrPrntSeq,
	 ":    to select a new "
	 "character color and character for the cursor character.\n",
	 hiltColPrntSeq, editChars::bgNextCurrentChar, hlpColrPrntSeq,
	 ":    to cycle forward (next) through the recent cursor characters.\n",
	 hiltColPrntSeq, editChars::bgLastCurrentChar, hlpColrPrntSeq,
	 ":    to cycle backwards (last) through the recent cursor characters."
	 "\n",
	 hiltColPrntSeq, editChars::redo, hlpColrPrntSeq,
	 ":    to cycle the chunk buffer position forward (redo). This cycles "
	 "forward through the chunk buffer. Every time a chunk is altered the "
	 "chunk at the current position in the buffer is unchanged and a copy "
	 "of that chunk is saved in the next buffer position where the change "
	 "is made. The current position is updated to the position of this new "
	 "altered copy.\n",
	 hiltColPrntSeq, editChars::undo, hlpColrPrntSeq,
	 ":    to cycle the chunk buffer position back (undo). This cycles "
	 "back through the chunk buffer. Every time a chunk is altered the "
	 "chunk at the current position in the buffer is unchanged and a copy "
	 "of that chunk is saved in the next buffer position where the change "
	 "is made. The current position is updated to the position of this new "
	 "altered copy.\n",
	 hiltColPrntSeq, editChars::bgGetCharAtPos, hlpColrPrntSeq,
	 ":    to set the cursor character to the background chunk character "
	 "below it. This is only applicable if the character at the cursor pos "
	 "has already been set.\n",
	 hiltColPrntSeq, editChars::toggleLineDrawMode, hlpColrPrntSeq,
	 ":    to toggle line drawing. When line drawing is turned on any "
	 "character the user moves the cursor over will be set to the current "
	 "cursor fcharacter. This way lines can be drawn without having to "
	 "constantly press \"",
	 hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq, "\".\n",
	 hiltColPrntSeq, editChars::floodFill, hlpColrPrntSeq,
	 ":    to do a flood fill using the cursor character and starting at "
	 "the current cursor pos.\n",
	 hiltColPrntSeq, editChars::selectSelection, hlpColrPrntSeq,
	 ":    to enter selection mode, where an area (selection) can be "
	 "selected for copying and later pasting.\n",
	 hiltColPrntSeq, editChars::pasteSelection, hlpColrPrntSeq,
	 ":    to enter past mode, where ", selectionRingBufferSize,
	 " of the last selections made can be pasted.\n",
	 hiltColPrntSeq, editChars::bgShowUnsetChars, hlpColrPrntSeq,
	 ":    to print noise in locations where characters aren't set. This "
	 "helps when trying to find unset background chunk characters. Note "
	 "that the chunks cannot be saved if any of the background chunk "
	 "characters are unset. Also note that when editing a new background "
	 "chunk file all characters are initially unset.\n",
	 hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	 ":    to toggle crosshair cursor.\n",
	 hiltColPrntSeq, editChars::changeCoordinates, hlpColrPrntSeq,
	 ":    to change coordinates. This changes the coordinates of both the "
	 "background chunk and rules character chunk.\n",
	 hiltColPrntSeq, editChars::saveChunks, hlpColrPrntSeq,
	 ":    to save (output) both chunks.\n",
	 hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	 ":    to quite the program.")};
    const std::string mainViewingCR
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In the main editing mode (viewing a character rules"
	"chunk.)", hlpColrPrntSeq, "\n", 
	 hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	"to move the cursor up, left, down or right respectively.\n\"",
	hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq,
	"\":    to perform an action at the current cursor position. The "
	"specific action performed depends on the context. The most important "
	"action is to change the character under the cursor to the same "
	"character as the cursor.\n",
	hiltColPrntSeq, editChars::toggleBetweenCRandBg, hlpColrPrntSeq,
	":    to toggle between background and character rules chunks.\n",
	hiltColPrntSeq, editChars::toggleReferenceChunkView, hlpColrPrntSeq,
	":    to toggle the reference chunk view (this is only relevant when "
	"in mode 2, see -h for more info.)\n",
	hiltColPrntSeq, editChars::cREraseCRChar, hlpColrPrntSeq,
	":    to set the cursor character to the rules erase character. This "
	"allows for the erasure of rules characters from the chunk.\n",
	hiltColPrntSeq, editChars::cRSetCRCharToBorder, hlpColrPrntSeq,
	":    to set the cursor character to the border character.\n",
	hiltColPrntSeq, editChars::cRSetCRCharToPlatform, hlpColrPrntSeq,
	":    to set the cursor character to the platform character.\n",
	hiltColPrntSeq, editChars::redo, hlpColrPrntSeq,
	":    to cycle the chunk buffer position forward (redo). This cycles "
	"forward through the chunk buffer. Every time a chunk is altered the "
	"chunk at the current position in the buffer is unchanged and a copy "
	"of that chunk is saved in the next buffer position where the change "
	"is made. The current position is updated to the position of this new "
	"altered copy.\n",
	hiltColPrntSeq, editChars::undo, hlpColrPrntSeq,
	":    to cycle the chunk buffer position back (undo). This cycles back "
	"through the chunk buffer. Every time a chunk is altered the chunk at "
	"the current position in the buffer is unchanged and a copy of that "
	"chunk is saved in the next buffer position where the change is made. "
	"The current position is updated to the position of this new altered "
	"copy.\n",
	hiltColPrntSeq, editChars::toggleLineDrawMode, hlpColrPrntSeq,
	":    to toggle line drawing. When line drawing is turned on any "
	"character the user moves the cursor over will be set to the current "
	"cursor character. This way lines can be drawn without having to "
	"constantly press \"",
	hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq, "\".\n",
	hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	":    to toggle crosshair cursor.\n",
	hiltColPrntSeq, editChars::changeCoordinates, hlpColrPrntSeq,
	":    to change coordinates. This changes the coordinates of both the "
	"background chunk and rules character chunk as they are a set.\n",
	hiltColPrntSeq, editChars::saveChunks,  hlpColrPrntSeq,
	":    to save (output) both chunks.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to quite the program.")};
    const std::string refViewingBg
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In the reference chunk view mode (viewing a "
	"background chunk).", hlpColrPrntSeq, "\n",
	hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	"to move the cursor up, left, down or right respectively.\n",
	hiltColPrntSeq, editChars::toggleBetweenCRandBg, hlpColrPrntSeq,
	":    to toggle between background and character rules chunks.\n",
	hiltColPrntSeq, editChars::toggleReferenceChunkView, hlpColrPrntSeq,
	":    to toggle the reference chunk view (this is only relevant when "
	"in mode 2, see -h for more info.)\n",
	hiltColPrntSeq, editChars::bgGetCharAtPos, hlpColrPrntSeq,
	":    to set the cursor character to the background chunk character "
	"below it.\n",
	hiltColPrntSeq, editChars::selectSelection, hlpColrPrntSeq,
	":    to enter selection mode, where an area (selection) can be "
	"selected for copying and later pasting.\n",
	hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	":    to toggle crosshair cursor.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to quite the program.")};
    const std::string refViewingCR
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In the reference chunk view mode (viewing a "
	"character rules chunk).", hlpColrPrntSeq, "\n",
	hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	"to move the cursor up, left, down or right respectively.\n",
	hiltColPrntSeq, editChars::toggleBetweenCRandBg, hlpColrPrntSeq,
	":    to toggle between background and character rules chunks.\n",
	hiltColPrntSeq, editChars::toggleReferenceChunkView, hlpColrPrntSeq,
	":    to toggle the reference chunk view (this is only relevant when "
	"in mode 2, see -h for more info.)\n",
	hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	":    to toggle crosshair cursor.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to quite the program.")};
    const std::string showUnsetBgChars
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In show unset background characters mode.",
	hlpColrPrntSeq, "\n"
	"If a new chunk is being edited this mode will display random colours "
	"in any positions that have not yet had a character assigned to them. "
	"This is useful because the background and corresponding character "
	"rules chunk cannot be saved unless the background chunk has had a "
	"character assigned to each position in the chunk.\n",
	hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to return to the main editing mode.")};
    const std::string selectAndCopySelection
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In selection mode.", hlpColrPrntSeq, "\n",
	hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	"to move the cursor up, left, down or right respectively.\n\"",
	hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq,
	"\":    to select / deselect the character under the cursor.\n",
	hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	":    to toggle crosshair cursor.\n",
	hiltColPrntSeq, editChars::toggleLineDrawMode, hlpColrPrntSeq,
	":    to toggle line drawing. When line drawing is turned on any "
	"character the user moves the cursor over will be marked to be copied "
	"(unless it is already marked in which case it will be unmarked.) This "
	"way lines can be drawn without having to constantly press \"",
	hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq,
	"\".\n",
	hiltColPrntSeq, editChars::floodFill, hlpColrPrntSeq,
	":    to do a flood fill starting at the current cursor pos.\n",
	hiltColPrntSeq, editChars::selectSelection, hlpColrPrntSeq,
	":    to save the current selection and exit the selection mode.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to quit selection mode and return to the main editing mode.")};
    const std::string pasteSelection
      {concat
       (impctCol1PrntSeq, "~H~E~L~P~!~", hlpColrPrntSeq, "\n",
	impctCol2PrntSeq, "In paste mode.", hlpColrPrntSeq, "\n",
	hiltColPrntSeq, editChars::toggleHelpMsg, hlpColrPrntSeq,
	":    to toggle this message.\n",
	hiltColPrntSeq, editChars::cursorUp, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorLeft, hlpColrPrntSeq, ", ",
	hiltColPrntSeq, editChars::cursorDown, hlpColrPrntSeq, " or ",
	hiltColPrntSeq, editChars::cursorRight, hlpColrPrntSeq, ":    "
	"to move the cursor up, left, down or right respectively.\n",
	"\"", hiltColPrntSeq, editChars::performActionAtPos, hlpColrPrntSeq,
	"\":    to paste the current selection.\n",
	hiltColPrntSeq, editChars::rotateSelection, hlpColrPrntSeq,
	":    to rotate the selection right 90 degrees.\n",
	hiltColPrntSeq, editChars::mirrorSelection, hlpColrPrntSeq,
	":    to mirror the selection horizontally\n",
	hiltColPrntSeq, editChars::nextSelection, hlpColrPrntSeq,
	":    to cycle to the next selection (if any)\n",
	hiltColPrntSeq, editChars::lastSelection, hlpColrPrntSeq,
	":    to cycle to the last selection (if any)\n",
	hiltColPrntSeq, editChars::toggleCrosshairCursor, hlpColrPrntSeq,
	":    to toggle crosshair cursor.\n",
	hiltColPrntSeq, editChars::quit, hlpColrPrntSeq,
	":    to quit paste selection mode and return to the main editing mode."
	)};
    const std::string getBgCharFgColorFromUser
      {"Please select a foreground color for the character."};
    const std::string getBgCharBgColorFromUser
      {"Please select a background color for the character."};
    const std::string getBgCharFromUser
      {concat("Please select a character.")};
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
