#include <stdexcept>
#include "collapse.h"
#include "../../draw/colorS.h"
#include <ncurses.h>
//#include <sstream>

chMap::chMap()
{
  int chVal {128};//128 is one above the ascii character set
  for(std::string strIter: characters)//insert all the spcial character types into the map along with their key values
    {
      strMap[strIter]=chVal;//enter element into make with a key of strIter and a value of offset
      ++chVal;//increment chVal (each character must have a uniqe value not equal to any ascii value).
    }
}

int chMap::find(const std::string str)
{//map.at() throws and out of range exception if no matching key is found
  return strMap.at(str);//returns value that corresponds to the key str
}


void collapse(const std::string & buffWithSpaces, std::vector<int> & ret)
{//collaps special character escape sequences on first pass and colour escape sequences on second.
  ret.clear();//make suren that ret is empty.
  //declared as static so that the constructor will only be called once. This is an optimization since the constructor does
  static chMap escCharNums;//a lot of work that is redundent if done more then once.
  static colourMap cuMap;//static for the same reason as the last veriable. (this class converts to and from colour offsets).
  //---------------------------------(First pass (get rid of spaces))---------------------------------------------
  std::string buff {};
  for(int iter{}; iter < buffWithSpaces.size(); ++iter)
    {//get rid of spaces
      if(buffWithSpaces[iter] != '\n')
	buff.push_back(buffWithSpaces[iter]);
    }
  
  std::string escSeq{};
  std::vector<int> preRet{};
  //----------------------------------(Second pass (encode special characters))-------------------------------------
  for(int iter{}; iter < buff.size(); ++iter)//collaps "\nc/" ecsape sequences (special characters).
    {//if we have found the start of a nonColour escape sequence
      if(buff[iter] == '\\' && buff[1 + iter] == 'n' && buff[2 + iter] == 'c')
	{
	  iter += 3;//skip first three characters of escape sequencse ("\nc").
	  if(iter > buff.size())//check for errors in formatting
	    {//if iter is out side of range.
	      std::string eMsg {"in collaps.cpp ->collaps(const std::string & buffWithSpace, std::vector<int> ret). First pass. iter is > buff.size. Iter ="};
	      eMsg += iter;
	      eMsg += buff.size();
	      eMsg += '\n';
	      throw std::logic_error(eMsg);
	    }
	  for( ; buff[iter] != '/'; ++iter)
	    {//the next two if statments check for errors
	      if(iter > buff.size())
		{
		  std::string eMsg {"in collaps.cpp->collaps(const std::string & buffWithSpace, std::vector<int ret). iter ="};
		  eMsg += iter;       eMsg += "\n";
		  throw std::logic_error(eMsg);
		}
	      escSeq += buff[iter];//get nonColour escape sequence		
	    }
	  //set next element in preRet to int value that corresponds to escSeq nonColour escape sequence
	  preRet.push_back(escCharNums.find(escSeq));
	  escSeq.clear();//clear escSeq so that it is empty next time we find a nonColour escape sequence and use += on escSeq
	}
      else
	{//if we have found a normal character or a transparent space (no character is displayed) character. 
	  if(buff[iter] == '\\' && buff[1 + iter] == 't' && buff[2 + iter] == 'r' && buff[3 + iter] == 'a' && buff[4 + iter] == 'n'
	     && buff[5 + iter] == 's' && buff[6 + iter] == 'S' && buff[7 + iter] == 'P' && buff[8 + iter] == '/')
	    {
	      preRet.push_back(10176);//Value of transparent space is 1 higher then 159 * 64 whitch is above the space of all ACS and ASCII characters whether colored or not.
	      iter += 8;
	    }
	  else
	    preRet.push_back(buff[iter]);
	}
    }

  //------------------------------(Third pass (encode color characters))-------------------------------------------
  for(int iter{}; iter < preRet.size(); ++iter)
    {
      if(char(preRet[iter]) == '\\' && char(preRet[1 + iter]) == 'c' && char(preRet[2 + iter]) == 'u')
	{//we are in the start of a color escape sequence
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
	      escSeq += preRet[iter];//escSeq holds the string used to get the color code cuMap
	    }//we have the color information (in escSeq)
	  iter++;
	  bool inColor {true};
	  for(int iterColor{iter}; inColor; ++iterColor)
	    {//now apply color information to characters
	      if((iterColor - 3) >= preRet.size())
		{
		  mvprintw(0, 0, "iterColor = %d, preRet.size() = %d ", iterColor, preRet.size());
			   getch();
		  throw std::logic_error("if(iterColor >= preRet.size())");
		}
	      if(char(preRet[iterColor]) == '\\' && char(preRet[iterColor + 1]) == 'c' && char(preRet[iterColor + 2]) == 'x' && char(preRet[iterColor + 3]) == '/')
		{//have we reached the closing escape sequences?
		  inColor = false;
		  iter = iterColor + 3;
		}
	      else
		{//apply color information to character
		  if(preRet[iterColor] != 10176)//if the character is not a "transparent space" character!
		    {//non "transparent space" characters have color information applied here.
		      ret.push_back(preRet[iterColor] + cuMap.getRange(escSeq));
		    }
		  else
		    {//transparent space! (this character is a special case and so color information should not be applied to it).
		      ret.push_back(preRet[iterColor]);
		    }
		}
	    }
	  inColor = true;
	  escSeq.clear();
	}
      else
	{//character with default color
	  ret.push_back(preRet[iter]);
	}
    }
}
