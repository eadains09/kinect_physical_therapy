//
// PhysicianMenuDisplay.cpp
//

#include "PhysicianMenuDisplay.h"

PhysicianMenuDisplay::PhysicianMenuDisplay() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

bool PhysicianMenuDisplay::run() {
	if (!init()) {
		printf("Failed to initialize!\n");
		return false;
	}

	//Load media
	if (!loadMedia()) {
		printf("Failed to load media!\n");
		return false;
	}

	//runLoop();
	//Main loop flag
	bool quit = false;
	//Event Handler
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				handleKeyPresses(e);
			} else {
				for (int i = 0; i < gButtons.size(); i++) {
					(*gButtons.at(i)).handleEvent(&e);
				}
			}
		}

		renderScreen();

	}

	close();

	return true;
}

bool PhysicianMenuDisplay::init() {
	bool success;

	success = DisplayBase::init();

	return success;
}

bool PhysicianMenuDisplay::renderScreen() {
	renderFrame();

	return true;
}

bool PhysicianMenuDisplay::renderFrame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

	headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerDestR);
	SDL_DestroyTexture(headerTexture);
	
	renderButtons();
	once = false;

	SDL_RenderPresent(renderer);

	return true;
}

bool PhysicianMenuDisplay::loadMedia(); 

bool PhysicianMenuDisplay::loadButtons();

void PhysicianMenuDisplay::handleKeyPresses(SDL_Event e);

void PhysicianMenuDisplay::close();

