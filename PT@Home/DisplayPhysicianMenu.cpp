//
// PhysicianMenuDisplay.cpp
//

#include "DisplayPhysicianMenu.h"
#include "DisplayMain.h"

PhysicianMenuDisplay::PhysicianMenuDisplay() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

PhysicianMenuDisplay::PhysicianMenuDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r) : DisplayBase(c, w, r) {
	headerSurface = NULL;
	headerTexture = NULL;
}

void PhysicianMenuDisplay::run() {
	//Load media
	if (!loadMedia()) {
		printf("Failed to load media!\n");
		exit(0);
	}

	//Main loop flag
	quit = false;
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

		if (!quit) {
			renderScreen();
		}
	}
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

	SDL_RenderPresent(renderer);

	return true;
}

bool PhysicianMenuDisplay::loadMedia() {
	bool success = true;

	headerSurface = SDL_LoadBMP("art/PhysicianMenu/PhysicianMenuHeader.bmp");
	headerDestR.x = (SCREEN_WIDTH/2) - 150;
    headerDestR.y = 100;
    headerDestR.w = 300;
    headerDestR.h = 100;

	loadButtons();

	return success;
}

bool PhysicianMenuDisplay::loadButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_BACK, 10, 10, "art/back.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_KEYFRAME, 300, 100, 66, SCREEN_HEIGHT/2, "art/PhysicianMenu/CaptureKeyframes.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_PLAYBACK, 300, 100, SCREEN_WIDTH - 366, SCREEN_HEIGHT/2, "art/PhysicianMenu/PlaybackMovement.bmp"));

	return true;
}

void PhysicianMenuDisplay::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_k:
			//Load Keyframe screen
			loadActionDisplay(LIVE);
			break;

		case SDLK_p:
			//Load Playback screen
			loadActionDisplay(RECORDED);
			break;

		case SDLK_BACKSPACE:
			loadPrevDisplay();
			break;
	}
}

void PhysicianMenuDisplay::handleButtonEvent(SDL_Event* e, Button *currButton) {
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Mouse is inside button
		if ((*currButton).isInside(e))
		{
			buttonLog.open("buttonLogData.txt", std::ofstream::app);
			buttonLog << "Physician Menu: " << (*currButton).getType() << " button clicked" << std::endl;
			buttonLog.close();

			switch ((*currButton).getType()) {
				case BUTTON_SPRITE_KEYFRAME:
					loadActionDisplay(LIVE);
					break;

				case BUTTON_SPRITE_PLAYBACK:
					loadActionDisplay(RECORDED);
					break;

				case BUTTON_SPRITE_BACK:
					loadPrevDisplay();
					break;
			}
		}
	}
}

void PhysicianMenuDisplay::loadActionDisplay(PlaybackType playType) {
	newDisplay = new ActionDisplay(control, window, renderer, playType, PHYSICIAN_MENU);
	loadNewDisplay();
}

void PhysicianMenuDisplay::loadPrevDisplay() {
	newDisplay = new MainDisplay(control, window, renderer);
	loadNewDisplay();
}

// void PhysicianMenuDisplay::loadNewDisplay() {
// 	control.switchDisplays(&newDisplay);
// 	quit = true;
// }


void PhysicianMenuDisplay::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
}

