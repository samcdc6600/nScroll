#include <iostream>
#include "create/create.h"


int main()
{
  std::cout<<"Welcome to sprite ed! \nWould you like to edit an existing"
	   <<" .sprite.dat file or create a new one? (e/n) \n";
  bool run{true};
  while(run)
    {
      char choice{};
      std::cin>>choice;
      if(choice == 'e' || choice == 'E')
	{//edit and existing file.
	  //edit();
	  std::cout<<"Sorry this option is not currently available.'\n";
	  run = false;
	}
      else
	if(choice == 'n' || choice == 'N')
	  {//creat a new file.
	    create();
	    run = false;
	  }
	else
	  {
	    if(choice == 'q' || choice == 'Q')
	      {//exit without editing or creating a file.
		std::cout<<"The user is fickle\n";
		run = false;
	      }
	    //valid input not entered.
	    std::cout<<"Sorry but "<<choice<<" does not constitue valid input. "
		     <<"You must must enter either y or n (existing or new)\n";
	  }
    }

  return 0;
}
