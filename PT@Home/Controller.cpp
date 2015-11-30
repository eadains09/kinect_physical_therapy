//
// Controller.cpp
//

#include "Controller.h"
#include "DisplayMain.h"
#include <stdio.h>

Controller::Controller() {
	if (!init()) {
		printf("Failed to initialize!\n");
		window = NULL;
		renderer = NULL;
		view = NULL;
	} else {
		view = new MainDisplay(this, window, renderer);
	}
}

void Controller::runDisplay() {
	(*view).run();
}

void Controller::switchDisplays(DisplayBase* newDisplay) {
	(*view).close();
	view = newDisplay;
	(*view).run();

}

void Controller::closeDisplay() {
	(*view).close();
	(*view).closeSDL();
}

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
            }
        }
    }
    
    return success;
}