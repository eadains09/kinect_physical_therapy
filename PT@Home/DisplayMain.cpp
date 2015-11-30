//
//  MainDisplay.cpp
//

#include "MainDisplay.h"


MainDisplay::MainDisplay() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

MainDisplay::MainDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r) : DisplayBase(c, w, r) {
	headerSurface = NULL;
	headerTexture = NULL;
}

bool MainDisplay::run() {

	// if (!init()) {
	// 	printf("Failed to initialize!\n");
	// 	return false;
	// }

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
                	handleButtonEvent(&e, gButtons.at(i));
				}
			}
		}

		renderScreen();

	}

	close();

	return true;
}

bool MainDisplay::init() {
	bool success;

	success = DisplayBase::init();

	return success;
}
    
bool MainDisplay::renderScreen() {
	renderFrame();

	return true;
}

bool MainDisplay::renderFrame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

	headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerDestR);
	SDL_DestroyTexture(headerTexture);
	
	renderButtons();

	SDL_RenderPresent(renderer);

	return true;
}

bool MainDisplay::loadMedia() {
	bool success = true;

	headerSurface = SDL_LoadBMP("art/MainDisplay/title.bmp");
	headerDestR.x = (SCREEN_WIDTH/2) - 150;
    headerDestR.y = 100;
    headerDestR.w = 300;
    headerDestR.h = 100;

	loadButtons();

	return success;
}

bool MainDisplay::loadButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_PHYSICIAN, 300, 100, 66, SCREEN_HEIGHT/2, "art/MainDisplay/physician.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_PATIENT, 300, 100, SCREEN_WIDTH - 366, SCREEN_HEIGHT/2, "art/MainDisplay/patient.bmp"));

	return true;
}

void MainDisplay::handleButtonEvent(SDL_Event* e, Button *currButton)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Mouse is inside button
		if ((*currButton).isInside(e))
		{
			buttonLog.open("buttonLogData.txt", std::ofstream::app);
			buttonLog << "Main Display: " << (*currButton).getType() << " button clicked" << std::endl;
			buttonLog.close();

			switch ((*currButton).getType()) {
				case BUTTON_SPRITE_PHYSICIAN:
					newDisplay = new PhysicianMenuDisplay(control, window, renderer);
					control.switchDisplays(&newDisplay);
					break;

				case BUTTON_SPRITE_PATIENT:
					break;
			}
		}
	}
}

void MainDisplay::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_d:
			//Load Physician screen
		break;

		case SDLK_p:
			//Load Patient screen
		break;
	}
}

void MainDisplay::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
    //closeSDL();
}

