//
//  MainDisplay.h
//

#ifndef _MAIN_DISPLAY_H_
#define _MAIN_DISPLAY_H_

#include "DisplayBase.h"

class DisplayMain : public DisplayBase {
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
	DisplayMain();
	bool run();

};

#endif /* _MAIN_DISPLAY_H_ */