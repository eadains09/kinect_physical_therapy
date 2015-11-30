//
// PatientMenuDisplay.cpp
//

#include "DisplayPatientMenu.h"
#include "DisplayAction.h"
#include "DisplayMain.h"

PatientMenuDisplay::PatientMenuDisplay() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

PatientMenuDisplay::PatientMenuDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r) : DisplayBase(c, w, r) {
	headerSurface = NULL;
	headerTexture = NULL;
}

void PatientMenuDisplay::run() {
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

bool PatientMenuDisplay::renderScreen() {
	renderFrame();

	return true;
}

bool PatientMenuDisplay::renderFrame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

	headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerDestR);
	SDL_DestroyTexture(headerTexture);
	
	renderButtons();

	SDL_RenderPresent(renderer);

	return true;
}

bool PatientMenuDisplay::loadMedia() {
	bool success = true;

	headerSurface = SDL_LoadBMP("art/PatientMenu/PatientMenuHeader.bmp");
	headerDestR.x = (SCREEN_WIDTH/2) - 150;
    headerDestR.y = 100;
    headerDestR.w = 300;
    headerDestR.h = 100;

	loadButtons();

	return success;
}

bool PatientMenuDisplay::loadButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_BACK, 10, 10, "art/back.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_FULLWORKOUT, 300, 100, (SCREEN_WIDTH/2)-150, SCREEN_HEIGHT/2, "art/PatientMenu/FullWorkout.bmp"));

	return true;
}

void PatientMenuDisplay::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_w:
			//Load Exercise screen
			loadActionDisplay();
			break;

		case SDLK_BACKSPACE:
			loadPrevDisplay();
			break;
	}
}

void PatientMenuDisplay::handleButtonEvent(SDL_Event* e, Button *currButton) {
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
				case BUTTON_SPRITE_FULLWORKOUT:
					loadActionDisplay();
					break;

				case BUTTON_SPRITE_BACK:
					loadPrevDisplay();
					break;
			}
		}
	}
}

void PatientMenuDisplay::loadActionDisplay() {
	newDisplay = new ActionDisplay(control, window, renderer, LIVE_RECORD, PATIENT_MENU);
	loadNewDisplay();
}

void PatientMenuDisplay::loadPrevDisplay() {
	newDisplay = new MainDisplay(control, window, renderer);
	loadNewDisplay();
}

void PatientMenuDisplay::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
}
