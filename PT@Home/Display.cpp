//
//  Display.cpp
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "Display.h"
#include "stdafx.h"
#include <quaternion.h>
#include "QuatFrame.h"


bool Display::run() {
	HRESULT hr;

	log.open("logData.txt");

	if (!init()) {
		printf("Failed to initialize!\n");
		return false;
	}

	//Load media
	if (!loadMedia()) {
		printf("Failed to load media!\n");
		return false;
	}

	//Main loop flag
	bool quit = false;
	//Event Handler
	SDL_Event e;

	//true only for the first pointBodyFrame
	bool firstRun = true;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				handleKeyPresses(e);
			} else {
            	for (int i = 0; i < TOTAL_BUTTONS; i++) {
                	(*gButtons[i]).handleEvent(&e);
            	}
            }
		}

		if (playback == LIVE) {
			bodyCount = 1;
			if (framesFromKinect(firstRun))
				firstRun = false;
		}
		else if (playback == RECORDED) {
			bodyCount = 1;
			getSingleFrameFromFile();
		}
		else {
			//simultaneous playback
			bodyCount = 2;

			getSingleFrameFromFile();
			if (framesFromKinect(firstRun))
				firstRun = false;
		}

		renderFrame();
		SDL_Delay(50);
		frameNumber++;

	}
		//Free resources and close SDL
		close();

		return true;
}

void Display::handleKeyPresses(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_SPACE:
			captureKeyframe();
		break;

		case SDLK_BACKSPACE:
			//pop most recent from stack
			keyframes.popBackKeyframe();
		break;

		case SDLK_s:
			//save current contents of stack to file
			saveKeyframes();
		break;
	}
}

void Display::captureKeyframe() {
	time_t currTime;
	double seconds;

	time(&currTime);
	if (prevTime != NULL) {
		seconds = difftime(currTime, prevTime);
	} else {
		seconds = 0;
	}
	prevTime = currTime;

	framesFromKinect(false);
	displayBodies[bodyCount-1].setTimestamp(seconds);
	keyframes.pushBackKeyframe(displayBodies[bodyCount-1]);
	flashScreen();
}

void Display::flashScreen() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_Delay(10);
}

void Display::saveKeyframes() {
	saveCount++;
	string filename = "testMovement" + to_string(saveCount);
	keyframes.logKeyframes(filename);
}


bool Display::framesFromKinect(bool firstRun)
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

	if (firstRun)
	{
		writer.firstPointBodyFrame();
		writer.firstQuatBodyFrame();
	}
	else
	{
		writer.openPointBodyFrame();
		writer.openQuatBodyFrame();
	}

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
			if (i)
			{
				writer.subsequentPoint();
				writer.subsequentQuat();
			}
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
			writer.logQuat(quat->X, quat->Y, quat->Z, quat->W);
			delete quat;

			writer.logPoint(joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z);
			log << joints[i].Position.X << joints[i].Position.Y << joints[i].Position.Z << std::endl;
			anorexia->addJoint(*(new eJoint(i, (int)((joints[i].Position.X + 1) * 200), (int)((joints[i].Position.Y - 1)*-200))));

		}
		displayBodies[bodyCount-1] = *anorexia;
	}
	SafeRelease(pBodyFrame);
	writer.closePointBodyFrame();
	writer.closeQuatBodyFrame();

	for (int i = 0; i < _countof(ppBodies); ++i)
	{
		SafeRelease(ppBodies[i]);
	}

	return true;

}

bool Display::renderFrame() {
    int colorArray[2] = {0x00, 0xFF};

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    
    //render buttons
    for (int i = 0; i < TOTAL_BUTTONS; i++) {
        (*gButtons[i]).render(renderer);
    }
    
    //render bodies
	for (int j = 0; j < bodyCount; j++) {
		QuatFrame *proof = new QuatFrame(displayBodies[j]);
		proof->initBodyFrame(&displayBodies[j]);

		eJoint *joints = displayBodies[j].getJoints();
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[j%2], 0xFF);

		for (int i = 0; i < displayBodies[j].getCurrJointCount(); i++) {
			log << joints[i].getX() << " " << joints[i].getY() << endl;
			JointType parent = joints[i].getParent();

			if (parent != joints[i].getType()) {
				SDL_RenderDrawLine(renderer, joints[i].getX(), joints[i].getY(), joints[parent].getX(), joints[parent].getY());
			}
		}
	}

    SDL_RenderPresent(renderer);

    return true;
}

bool Display::getSingleFrameFromFile() {
    // If loaded file was empty, getCurrFrameCount will be 0
    // In that case, subtract one from bodyCount so that only 
    // the kinect body or no body will be rendered
	if (currMove.getCurrFrameCount() <= 0) {
		bodyCount--;
	}
	else {
		if (frameNumber >= currMove.getCurrFrameCount()) {
			frameNumber = frameNumber % currMove.getCurrFrameCount();
		}
		displayBodies[0] = currMove.getSingleFrame(frameNumber);
	}
	return true;
}


bool Display::init() {
    bool success = true;
    saveCount = 0;
	HRESULT hr;
	IBodyFrameSource* pBodyFrameSource = NULL;
	frameNumber = 0;

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

		writer.openPointLog();
		writer.openQuatLog();
	}

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow("Kinect Physical Therapy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return success;
}

bool Display::loadMedia() {
    bool success = true;
    
    //currMove.readPoints("movement1.dat");
	currMove.readPoints("whereData2.dat");
	//currMove.readPoints("testMovement1.dat");
    
    //initialize buttons
    gButtons[0] = new Button(BUTTON_SPRITE_BACK, 0, 0, "art/back.bmp");
    gButtons[1] = new Button(BUTTON_SPRITE_RECORD, SCREEN_WIDTH-BUTTON_WIDTH, 0, "art/play.bmp");

    
    return success;
}

void Display::close() {
    //This may not be necessary:
	if(playback == RECORDED || playback == LIVE_RECORD)
		currMove.freeFrames();
    
    for (int i = 0; i < TOTAL_BUTTONS; i++) {
        (*gButtons[i]).freeButton();
    }
    
    SDL_DestroyRenderer(renderer);
    
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();

// TODO Should we be closing log file also?
	//TODO this should be closed in response to user event, not the program closing
	if (playback == LIVE || playback == LIVE_RECORD)
	{
		writer.closePointLog();
		writer.closeQuatLog();
	}
    
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
