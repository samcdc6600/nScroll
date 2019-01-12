#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "create.h"
#include "../io/fileName.h"

//file should contain the following:
//*slides that hold sprite animation images.
//*internal slide cycle speed.
void create()
{
  bool run {true};
  while(run)
    {//get information
      std::cout<<"What number would you like to assign to you new .sprite.dat file?";
      unsigned int fileNum{};
      std::cin>>fileNum;
      std::cout<<"How many animation slides do you want to create?";
      unsigned int slideNum{};
      std::cin>>slideNum;
      std::vector<std::string> slides;
      std::cout<<"How fast would you like the internal slide cycle speed to be\n"
	       <<"note slide speed is mesured in ms?\n";
      unsigned int sS{};
      std::cin>>sS;
      std::cout<<"please enter slides. Note: enter q at the end of a line to finish\n";
      std::cout<<"the current slide. If q is not entered on an empty line \n";
      std::cout<<"(exception below) then the line will be discarded! If you wish to\n";
      std::cout<<"enter the letter 'q' at the end\n";
      std::cout<<"of a line for use in a slide use '\\'\n";

      //get slice
      for(int iter {}; iter < slideNum; ++iter)//this needs more work
	{
	  std::string workingSlice {};	//slice currently being entered
	  std::string workingLn {};	//line currently being entered
	  //does the user want to keep entering lines into the current slice?
	  bool enterSlice {true};	
	  while(enterSlice)
	    {	     
	      std::getline(std::cin, workingLn);
	      //end() returns a past-the-end iterator so must be decremented!
	      if(workingLn.size() == 1)//check for special case to avoid buffer over flow
		{
		  if(*(workingLn.end() -1) == 'q')
		    {
		      enterSlice = false;
		      goto endSliceWhile;//don't add line to slice
		    }
		}
	      else
		if(workingLn.size() < 1)
		  {
		    if(*(workingLn.end() -1) == 'q')
		      if(*(workingLn.end() -2) == '\\')
			{//if we want to have the 'q' character at the end of the line
			  workingLn.pop_back();
			  workingLn.pop_back();
			  workingLn.push_back('q');
			}
		      else
			{//if we wish to finish adding lines to this slice
			  enterSlice = false;
			  goto endSliceWhile;//don't add line to slice
			}
		  }
	      workingSlice += workingLn;
	      workingSlice += '\n';	      
	    endSliceWhile:
	      //clear the working line for use on the next iteration.
	      workingLn.clear();
	    }
	  enterSlice = true;//we wan't to enter more slices
	  slides[iter] = workingSlice;
	  workingSlice.clear();
	}
  
      //display information
      std::cout<<"Are you satisfied with the entrie's? (y/n)\n";
      char en{};
      std::cin>>en;      
      if(en == 'y' || en == 'Y')
	{
	  run = false;
	}
      else
	if(en == 'n' || en == 'N')
	  continue;
	else
	  {
	    std::cout<<"Invalid information entered. Y assumed. Writing file";
	    run = false;
	  }
    }

  //create file with info
  //open file (data) with name (start + n(converted to string) + end)
  std::cout<<"File created!\n";
}
