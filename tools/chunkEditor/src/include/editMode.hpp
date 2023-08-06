#ifndef EDITMODE_HPP_
#define EDITMODE_HPP_


// #include <string>
#include "utils.hpp"
#include "colorS.hpp"


namespace editingSettings
{
  constexpr int loopSleepTimeMs {5};
  /* Sleep for this time when exiting a sub menu so that the character the user
     is pressing isn't read in as an input to the "screen" "above" the
     menu. This is especially important as the character my be editChars::quit.
  */
  constexpr int editSubMenuSleepTimeMs {160};
  constexpr std::chrono::milliseconds cursorBlinkTimeMs {700};
  extern setColorMode colorMode;
  // Color used for help menu.
  constexpr int helpColor {1};
  /* This color pair is used when a character is not yet in the position in
     question. */
  constexpr int noCharColorPair {15};
  constexpr int validColorNumber {1};
  constexpr int emptyCharChar {' '};
  
  /* See editMode.cpp for
     setColorMode colorMode {colorParams::defaultColorPair}; */
  namespace editChars
  {
    /* Where a cR prefix means that the command should be specific to character
       rules editing mode and a prefix of br means that the command should be
       specific to background editing mode. */
    constexpr char quit			{'q'};
    constexpr char cursorUp		{'w'};
    constexpr char cursorDown		{'s'};
    constexpr char cursorLeft		{'a'};
    constexpr char cursorRight		{'d'};
    constexpr char performActionAtPos	{' '};
    constexpr char toggleBetweenCRandBg	{'t'};
    constexpr char cREraseCRChar       	{'e'};
    constexpr char cRSetCRCharToBorder	{'b'};
    constexpr char cRSetCRCharToPlatform	{'p'};
    constexpr char bGToggleCharacterSelection	{'c'};
    constexpr char bgNextCurrentChar	{'n'};
    constexpr char bgLastCurrentChar	{'l'};
    constexpr char bgGetCharAtPos      	{'g'};
    constexpr char bgFloodFill		{'f'};
    // 'i' for info.
    constexpr char bgShowUnsetChars	{'i'};
    constexpr char toggleHelpMsg       	{'h'};
  };

  namespace rulesChars
  {
    constexpr char nullRule	{' '};
    constexpr char boarder	{'b'};
    constexpr char platform	{'p'};
  }

  namespace bgChars
  {
  }
}


struct backgroundChunkCharInfo
{
  int ch;
  // Tells if ch has been set yet.
  bool set;
};


/* Read in and decompress files if they exist, else ask user for coordinates.
 Finally call editModeProper(). */
void editMode
(const std::string bgChunkFileName, const std::string cRChunkFileName,
 backgroundChunkCharInfo bgChunk[][xWidth], char cRChunk[][xWidth],
 const yx chunkSize);


#endif
