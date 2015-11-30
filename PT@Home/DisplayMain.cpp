//
//  MainDisplay.cpp
//

#include "DisplayMain.h"


DisplayMain::DisplayMain() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

bool DisplayMain::run() {

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
					//(*gButtons.at(i)).handleEvent(&e);
				}
			}
		}

		renderScreen();

	}

	close();

	return true;
}

bool DisplayMain::init() {
	bool success;

	success = DisplayBase::init();

	return success;
}
    
bool DisplayMain::renderScreen() {
	renderFrame();

	return true;
}

bool DisplayMain::renderFrame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

	headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerDestR);
	SDL_DestroyTexture(headerTexture);
	
	renderButtons();

	SDL_RenderPresent(renderer);

	return true;
}

bool DisplayMain::loadMedia() {
	bool success = true;

	headerSurface = SDL_LoadBMP("art/MainDisplay/title.bmp");
	headerDestR.x = (SCREEN_WIDTH/2) - 150;
    headerDestR.y = 100;
    headerDestR.w = 300;
    headerDestR.h = 100;

	loadButtons();

	return success;
}

bool DisplayMain::loadButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_PHYSICIAN, 300, 100, 66, SCREEN_HEIGHT/2, "art/MainDisplay/physician.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_PATIENT, 300, 100, SCREEN_WIDTH - 366, SCREEN_HEIGHT/2, "art/MainDisplay/patient.bmp"));

	return true;
}

void DisplayMain::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_d:
			//Load Physician screen
		break;

		case SDLK_p:
			//Load Patient screen
		break;
	}
}

void DisplayMain::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
    closeSDL();
}

