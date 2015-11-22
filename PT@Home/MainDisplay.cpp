//
//  MainDisplay.cpp
//

#include "MainDisplay.h"


MainDisplay::MainDisplay() {
	DisplayBase::DisplayBase();

	headerSurface = NULL;
	headerTexture = NULL;
}

bool MainDisplay::run() {

	if (!init()) {
		printf("Failed to initialize!\n");
		return false;
	}

	//Load media
	if (!loadMedia()) {
		printf("Failed to load media!\n");
		return false;
	}

	runLoop();

	close();
}

virtual bool MainDisplay::init() {
	bool success;

	success = DisplayBase::init();
}
    
virtual bool MainDisplay::renderScreen() {
	renderFrame();
}

virtual bool MainDisplay::renderFrame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

	headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerDestR);

	renderButtons();

	SDL_RenderPresent(renderer);

	return true;

}

virtual bool MainDisplay::loadMedia() {
	bool success = true;

	headerSurface = SDL_LoadBMP("art/MainDisplay/title.bmp");
	headerDestR.x = 100;
    headerDestR.y = 50;
    headerDestR.w = 300;
    headerDestR.h = 100;

	loadButtons();

	return success;
}

virtual bool MainDisplay::loadButtons() {

	gButtons[0] = new Button(BUTTON_SPRITE_PHYSICIAN, 300, 100, 50, 200, "art/MainDisplay/physician.bmp");
    gButtons[1] = new Button(BUTTON_SPRITE_PATIENT, 300, 100, SCREEN_WIDTH-350, 200, "art/MainDisplay/patient.bmp");

}

virtual void MainDisplay::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_d:
			//Load Physician screen
		break;

		case SDLK_p:
			//Load Patient screen
		break;
	}
}

virtual void MainDisplay::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
    closeSDL();
}

