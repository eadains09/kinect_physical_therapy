//
// Controller.h
//

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

class Controller {

private:
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