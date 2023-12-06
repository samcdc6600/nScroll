#include "include/initSDL.hpp"


namespace SDL_Constants
{
  const int SCREEN_WIDTH {640};
  const int SCREEN_HEIGHT {480};
}


void createWindow()
{
  // A pointer to the window we will be rendering to...
  SDL_Window * window = nullptr;

  // A pointer to the surface contained by the window...
  SDL_Surface * screenSurface = nullptr;

  // Here we initialise SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      std::cout<<"Error: could not initialise SDL. SDL_Error: "
	       <<SDL_GetError()<<"\n";
    }
  else
    {
      // Create a window.
      window = SDL_CreateWindow
	("Scrollio Extremeo!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	 SDL_Constants::SCREEN_WIDTH,
	 SDL_Constants::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
      if(window == nullptr)
	{
	  std::cout<<"Error: could not create window. SDL_Error: "
		   <<SDL_GetError()<<"\n";
	}
      else
	{
	  // Get windows surface.
	  screenSurface = SDL_GetWindowSurface(window);

	  // Fill the surface with a color.
	  SDL_FillRect(screenSurface, nullptr, SDL_MapRGB
		       (screenSurface->format, 0x00, 0xFF, 0x00));

	  // Update the surface.
	  SDL_UpdateWindowSurface(window);

	  // Hack to get the window to stay up.
	   SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
	}
    }

  // Destroy the window.
  SDL_DestroyWindow(window);

  // Quit SDL subsystems.
  SDL_Quit();
}
