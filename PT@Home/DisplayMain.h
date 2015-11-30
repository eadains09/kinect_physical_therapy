//
//  MainDisplay.h
//

#ifndef _MAIN_DISPLAY_H_
#define _MAIN_DISPLAY_H_

#include "DisplayBase.h"
#include "Controller.h"

class DisplayMain : public DisplayBase {
private:
	SDL_Surface* headerSurface;
	SDL_Texture* headerTexture;
	SDL_Rect headerDestR;

	std::ofstream log, buttonLog;
    
    virtual bool renderScreen();
    virtual bool renderFrame();
    virtual bool loadMedia();    
    virtual bool loadButtons();
    virtual void handleKeyPresses(SDL_Event e);
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton);
    void loadPhysicianScreen();
    void loadPatientScreen();
    void loadNewDisplay();


    virtual void close();

public:
	MainDisplay();
	MainDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r);
	virtual void run();
};

#endif /* _MAIN_DISPLAY_H_ */