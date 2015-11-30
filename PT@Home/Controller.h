//
// Controller.h
//

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

//#include "DisplayBase.h"
#include <SDL.h>

class DisplayBase;

class Controller {

private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	DisplayBase *view;
	SDL_Window *window;
	SDL_Renderer *renderer;

public:
	Controller();
	void runDisplay();
	void switchDisplays(DisplayBase* newDisplay);
	void closeDisplay();
	bool init();

};

#endif /* _CONTROLLER_H_ */