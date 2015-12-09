//
// DisplayAction.h
//

#ifndef _DISPLAY_ACTION_H_
#define _DISPLAY_ACTION_H_

#include "BodyFrame.h"
#include "DisplayBase.h"
#include "FileWriter.h"
#include "Movement.h"

const int TOTAL_BODIES = 2;

enum PlaybackType {
	LIVE = 0,
	RECORDED = 1,
	LIVE_RECORD = 2
};

class ActionDisplay : public DisplayBase {
private:
	// displaying bodies variables
	int frameNumber; //Which frame to read from file
	int bodyCount; //Number of bodies being displayed, right now only options are 1 or 2
	bool playing;
	Movement *moveFromFile;
	BodyFrame *displayBodies;
	QuatFrame *displayQuats;
	//BodyFrame displayBodies[TOTAL_BODIES];
	PlaybackType playback;
	string playbackFile;
	string playFileName;
	time_t beginningTimestamp;
	time_t beginPauseTime;
	double pauseTime;
	bool comparisonOn;
	int exerciseCount;

	// navigation variable
	DisplayType prevScreen;

	// instruction variables
	SDL_Surface* instructionSurface;
	SDL_Texture* instructionTexture;
	SDL_Rect instructionDestR;

	// capturing keyframe variables
	static int saveCount;
	bool keyframeCaptured;
	time_t prevTime = NULL;
	BodyFrame prevKeyframe;
	Movement keyframes;
	FileWriter writer;
	OPENFILENAME saveFile;
	char szFile[100]; // memory buffer to contain file name


	// kinect capturing variables
	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;

	std::ofstream log, buttonLog;

	void constructUniversalActionDisplay();
	virtual bool renderScreen(); // Class specific what should go on the screen other than buttons    
    virtual bool renderFrame();
    virtual void handleKeyPresses(SDL_Event e);
    virtual void handleButtonEvent(SDL_Event* e, Button *currButton);
    virtual bool loadMedia();
	bool loadText(std::string textureText, SDL_Color textColor);
    virtual bool loadButtons();
	string trimAddress(string pfile);


    bool init();
    void renderBody(BodyFrame currBody);
    bool frameFromKinect();
    bool getSingleFrameFromFile(double elapsedTime);

    void captureKeyframe();
    void deleteLastKeyframe();
    void saveKeyframes();
	void initFileSelector();


    void loadPrevDisplay();
    void togglePlaying();
    void loadKeyframeButtons(int yPos);
    void loadPlaybackButtons(int yPos);

public:
	ActionDisplay();
	ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d);
	ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d, string pfile);

	
	virtual void run();
	virtual void close();
};

int getParent(int);

#endif /* _DISPLAY_ACTION_H_ */