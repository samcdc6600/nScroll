#ifndef EDITMODE_HPP_
#define EDITMODE_HPP_


// #include <string>
#include "utils.hpp"
#include "colorS.hpp"


namespace editingSettings
{
  constexpr int loopSleepTimeMs {10};
  constexpr std::chrono::milliseconds cursorBlinkTimeMs {700};
  const int gameColorPairsNo {15625};
  extern setColorMode colorMode;
  /* See editMode.cpp for
     setColorMode colorMode {colorParams::defaultColorPair}; */
  namespace editChars
  {
    constexpr char quit			{'q'};
    constexpr char cursorUp		{'w'};
    constexpr char cursorDown		{'s'};
    constexpr char cursorLeft		{'a'};
    constexpr char cursorRight		{'d'};
    constexpr char setCharacterAtPos	{' '};
    constexpr char toggleBetweenCRandBg	{'t'};
    constexpr char eraseCRChar		{'e'};
    constexpr char setCRCharToBorder	{'b'};
    constexpr char setCRCharToPlatform	{'p'};
    constexpr char toggleCharacterSelection {'c'};
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
