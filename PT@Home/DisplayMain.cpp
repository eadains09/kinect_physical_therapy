//
//  MainDisplay.cpp
//

#include "DisplayMain.h"
#include "DisplayPhysicianMenu.h"
#include "DisplayPatientMenu.h"


DisplayMain::DisplayMain() : DisplayBase() {
	headerSurface = NULL;
	headerTexture = NULL;
}

DisplayMain::DisplayMain(Controller *c, SDL_Window *w, SDL_Renderer *r) : DisplayBase(c, w, r) {
	headerSurface = NULL;
	headerTexture = NULL;
}

void DisplayMain::run() {
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
				for (size_t i = 0; i < gButtons.size(); i++) {
                	handleButtonEvent(&e, gButtons.at(i));
				}
			}
		}
		// Extra quit check necessary because quitting in a different display waterfalls up the path
		// yet the controller will have already closed the textures/buttons/etc of the previous displays
		if (!quit) {
			renderScreen();
		}
	}
}
 //if all this does is call renderFrame(), neither have arguments,
//and both always return true, do we really need renderScreen()?
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

void DisplayMain::handleButtonEvent(SDL_Event* e, Button *currButton)
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
					loadPhysicianScreen();
					break;
				case BUTTON_SPRITE_PATIENT:
					loadPatientScreen();
					break;
				default:
					break;
			}
		}
	}
}

void DisplayMain::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_d:
			loadPhysicianScreen();
			break;
		case SDLK_p:
			loadPatientScreen();
			break;
	}
}

void DisplayMain::loadPhysicianScreen() {
	newDisplay = new PhysicianMenuDisplay(control, window, renderer);
	loadNewDisplay();
}

void DisplayMain::loadPatientScreen() {
	newDisplay = new PatientMenuDisplay(control, window, renderer);
	loadNewDisplay();
}

void DisplayMain::loadNewDisplay() {
	(*control).switchDisplays(newDisplay);
	quit = true; // Necessary for when control waterfalls back up the chain of displays loading, which will only happen when program is closing
}

void DisplayMain::close() {
	SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);

    closeButtons();
    //closeSDL();
}

