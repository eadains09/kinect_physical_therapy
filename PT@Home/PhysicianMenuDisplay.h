//
//  PhysicianMenuDisplay.h
//

#ifndef _PHYSICIAN_MENU_DISPLAY_H_
#define _PHYSICIAN_MENU_DISPLAY_H_

#include "DisplayBase.h"

class PhysicianMenuDisplay : public DisplayBase {
private:
	SDL_Surface* headerSurface;
	SDL_Texture* headerTexture;
	SDL_Rect headerDestR;

	std::ofstream log;



	virtual bool init(); // Must call parent constructor
    
    virtual bool renderScreen();
    virtual bool renderFrame();
    virtual bool loadMedia();    
    virtual bool loadButtons();
    virtual void handleKeyPresses(SDL_Event e);
    virtual void close();

public:
	PhysicianMenuDisplay();
	bool run();
};
#endif /* _PHYSICIAN_MENU_DISPLAY_H_*/