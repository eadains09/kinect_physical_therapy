//
// DisplayAction.cpp
//

#include "DisplayAction.h"

ActionDisplay::ActionDisplay() : DisplayBase() {
	frameNumber = 0;
	keyframeCaptured = false;
	playing = true;
	playback = LIVE;
	bodyCount = 1;
	prevScreen = DISPLAY_MAIN;
	log.open("logData.txt");
}

ActionDisplay::ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d) : DisplayBase(c, w, r) {
	frameNumber = 0;
	keyframeCaptured = false;
	playing = true;
	playback = p;
	prevScreen = d;

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
		log << &pBodyFrameSource << std::endl;
		SafeRelease(pBodyFrameSource);
	}
}

void ActionDisplay::run() {
	if (!init()) {
		printf("Failed to initialize Kinect!\n");
		exit();
	}

	if(!loadMedia()) {
		printf("Failed to load media!\n");
		exit();
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
            	for (int i = 0; i < TOTAL_BUTTONS; i++) {
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

	if (playback == LIVE) {
		frameFromKinect();
	}
	else if (playback == RECORDED) {
		getSingleFrameFromFile();
	}
	else {
		//simultaneous playback
		getSingleFrameFromFile();
		frameFromKinect();
	}

	renderFrame();
	SDL_Delay(50);
	frameNumber++;
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
		QuatFrame *proof = new QuatFrame(displayBodies[i]);
		proof->initBodyFrame(&displayBodies[i]);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i%2], 0xFF);
		renderBody(displayBodies[i]);
	}

	if (keyframeCaptured) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i%2], 0xFF);
		renderBody(prevKeyframe);
	}

    SDL_RenderPresent(renderer);

    return true;
}

void ActionDisplay::renderBody(BodyFrame currBody) {
	eJoint *joints = currBody.getJoints();

	for (int i = 0; i < currBody.getCurrJointCount(); i++) {
			log << joints[i].getX() << " " << joints[i].getY() << endl;
			JointType parent = joints[i].getParent();

			if (parent != joints[i].getType()) {
				SDL_RenderDrawLine(renderer, joints[i].getX(), joints[i].getY(), joints[parent].getX(), joints[parent].getY());
			}
		}
}

bool ActionDisplay::frameFromKinect()
{
	HRESULT hr;
	if (!m_pBodyFrameReader)
	{
		return false;
	}

	IBodyFrame* pBodyFrame = NULL;
	m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (!pBodyFrame)
		return false;

	INT64 nTime = 0;

	hr = pBodyFrame->get_RelativeTime(&nTime);

	IBody* ppBodies[BODY_COUNT] = { 0 };

	hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

	BodyFrame *anorexia = new BodyFrame();

	Joint *joints = new Joint[JointType_Count];
	for (int j = 0; j < _countof(ppBodies); j++)
	{

		BOOLEAN bTracked = false;
		hr = ppBodies[j]->get_IsTracked(&bTracked);

		if (!bTracked)
		{
			continue;
		}


		ppBodies[j]->GetJoints(JointType_Count, joints);

		for (int i = 0; i < JointType_Count; i++)
		{
			irr::core::quaternion *quat;
			if (getParent(i) == i)
			{
				quat = new irr::core::quaternion(0, 0, 0, 0);
			}
			else
			{
				float x = joints[i].Position.X - joints[getParent(i)].Position.X;
				float y = joints[i].Position.Y - joints[getParent(i)].Position.Y;
				float z = joints[i].Position.Z - joints[getParent(i)].Position.Z;
				float yaw = atan2(x, z) *180.0 / 3.141592653;
				float padj = sqrt(pow(x, 2) + pow(z, 2));
				float pitch = atan2(padj, y) *180.0 / 3.141592653;


				quat = new irr::core::quaternion(0, pitch, yaw);
			}
			delete quat;

			log << joints[i].Position.X << joints[i].Position.Y << joints[i].Position.Z << std::endl;
			anorexia->addJoint(*(new eJoint(i, (int)((joints[i].Position.X + 1) * 200), (int)((joints[i].Position.Y - 1)*-200))));

		}
		displayBodies[bodyCount-1] = *anorexia;
	}
	SafeRelease(pBodyFrame);

	for (int i = 0; i < _countof(ppBodies); ++i)
	{
		SafeRelease(ppBodies[i]);
	}

	return true;

}

bool ActionDisplay::getSingleFrameFromFile() {
    // If loaded file was empty, getCurrFrameCount will be 0
    // In that case, subtract one from bodyCount so that only 
    // the kinect body or no body will be rendered
	if (moveFromFile.getCurrFrameCount() <= 0) {
		bodyCount--;
	}
	else {
		if (frameNumber >= moveFromFile.getCurrFrameCount()) {
			frameNumber = frameNumber % moveFromFile.getCurrFrameCount();
		}
		displayBodies[0] = moveFromFile.getSingleFrame(frameNumber);
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
				}
			}
			buttonLog.open("buttonLogData.txt", std::ofstream::app);

			buttonLog << (*currButton).getType() << " button clicked" << std::endl;
			//do something in response to which button it is

			buttonLog.close();
		}
	}
}

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

	framesFromKinect(false);
	prevKeyframe = displayBodies[bodyCount-1];
	prevKeyframe.setTimestamp(seconds);
	keyframes.pushBackFrame(prevKeyframe);
	flashScreen();
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
	keyframes.logFrames(filename);
	keyframeCaptured = false;
}

void ActionDisplay::loadPrevDisplay() {
	if (prevScreen == PATIENT_MENU) {
		newDisplay = new PatientMenuDisplay(control, window, renderer);
	} else if (prevScreen == PHYSICIAN_MENU) {
		newDisplay = new PhysicianMenuDisplay(control, window, renderer);
	} else {
		newDisplay = new MainDisplay(control, window, renderer);
	}
	loadNewDisplay();
}

// void ActionDisplay::loadNewDisplay() {
// 	control.switchDisplays(&newDisplay);
// 	quit = true;
// }

void ActionDisplay::togglePlaying() {
	playing = !playing;

	for (int i = 0; i < gButtons.size(); i++) {
		Button* currButton = *gButtons.at(i);
		if ((*currButton).getType() == BUTTON_SPRITE_PLAY) {
			(*currButton).freeButton();
			(*gButtons.at(i)).erase(i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PAUSE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/pause.bmp"));
		} else if ((*currButton).getType() == BUTTON_SPRITE_PAUSE) {
			(*currButton).freeButton();
			(*gButtons.at(i)).erase(i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PLAY, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/play.bmp"));
		}
	}
}

bool ActionDisplay::loadMedia() {
    bool success = true;
    
    if (playback == RECORDED || playback == LIVE_RECORD) {
    	//moveFromFile.readPoints("movement1.dat");
		moveFromFile.readPoints("whereData.dat");
		//moveFromFile.readPoints("testMovement1.dat");
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
	//This may not be necessary:
	if(playback == RECORDED || playback == LIVE_RECORD)
		moveFromFile.freeFrames();

	closeButtons();
}

int getParent(int type) {
	switch (type) {
	case JointType_SpineBase:// SPINE_BASE:
		return JointType_SpineMid;// SPINE_MID;
	case JointType_SpineMid:// SPINE_MID:
		return JointType_SpineMid;// JOINT_DEFAULT;
	case JointType_Neck:// NECK:
		return JointType_SpineShoulder;// SPINE_SHOULDER;
	case JointType_Head:// HEAD:
		return JointType_Neck;// NECK;
	case JointType_ShoulderLeft:// SHOULDER_LEFT:
		return JointType_SpineShoulder;// SPINE_SHOULDER;
	case JointType_ElbowLeft:// ELBOW_LEFT:
		return JointType_ShoulderLeft;// SHOULDER_LEFT;
	case JointType_WristLeft:// WRIST_LEFT:
		return JointType_ElbowLeft;// ELBOW_LEFT;
	case JointType_HandLeft:// HAND_LEFT:
		return JointType_WristLeft;// WRIST_LEFT;
	case JointType_HandTipLeft:// HAND_TIP_LEFT:
		return JointType_HandLeft;// HAND_LEFT;
	case JointType_ThumbLeft:// THUMB_LEFT:
		return JointType_HandLeft;// HAND_LEFT;
	case JointType_ShoulderRight:// SHOULDER_RIGHT:
		return JointType_SpineShoulder; //SPINE_SHOULDER;
	case JointType_ElbowRight:// ELBOW_RIGHT:
		return JointType_ShoulderRight;// SHOULDER_RIGHT;
	case JointType_WristRight:// WRIST_RIGHT:
		return JointType_ElbowRight;// ELBOW_RIGHT;
	case JointType_HandRight:// HAND_RIGHT:
		return JointType_WristRight;// WRIST_RIGHT;
	case JointType_HandTipRight:// HAND_TIP_RIGHT:
		return JointType_HandRight;// HAND_RIGHT;
	case JointType_ThumbRight:// THUMB_RIGHT:
		return JointType_HandRight;// HAND_RIGHT;
	case JointType_HipLeft:// HIP_LEFT:
		return JointType_SpineBase;// SPINE_BASE;
	case JointType_KneeLeft:// KNEE_LEFT:
		return JointType_HipLeft;// HIP_LEFT;
	case JointType_AnkleLeft:// ANKLE_LEFT:
		return JointType_KneeLeft;// KNEE_LEFT;
	case JointType_FootLeft:// FOOT_LEFT:
		return JointType_AnkleLeft;// ANKLE_LEFT;
	case JointType_HipRight:// HIP_RIGHT:
		return JointType_SpineBase;// SPINE_BASE;
	case JointType_KneeRight:// KNEE_RIGHT:
		return JointType_HipRight;// HIP_RIGHT;
	case JointType_AnkleRight:// ANKLE_RIGHT:
		return JointType_KneeRight;// KNEE_RIGHT;
	case JointType_FootRight:// FOOT_RIGHT:
		return JointType_AnkleRight;// ANKLE_RIGHT;
	case JointType_SpineShoulder:// SPINE_SHOULDER:
		return JointType_SpineMid;// SPINE_MID;
	default:
		return type;
	}

}

