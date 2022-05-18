#include <stdexcept>
#include <sstream>
#include <ncurses.h>
#include "include/utils.hpp"
#include "include/collapse.hpp"
#include "include/colorS.hpp"


constexpr char NAME_OF_THIS_FILE [] = "collapse.cpp";


// Copies rawBuff to buff with all '\n' characters removed.
void firstPassOfCollapse(const std::string & rawBuff,
		       std::string & buff);
/* Copies buff to preRet with all "\nc/" (non colour) escape sequences
processed. */
void secondPassOfCollapse(std::string & buff, std::vector<int> & preRet);
void thirdPassOfCollapse(std::vector<int> & preRet, std::vector<int> & ret);


chMap::chMap()
{
  int chVal {128};		// 128 is one above the ascii character set
  /* insert all the spcial character types into the map along with their key
  values */
  for(std::string strIter: characters)
    {				/* enter element into map with a key of strIter
				   and a value of offset. */
      strMap[strIter]=chVal;
      /* increment chVal (each character must have a uniqe value not equal to
	 any ascii value). */
      ++chVal;
    }
}


int chMap::find(const std::string str)
{//map.at() throws and out of range exception if no matching key is found
  return strMap.at(str);//returns value that corresponds to the key str
}


void collapse(const std::string & rawBuff,
	      std::vector<int> & ret)
{
  ret.clear();			// Make sure that ret is empty.
  std::string buff {};

  firstPassOfCollapse(rawBuff, buff);  
  std::vector<int> preRet {};
  secondPassOfCollapse(buff, preRet);
  thirdPassOfCollapse(preRet, ret);
}


void firstPassOfCollapse(const std::string & rawBuff,
		       std::string & buff)
{
  for(size_t iter{}; iter < rawBuff.size(); ++iter)
    {
      if(rawBuff[iter] != '\n')
	buff.push_back(rawBuff[iter]);
    }
}


void secondPassOfCollapse(std::string & buff, std::vector<int> & preRet)
{ // Collaps "\nc/" ecsape sequences (special characters.)
  // escSeq holds the string used to get the color code from escCharNums.
  std::string escSeq {};
  /* Declared as static so that the constructor will only be called once. This
     is an optimization since the constructor does a lot of work that is
     redundent if done more then once. */
  static chMap escCharNums;
  constexpr size_t ncEscLen {3}; // Length of the escape sequence.
  
  for(size_t iter{}; iter < buff.size(); ++iter)
    { // If we have found the start of a nonColour escape sequence.
      if(buff[iter] == '\\' && buff[1 + iter] == 'n' && buff[2 + iter] == 'c')
	{ // Skip first three characters of escape sequencse ("\nc").
	  iter += ncEscLen;
	  if(iter > buff.size()) // Check for errors in formatting.
	    { // If iter is outside of range.
	      std::string eMsg {"in collaps.cpp ->collaps(const std::string & "
		"buffWithSpace, std::vector<int> ret). First pass. iter is > "
		"buff.size. Iter ="};
	      eMsg += iter;
	      eMsg += buff.size();
	      eMsg += '\n';
	      throw std::logic_error(eMsg);
	    }
	  for( ; buff[iter] != '/'; ++iter)
	    { // The next two if statments check for errors.
	      if(iter > buff.size())
		{
		  std::string eMsg {"in collaps.cpp->collaps(const std::string "
		    "& buffWithSpace, std::vector<int ret). iter ="};
		  eMsg += iter;       eMsg += "\n";
		  throw std::logic_error(eMsg);
		}
	      escSeq += buff[iter]; // Get nonColour escape sequence.
	    }
	  /* Set next element in preRet to int value that corresponds to escSeq
	     nonColour escape sequence. */
	  preRet.push_back(escCharNums.find(escSeq));
	  /* Clear escSeq so that it is empty next time we find a nonColour
	     escape sequence and use += on escSeq. */
	  escSeq.clear();
	}
      else
	{ /* If we have found a normal character or a transparent space (no
	     character is displayed) character. */
	  if(buff[iter] == '\\' && buff[1 + iter] == 't' &&
	     buff[2 + iter] == 'r' && buff[3 + iter] == 'a' &&
	     buff[4 + iter] == 'n' && buff[5 + iter] == 's' &&
	     buff[6 + iter] == 'S' && buff[7 + iter] == 'P' &&
	     buff[8 + iter] == '/')
	    {
	      preRet.push_back(TRANS_SP);
	      iter += 8;
	    }
	  else
	    preRet.push_back(buff[iter]);
	}
    }
}


void thirdPassOfCollapse(std::vector<int> & preRet, std::vector<int> & ret)
{
  /* Declared as static so that the constructor will only be called once. This
     is an optimization since the constructor does a lot of work that is
     redundent if done more then once. */
  static colourMap cuMap;
  // EscSeq holds the string used to get the color code from cuMap.
  std::string escSeq {};
  for(size_t iter{}; iter < preRet.size(); ++iter)
    {
      if(char(preRet[iter]) == '\\' && char(preRet[1 + iter]) == 'c' && char(preRet[2 + iter]) == 'u')
	{ // We are in the start of a color escape sequence.
	  iter += 3;
	  if(iter > preRet.size())
	    {
	      throw std::logic_error("iter += 3 if(iter > preRet.size())");
	    }
	  for( ; char(preRet[iter]) != '/'; ++iter)
	    {
	      if(iter > preRet.size())
		{
		  throw std::logic_error("for( ; char(preRet[iter]) != '/'; ++iter) if(iter > preRet.size())");
		}
	      escSeq += preRet[iter];
	    } // We have the color information (in escSeq.)
	  iter++;
	  bool inColor {true};
	  for(size_t iterColor{iter}; inColor; ++iterColor)
	    {// Now apply color information to characters.
	      if((iterColor - 3) >= preRet.size())
		{
		  std::stringstream e {};
		  e<<"Error (in \""<<NAME_OF_THIS_FILE<<"\"): iterColor = "
		   <<iterColor<<", preRet.size() = "<<preRet.size();
		  exit(e.str().c_str(), ERROR_GENERIC_RANGE_ERROR);
		}
	      if(char(preRet[iterColor]) == '\\' && char(preRet[iterColor + 1]) == 'c' && char(preRet[iterColor + 2]) == 'x' && char(preRet[iterColor + 3]) == '/')
		{ // Have we reached the closing escape sequences?
		  inColor = false;
		  iter = iterColor + 3;
		}
	      else
		{ // Apply color information to character.
		  // If the character is not a "transparent space" character!
		  if(preRet[iterColor] != 10176)
		    { /* Non "transparent space" characters have color
			 information applied here. */
		      ret.push_back(preRet[iterColor] + cuMap.getRange(escSeq));
		    }
		  else
		    { /* Transparent space! (this character is a special case
			 and so color information should not be applied to
			 it). */
		      ret.push_back(preRet[iterColor]);
		    }
		}
	    }
	  inColor = true;
	  escSeq.clear();
	}
      else
	{ // Character with default color.
	  ret.push_back(preRet[iter]);
	}
    }
}
