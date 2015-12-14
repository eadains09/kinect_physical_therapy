/*
 *  File:         Controller.cpp
 *
 *  Purpose:      Created as a way to use just one SDL_Window and one SDL_Renderer in all Display classes. Badly inspired by MVC.
 *
 *  Member Vars:  SDL_Window, SDL_Renderer, DisplayBase view - this is a pointer to whichever Display class is currently active.
 */

#include "Controller.h"
#include "DisplayMain.h"
#include <stdio.h>

// Constructor - calls init method, if it returns true, calls DisplayMain constructor and sets view to point to that instance. 
// Else it sets all pointers to NULL and does not launch a window.
Controller::Controller() {
	if (!init()) {
		window = NULL;
		renderer = NULL;
		view = NULL;
	} else {
		view = new DisplayMain(this, window, renderer);
	}
}

// Calls the run method of whichever Display screen is currently active. 
// Run is a virtual method of DisplayBase, so all child classes will have one.
void Controller::runDisplay() {
	(*view).run();
}

// Closes the display currently active, and activates the new display being passed in by setting that display to view and 
// calling its run method, therby transferring program control to the new display
void Controller::switchDisplays(DisplayBase* newDisplay) {
	(*view).close();
	view = newDisplay;
	(*view).run();

}

/* Called (indirectly) once the windows x or close button has been clicked, will close the current display, as well as SDL
 * Because of the way the transfers of control work (probably not ideally) this function will not be called until all previous displays
 * that have ever been run have exited their run functions, control will traverse backwards from current display to first display,
 * then back to main function, which calls this function.
 */
void Controller::closeDisplay() {
	(*view).close();
	(*view).closeSDL();
}

// Inits SDL variables (Window, Renderer, and font library) that will be universal to all display screens, returns true if they are all
// successfully initialized, false otherwise.
bool Controller::init() {
	bool success = true;

	//Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow("Kinect Physical Therapy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);


				if (TTF_Init() != 0) {
					printf("SDL_ttf could not initialize. SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
            }
        }
    }
    
    return success;
}
