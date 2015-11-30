//
//  DisplayBase.h
//

#ifndef _DISPLAY_BASE_H_
#define _DISPLAY_BASE_H_

#include <vector>
#include "Button.h"


class DisplayBase {
protected:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	
	// TODO I think these should be static
	SDL_Window* window;  //The window we'll be rendering to
	SDL_Renderer* renderer;
    Controller* control;
    DisplayBase* newDisplay;
	
    std::vector<Button*> gButtons;

    virtual bool init(); // Init's SDL window and renderer only - should be called by inheriting classes

    virtual bool renderScreen() = 0; // Class specific what should go on the screen other than buttons    
    virtual bool renderFrame() = 0; // Clears screen and renders buttons
    virtual bool loadMedia() = 0;
    virtual bool loadButtons() = 0;
    virtual void handleKeyPresses(SDL_Event e) = 0;
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton) = 0;

    virtual void close() = 0;

    void renderButtons();
    void flashScreen();
    void closeSDL();
    void closeButtons();

public:
	DisplayBase();
    DisplayBase(SDL_Window *w, SDL_Renderer *r);
	//Does render frame need to go here?


};

#endif /* _DISPLAY_BASE_H_ */