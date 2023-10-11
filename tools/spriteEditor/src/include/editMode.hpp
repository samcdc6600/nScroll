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
  constexpr std::chrono::milliseconds cursorBlinkTimeMs {650};
    constexpr std::chrono::milliseconds
    colorPairForSlicePrintDisplayTime {20000};
  constexpr std::chrono::milliseconds showLastChunkAfterUpdateIndexFor {2500};
  extern setColorMode colorMode;
  // Color used for help menu.
  constexpr int helpColor {helpColorPair};
  //   /* This color pair is used when the character in the position in question is
  //      not yet set. */
  // constexpr int invalidCharColorPair {-1};
  constexpr int blackBgColorPair {15};

  //    /* See editMode.cpp for
  //     setColorMode colorMode {colorParams::defaultColorPair}; */
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
    constexpr char eraseChar       	{'e'}; // Erase char at pos.
    constexpr char toggleCharacterSelection	{'c'};
    constexpr char nextCurrentChar	{'n'};
    constexpr char lastCurrentChar	{'l'};
    constexpr char redo			{'r'};
    constexpr char undo			{'u'};
    constexpr char getCharAtPos      	{'g'};
    constexpr char floodFill		{'f'};
    constexpr char changeSliceDisplayTime	{'t'};
    constexpr char saveSprite		{'o'};
    constexpr char toggleHelpMsg       	{'h'};
  };
}


// Type used for characters that make up sprite slices. 
typedef int spriteCharType;


struct spriteCharData
{
  bool set {false};
  spriteCharType ch {};
};

    
/* We need this to return an array as opposed to just a pointer (basically we
   return this struct and then we can access data and it will be an array.) */
template <typename T1, int yHeight, int xWidth>
struct array2D {
  T1 data[yHeight][xWidth];
};


/* We need use this as it will make it easier to handle the disp time for
   sprite slices. */
template <typename T1, int yHeight, int xWidth>
struct sliceArray2D {
  /* Time in MS that this slice should be displayed for. */
  int dispTimeMS;
  T1 data[yHeight][xWidth];
};


/* This class is used to store sprite slices for editing. It is designed to store
   multiple copies so that undo / redo can be implemented. The class exposes
   more than it probably should so it has to be used properly (see the comments
   for the member functions.) */
template<typename T, int yHeight, int xWidth>
class sprite
{
private:
  static constexpr int undoBuffers				{1024};
  std::vector<sliceArray2D<T, yHeight, xWidth>> tmpSprite	       	{};
  std::vector<sliceArray2D<T, yHeight, xWidth>> sprites[undoBuffers]	{};
  int currentSprite						{};
  int currentSlice						{};
  const std::chrono::milliseconds showLastSpriteIndexFor	{};
  std::chrono::steady_clock::time_point lastCurrentSpriteUpdate {};
  

  void copyToTmp()
  {
    tmpSprite.assign
      (sprites[currentSprite].begin(), sprites[currentSprite].end());
  }
    
  void copyFromTmp()
  {
    sprites[currentSprite].assign(tmpSprite.begin(), tmpSprite.end());
  }

public:
  sprite(const std::chrono::milliseconds showLastCurrentSpriteIndexFor):
    showLastSpriteIndexFor(showLastCurrentSpriteIndexFor),
    lastCurrentSpriteUpdate(std::chrono::steady_clock::now())
  {
    bool noSprite {true};
    if(noSprite)
      {
	sprites[currentSprite].push_back(sliceArray2D<T, yHeight, xWidth> {});
      }
  }
  
  /* Should be used when the sprite is needed for display, but not for
     modification. */
  // T operator()(const int y, const int x, int * dispTimeMS)
  // {
  //   dispTimeMS = &sprites[currentSprite][currentSlice].displayTimeMs;
  //   return sprites[currentSprite][currentSlice].data[y][x];
  // }

  T operator()(const int y, const int x)
  {
    return sprites[currentSprite][currentSlice].data[y][x];
  }

  int getCurrentSliceDispTime()
  {
    return sprites[currentSprite][currentSlice].displayTimeMs;
  }

  // /* NOTE THAT IF THIS FUNTION IS CALLED THE .CH MEMBER OF THE RETURN VALUE
  //    MUST BE SET!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //    This function should be called before modifying the sprite. It will return 
  //    a reference to the character of the current slice that is at position y, x
  //    and it will also set dispTimeMS to the address of dispTimeMS for the
  //    current slice. */
  // void advanceBeforeModify(const int y, const int x, int dispTimeMS)
  // {
  //   // Backup sprite.
  //   copyToTmp();
  //   // Advance.
  //   currentSprite < undoBuffers -1 ? currentSprite++:
  //     currentSprite = 0;
  //   // Copy over new current sprite.
  //   copyFromTmp();
  //   dispTimeMS = &sprites[currentSprite][currentSlice].displayTimeMs;
  //   if(!sprites[currentSprite][currentSlice].data[y][x].set)
  //     {
  // 	sprites[currentSprite][currentSlice].data[y][x].set = true;
  //     }
  //   // Return new copy for modification.
  //   return sprites[currentSprite][currentSlice].data[y][x];
  // }

  
  void advanceBeforeModify(const int y, const int x, spriteCharType newChar)
  {
    // Backup sprite.
    copyToTmp();
    // Advance.
    currentSprite < undoBuffers -1 ? currentSprite++:
      currentSprite = 0;
    // Copy over new current sprite.
    copyFromTmp();
    sprites[currentSprite][currentSlice].data[y][x].ch = newChar;
    sprites[currentSprite][currentSlice].data[y][x].set = true;
  }

  void forwardSlice()
  {
    currentSlice < sprites[currentSprite].size() -1 ? currentSlice++:
      currentSlice = 0;
  }

  void backwardSlice()
  {
    currentSlice == 0 ?
      currentSlice = sprites[currentSprite].size() -1:
      currentSlice--;
  }

  void newSlice()
  {
    if(sprites[currentSprite].size > 0)
      {
	// New sprite slice is a copy of the last one (if there is a last one.)
	sprites[currentSprite].push_back(sprites[currentSprite][currentSlice]);
      }
    else
      {
	sprites[currentSprite].push_back(sliceArray2D<T, yHeight, xWidth> {});
      }
  }

  /* This function (in conjunction with advanceBeforeModify) implements a "redo"
     function. */
  void forwardSprite()
  {
    currentSprite < undoBuffers -1 ? currentSprite++:
      currentSprite = 0;
    lastCurrentSpriteUpdate = std::chrono::steady_clock::now();
  }

  /* This function (in conjunction with advanceBeforeModify) implements an
     "undo" function. */
  void backwardSprite()
  {
    currentSprite == 0 ?
      currentSprite = undoBuffers - 1:
      currentSprite--;
    lastCurrentSpriteUpdate = std::chrono::steady_clock::now();
  }

  void printUndoRedoBufferIndexIfChanged(const int y, const int x)
  {
    if(std::chrono::steady_clock::now() - lastCurrentSpriteUpdate <
       showLastSpriteIndexFor)
      {
	editingSettings::colorMode.setColor(editingSettings::helpColor);
	mvprintw(y, x, "%d", currentSprite);
	refresh();
      }
  }
};


/* Read in and decompress file if it exists, else ask user for coordinates.
   Finally call editModeProper(). */
void editMode
(const std::string spriteFileName, const yx spriteSize);


#endif EDITMODE_HPP_
