//
// DisplayAction.cpp
//

#include <Kinect.h>
#include <quaternion.h>
#include <time.h>
#include "DisplayAction.h"
#include "DisplayMain.h"
#include "DisplayPatientMenu.h"
#include "DisplayPhysicianMenu.h"
#include "stdafx.h"
#include "QuatFrame.h"

int ActionDisplay::saveCount = 0;

ActionDisplay::ActionDisplay() : DisplayBase() {
	frameNumber = 0;
	keyframeCaptured = false;
	playing = true;
	playback = LIVE;
	bodyCount = 1;
	prevScreen = DISPLAY_MAIN;

	displayBodies = new BodyFrame[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayBodies[i] = BodyFrame();

	displayQuats = new QuatFrame[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayQuats[i] = QuatFrame();


	log.open("logData.txt");
}

ActionDisplay::ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d) : DisplayBase(c, w, r) {
	frameNumber = 0;
	keyframeCaptured = false;
	playing = true;
	playback = p;
	prevScreen = d;

	displayBodies = new BodyFrame[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayBodies[i] = BodyFrame();

	if (playback == LIVE || playback == RECORDED) {
		bodyCount = 1;
	}
	else {
		//simultaneous playback
		bodyCount = 2;
	}

	log.open("logData.txt");
}

bool ActionDisplay::init() {
	bool success = true;
	IBodyFrameSource* pBodyFrameSource = NULL;

	GetDefaultKinectSensor(&m_pKinectSensor);
	if (playback == LIVE || playback == LIVE_RECORD)
	{
		if (!m_pKinectSensor ||
			!SUCCEEDED(m_pKinectSensor->Open()) ||
			!SUCCEEDED(m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper)) ||
			!SUCCEEDED(m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource)) ||
			!SUCCEEDED(pBodyFrameSource->OpenReader(&m_pBodyFrameReader)))
		{
			log << "Could not find a connected kinect\n" << std::endl;
			return false;
		}
		SafeRelease(pBodyFrameSource);
	}

	return success;
}

void ActionDisplay::run() {
	if (!init()) {
		printf("Failed to initialize Kinect!\n");
		exit(0);
	}

	if(!loadMedia()) {
		printf("Failed to load media!\n");
		exit(0);
	}

	quit = false;
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				handleKeyPresses(e);
			} else {
            	for (size_t i = 0; i < gButtons.size(); i++) {
					handleButtonEvent(&e, gButtons.at(i));
            	}
            }
		}
		
		if (!quit) {
			renderScreen();
		}
	}
}

bool ActionDisplay::renderScreen() {
	if (playing) {
		if (playback == LIVE) {
			frameFromKinect();
			displayBodies[bodyCount - 1].transformPoints();
		}
		else if (playback == RECORDED) {
			//I think the best way to do this might be to change
			//single frame from file to deal with slerping
			getSingleFrameFromFile();
			displayQuats[0].initBodyFrame(displayBodies[0]);
		}
		else {
			//so this is the bit where we'll have to 
			//compare, and thus is the only place in which
			//it necessary to convert live kinect frames
			//to quaternions, I would suggest (a) new function(s)
			//that deal(s) with one to all of those things

			//simultaneous playback
			getSingleFrameFromFile();
			frameFromKinect();
		}
		frameNumber++;
	}

	renderFrame();
	SDL_Delay(50);

	return true;
}

bool ActionDisplay::renderFrame() {
	int i;
    int colorArray[2] = {0x00, 0xFF};

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    
    renderButtons();
    
    //render bodies
	for (i = 0; i < bodyCount; i++) {
		if (displayBodies[i].getCurrJointCount() != JOINT_TOTAL)
			continue;
	
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i % 2], 0xFF);
		renderBody(displayBodies[i]);
	}

	if (keyframeCaptured) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i % 2], 0xFF);
		renderBody(prevKeyframe);
	}

    SDL_RenderPresent(renderer);

    return true;
}

void ActionDisplay::renderBody(BodyFrame currBody) {
	irr::core::vector3df **joints = currBody.getJoints();

	for (int i = 0; i < currBody.getCurrJointCount(); i++)
		if (getParent(i) != i)
			SDL_RenderDrawLine(renderer, joints[i]->X, joints[i]->Y, joints[getParent(i)]->X, joints[getParent(i)]->Y);

}

bool ActionDisplay::frameFromKinect()
{
	HRESULT hr;
	int j;
	BodyFrame *anorexia = NULL;
	IBodyFrame* pBodyFrame = NULL;
	INT64 nTime = 0;

	if (!m_pBodyFrameReader)
		return false;

	hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (!pBodyFrame || !SUCCEEDED(hr))
		return false;

	Joint *joints = new Joint[JointType_Count];
	IBody* ppBodies[BODY_COUNT] = { 0 };

	hr = pBodyFrame->get_RelativeTime(&nTime);
	//check?
	hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
	//check?

	//So here we have an interesting issue
	//I'm going through ppbodies which supports several(6) bodies,
	//many of which are going to be bogus, but we can put only one
	//of these bodies into displayBodies, so we need some way of choosing a
	//body that is likely to be valid, ideally choosing the body that the
	//the most likly to be valid
	for (j = 0; j < _countof(ppBodies); j++)
	{
		BOOLEAN bTracked = false;
		hr = ppBodies[j]->get_IsTracked(&bTracked);

		if (!SUCCEEDED(hr) || !bTracked)
			continue;

		ppBodies[j]->GetJoints(JointType_Count, joints);
		anorexia = new BodyFrame();

		for (int i = 0; i < JointType_Count; i++)
		{
			irr::core::vector3df *joint = new irr::core::vector3df(joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z);
			anorexia->addJoint(joint);
		}
		//someCalculation(&nTime);
		//anorexia->setTimestamp(nTime);
		//or
		//something = someCalculation(&nTime);
		//anorexia->setTimestamp(something);
		
		displayBodies[bodyCount-1] = *anorexia;
		break; //once we've set the one entry in displayBodies we are alotted
		//we are done no matter what
	}

	//default just in case no bodies are being tracked
	//TODO make sure default behaves like it is supposed to
	if (j == BODY_COUNT || anorexia == NULL)
		displayBodies[bodyCount - 1] = BodyFrame();

	SafeRelease(pBodyFrame);

	for (int i = 0; i < _countof(ppBodies); i++)
	{
		SafeRelease(ppBodies[i]);
	}

	delete[] joints;
	return true;
}

bool ActionDisplay::getSingleFrameFromFile() {
    // If loaded file was empty, getCurrFrameCount will be 0
    // In that case, subtract one from bodyCount so that only 
    // the kinect body or no body will be rendered
	if (moveFromFile->getCurrFrameCount() <= 0) {
		//I quite like this, but there are a couple of other variables
		//that interact with this one and I see several control paths that
		//lead to this guy being problematic
		//also set PLAYBACK?
		//reset bodyCount in main loop every time and make sure this function ALWAYS gets called before other frameGetty functons?
		//some third thing I can't think of right now?
		bodyCount--;
	}
	else {
		if (frameNumber >= moveFromFile->getCurrFrameCount()) {
			frameNumber = frameNumber % moveFromFile->getCurrFrameCount();
		}
		//TODO 
		//we will now be reading in a keyquatframe instead of a bodyframe
		//let's make that happen
		displayBodies[0] = moveFromFile->getSingleFrame(frameNumber);
	}

	return true;
}

void ActionDisplay::handleKeyPresses(SDL_Event e) {
	if (playback == LIVE) {
		switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				captureKeyframe();
				break;
			case SDLK_d:
				deleteLastKeyframe();
				break;
			case SDLK_s:
				//save current contents of stack to file
				saveKeyframes();
				break;
			case SDLK_BACKSPACE:
				//go back a screen
				loadPrevDisplay();
				break;
		}
	} else {
		switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				togglePlaying();
				break;
			case SDLK_BACKSPACE:
				//go back a screen
				loadPrevDisplay();
				break;
		}
	}
}

void ActionDisplay::handleButtonEvent(SDL_Event* e, Button *currButton)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Mouse is inside button
		if ((*currButton).isInside(e))
		{
			if (playback == LIVE) {
				switch ((*currButton).getType()) {
					case BUTTON_SPRITE_ADD:
						captureKeyframe();
						break;
					case BUTTON_SPRITE_DELETE:
						deleteLastKeyframe();
						break;
					case BUTTON_SPRITE_SAVE:
						saveKeyframes();
						break;
					case BUTTON_SPRITE_BACK:
						loadPrevDisplay();
						break;
					default:
						break;
				}

			} else {
				switch ((*currButton).getType()) {
					case BUTTON_SPRITE_PLAY:
						togglePlaying();
						break;
					case BUTTON_SPRITE_PAUSE:
						togglePlaying();
						break;
					case BUTTON_SPRITE_BACK:
						loadPrevDisplay();
						break;
					default:
						break;
				}
			}
			buttonLog.open("buttonLogData.txt", std::ofstream::app);
			buttonLog << (*currButton).getType() << " button clicked" << std::endl;
			buttonLog.close();
		}
	}
}

//TODO so around here is where we want to do the quaternion math
//on the captured keyframe
//maybe
void ActionDisplay::captureKeyframe() {
	time_t currTime;
	double seconds;

	keyframeCaptured = true;

	time(&currTime);
	if (prevTime != NULL) {
		seconds = difftime(currTime, prevTime);
	} else {
		seconds = 0;
	}
	prevTime = currTime;

	//ask erika, is there any particular reason
	//we capture another frame from the kinect
	//instead of just using the displayBodies frame
	//that's already been loaded?
	frameFromKinect();
	prevKeyframe = displayBodies[bodyCount-1];
	prevKeyframe.setTimestamp(seconds);
	keyframes.pushBackFrame(prevKeyframe);
}

void ActionDisplay::deleteLastKeyframe() {
	time_t currTime;

	//pop most recent from stack
	if (keyframes.getCurrFrameCount() > 0) {
		keyframes.popBackFrame();
	}
	if (keyframes.getCurrFrameCount() > 0) {
		prevKeyframe = keyframes.getBackFrame();
	} else {
		keyframeCaptured = false;
	}
	
	time(&currTime);
	prevTime = currTime;
}

void ActionDisplay::saveKeyframes() {
	saveCount++;
	string filename = "testMovement" + to_string(saveCount);
	//there is a compelling argument to be made that I should
	//avoid messing with stuff here as much as possible and
	//instead do all the quat stuff in Movement.logFrames()
	keyframes.logFrames(filename);
	keyframeCaptured = false;
}

void ActionDisplay::loadPrevDisplay() {
	if (prevScreen == PATIENT_MENU) {
		newDisplay = new PatientMenuDisplay(control, window, renderer);
	} else if (prevScreen == PHYSICIAN_MENU) {
		newDisplay = new PhysicianMenuDisplay(control, window, renderer);
	} else {
		newDisplay = new DisplayMain(control, window, renderer);
	}
	loadNewDisplay();
}

void ActionDisplay::togglePlaying() {
	playing = !playing;

	for (size_t i = 0; i < gButtons.size(); i++) {
		Button* currButton = gButtons.at(i);
		if ((*currButton).getType() == BUTTON_SPRITE_PLAY) {
			(*currButton).freeButton();
			gButtons.erase(gButtons.begin() + i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PAUSE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/pause.bmp"));
		} else if ((*currButton).getType() == BUTTON_SPRITE_PAUSE) {
			(*currButton).freeButton();
			gButtons.erase(gButtons.begin() + i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PLAY, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/play.bmp"));
		}
	}
}

bool ActionDisplay::loadMedia() {
    bool success = true;

    moveFromFile = new Movement();
    
    if (playback == RECORDED || playback == LIVE_RECORD) {
    	//moveFromFile->readPoints("movement1.dat");
		//moveFromFile->readPoints("whereData.dat");
		//moveFromFile->readPoints("testMovement1.dat");
		moveFromFile->readQuatFrame("testMovement1.dat");
    }

    loadButtons();

    return success;
}

bool ActionDisplay::loadButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_BACK, 10, 10, "art/back.bmp"));
	if (playback == LIVE) {
		loadKeyframeButtons();
	} else {
		loadPlaybackButtons();
	}
	
	return true;
}

void ActionDisplay::loadKeyframeButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_ADD, (SCREEN_WIDTH-BUTTON_WIDTH*3)-30, 10, "art/KeyframeButtons/add.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_DELETE, (SCREEN_WIDTH-BUTTON_WIDTH*2)-20, 10, "art/KeyframeButtons/delete.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_SAVE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/KeyframeButtons/save.bmp"));
}

void ActionDisplay::loadPlaybackButtons() {
	gButtons.push_back(new Button(BUTTON_SPRITE_PAUSE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/pause.bmp"));
}

void ActionDisplay::close() {
	if(playback == RECORDED || playback == LIVE_RECORD) {
		// moveFromFile.freeFrames();
		delete moveFromFile;
	}

	closeButtons();
}

int getParent(int type) {
	switch (type) {
	case JointType_SpineShoulder:
	case JointType_SpineBase:
	case JointType_SpineMid:
		return JointType_SpineMid;
	case JointType_Neck:
	case JointType_ShoulderLeft:
	case JointType_ShoulderRight:
		return JointType_SpineShoulder;
	case JointType_HipLeft:
	case JointType_HipRight:
		return JointType_SpineBase;
	case JointType_HandTipLeft:
	case JointType_ThumbLeft:
		return JointType_HandLeft;
	case JointType_HandTipRight:
	case JointType_ThumbRight:
		return JointType_HandRight;
	case JointType_Head:
		return JointType_Neck;
	case JointType_ElbowLeft:
		return JointType_ShoulderLeft;
	case JointType_WristLeft:
		return JointType_ElbowLeft;
	case JointType_HandLeft:
		return JointType_WristLeft;
	case JointType_ElbowRight:
		return JointType_ShoulderRight;
	case JointType_WristRight:
		return JointType_ElbowRight;
	case JointType_HandRight:
		return JointType_WristRight;
	case JointType_KneeLeft:
		return JointType_HipLeft;
	case JointType_AnkleLeft:
		return JointType_KneeLeft;
	case JointType_FootLeft:
		return JointType_AnkleLeft;
	case JointType_KneeRight:
		return JointType_HipRight;
	case JointType_AnkleRight:
		return JointType_KneeRight;
	case JointType_FootRight:
		return JointType_AnkleRight;

	default:
		return type;
	}

}
