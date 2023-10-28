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
  int currentChunk				{};
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
