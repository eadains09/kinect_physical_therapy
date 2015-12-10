//
//  PatientMenuDisplay.h
//

#ifndef _PATIENT_MENU_DISPLAY_H_
#define _PATIENT_MENU_DISPLAY_H_

#include "DisplayBase.h"
#include "DisplayAction.h"


class PatientMenuDisplay : public DisplayBase {
private:
	SDL_Surface* headerSurface;
	SDL_Texture* headerTexture;
	SDL_Rect headerDestR;

	OPENFILENAME playbackPFile;
	char szPFile[100]; // memory buffer to contain file name
	std::ofstream log, buttonLog;
    
    virtual bool renderScreen();
    bool renderFrame();
    virtual bool loadMedia();    
    virtual bool loadButtons();
    virtual void handleKeyPresses(SDL_Event e);
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton);
    void loadActionDisplay();
	void loadActionDisplay(std::string file);
    void loadPrevDisplay();
	void initFileSelector();
    virtual void close();

public:
	PatientMenuDisplay();
	PatientMenuDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r);

	virtual void run();
};
#endif /* _PATIENT_MENU_DISPLAY_H_*/