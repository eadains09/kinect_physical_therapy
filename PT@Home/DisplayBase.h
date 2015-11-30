//
//  DisplayBase.h
//

#ifndef _DISPLAY_BASE_H_
#define _DISPLAY_BASE_H_

#include <vector>
#include "Button.h"
#include "Controller.h"

class DisplayBase {
protected:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	
	// TODO I think these should be static
	SDL_Window* window;  //The window we'll be rendering to
	SDL_Renderer* renderer;
    Controller* control;
    DisplayBase* newDisplay;
	bool quit;

    std::vector<Button*> gButtons;

    virtual bool init(); // Init's SDL window and renderer only - should be called by inheriting classes

    virtual bool renderScreen() = 0; // Class specific what should go on the screen other than buttons    
    virtual bool renderFrame() = 0; // Clears screen and renders buttons
    virtual bool loadMedia() = 0;
    virtual bool loadButtons() = 0;
    virtual void handleKeyPresses(SDL_Event e) = 0;
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton) = 0;


    void renderButtons();
    void flashScreen();
    void closeButtons();

public:
	DisplayBase();
    DisplayBase(Controller *c, SDL_Window *w, SDL_Renderer *r);
	void closeSDL();
	virtual void close() = 0;
	virtual void run() = 0;

	//Does render frame need to go here?


};

#endif /* _DISPLAY_BASE_H_ */