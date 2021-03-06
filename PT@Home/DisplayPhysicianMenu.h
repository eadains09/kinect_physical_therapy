//
//  PhysicianMenuDisplay.h
//

#ifndef _PHYSICIAN_MENU_DISPLAY_H_
#define _PHYSICIAN_MENU_DISPLAY_H_

#include "DisplayBase.h"
#include "DisplayAction.h"

class PhysicianMenuDisplay : public DisplayBase {
private:
	SDL_Surface* headerSurface;
	SDL_Texture* headerTexture;
	SDL_Rect headerDestR;

	OPENFILENAME playbackFile;
	char szFile[100]; // memory buffer to contain file name
	std::ofstream log, buttonLog;
    
    virtual bool renderScreen();
    bool renderFrame();
    virtual bool loadMedia();    
    virtual bool loadButtons();
    virtual void handleKeyPresses(SDL_Event e);
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton);
    void loadActionDisplay(PlaybackType playType);
	void loadActionDisplay(PlaybackType playType, string file);
    void loadPrevDisplay();
	void initFileSelector();

    virtual void close();

public:
	PhysicianMenuDisplay();
	PhysicianMenuDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r);

	virtual void run();
};
#endif /* _PHYSICIAN_MENU_DISPLAY_H_*/