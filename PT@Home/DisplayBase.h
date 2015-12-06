//
//  DisplayBase.h
//

#ifndef _DISPLAY_BASE_H_
#define _DISPLAY_BASE_H_

#include <vector>
#include "Button.h"
#include "Controller.h"

const static int SCREEN_WIDTH = 800;
const static int SCREEN_HEIGHT = 600;

enum DisplayType {
    DISPLAY_MAIN = 0,
    PHYSICIAN_MENU = 1,
    PATIENT_MENU = 2,
    ACTION_DISPLAY = 3
};

class DisplayBase {
protected:
	SDL_Window* window;  //The window we'll be rendering to
	SDL_Renderer* renderer;
    Controller* control;
    DisplayBase* newDisplay;
	bool quit;

    std::vector<Button*> gButtons;
	TTF_Font *currFont = NULL;
    // virtual bool init(); // Init's SDL window and renderer only - should be called by inheriting classes

    virtual bool renderScreen() = 0; // Class specific what should go on the screen other than buttons    
    virtual bool renderFrame() = 0; // Clears screen and renders buttons
    virtual bool loadMedia() = 0;
    virtual bool loadButtons() = 0;
    virtual void handleKeyPresses(SDL_Event e) = 0;
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton) = 0;

    void loadNewDisplay();
    void renderButtons();
    void closeButtons();

public:
	DisplayBase();
    DisplayBase(Controller *c, SDL_Window *w, SDL_Renderer *r);

	virtual void close() = 0;
	virtual void run() = 0;

    void closeSDL();

};

#endif /* _DISPLAY_BASE_H_ */