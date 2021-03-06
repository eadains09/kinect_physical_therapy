//
//  DisplayBase.cpp
//

#include "DisplayBase.h"

DisplayBase::DisplayBase() {
	window = NULL;
	renderer = NULL;
}

DisplayBase::DisplayBase(Controller *c, SDL_Window *w, SDL_Renderer *r) {
    window = w;
    renderer = r;
    control = c;
}


// bool DisplayBase::init() {
// 	bool success = true;

// 	//Initialize SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
//         success = false;
//     } else {
//         //Create window
//         window = SDL_CreateWindow("Kinect Physical Therapy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//         if (window == NULL) {
//             printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//             success = false;
//         } else {
//             renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//             if (renderer == NULL) {
//                 printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
//                 success = false;
//             } else {
//                 //Initialize renderer color
//                 SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
//             }
//         }
//     }
    
//     return success;
// }

void DisplayBase::loadNewDisplay() {
    (*control).switchDisplays(newDisplay);
    quit = true;
}

void DisplayBase::renderButtons() {
    for (size_t i = 0; i < gButtons.size(); i++) {
        (*gButtons.at(i)).render(renderer);
    }
}

void DisplayBase::closeSDL() {
	//closeButtons();

	TTF_CloseFont(currFont);
	currFont = NULL;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
	TTF_Quit();
    SDL_Quit();
}

void DisplayBase::closeButtons() {
	for (size_t i = 0; i < gButtons.size(); i++) {
        (gButtons.at(i))->freeButton();
    }
}


