//
//  Display.cpp
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

// NO LONGER USED CLASS - SHOULD BE DELETED
/*
#include "Display.h"
#include "stdafx.h"
#include <time.h>
#include <quaternion.h>
#include "QuatFrame.h"


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

bool Display::run() {
	HRESULT hr;
	playback = LIVE;

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
                	handleButtonEvent(&e, gButtons[i]);
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
			//so this is the bit where we'll have to 
			//compare, and thus is the only place in which
			//it necessary to convert live kinect frames
			//to quaternions, I would suggest a new function
			//for that

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
			deleteLastKeyframe();
		break;
		case SDLK_s:
			//save current contents of stack to file
			saveKeyframes();
		break;
	}
}

void Display::handleButtonEvent(SDL_Event* e, Button *currButton)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Mouse is inside button
		if ((*currButton).isInside(e))
		{
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
				default:
					break;
			}
			buttonLog.open("buttonLogData.txt", std::ofstream::app);

			buttonLog << (*currButton).getType() << " button clicked" << std::endl;
			//do something in response to which button it is

			buttonLog.close();
		}
	}
}

//TODO so around here is where we want to do the quaternion math
//on the captured keyframe
void Display::captureKeyframe() {
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

void Display::flashScreen() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_Delay(50);
}

void Display::deleteLastKeyframe() {
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

void Display::saveKeyframes() {
	saveCount++;
	string filename = "testMovement" + to_string(saveCount);
	keyframes.logFrames(filename);
	keyframeCaptured = false;
}


bool Display::framesFromKinect(bool firstRun)
{
	HRESULT hr;
	if (!m_pBodyFrameReader)
	{
		return false;
	}

	IBodyFrame* pBodyFrame = NULL;
	hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (!pBodyFrame || !SUCCEEDED(hr))
		return false;


	INT64 nTime = 0;

	hr = pBodyFrame->get_RelativeTime(&nTime);

	IBody* ppBodies[BODY_COUNT] = { 0 };

	hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

	BodyFrame *anorexia = new BodyFrame();

	Joint *joints = new Joint[JointType_Count];

	for (int j = 0; j < _countof(ppBodies); j++)
	{
		//For frames from kinect before keyframe selection I think it makes
		//most sense to just keep them points, then do all the quat math
		//on the keyframes we've captured
		ppBodies[j]->GetJoints(JointType_Count, joints);

		for (int i = 0; i < JointType_Count; i++)
		{
			//DO NOT DELETE UNTIL ERIKA HAS BEEN CONSULTED
			//????
//			if (i)
//			{
//				writer.subsequentPoint();
//				writer.subsequentQuat();
//			}
			
			//writer.logQuat(quat->X, quat->Y, quat->Z, quat->W);

			//writer.logPoint((joints[i].Position.X + 1) * 200, (joints[i].Position.Y -1) * -200, joints[i].Position.Z);
			irr::core::vector3df *joint = new irr::core::vector3df(joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z);
			anorexia->addJoint(joint);
		//	anorexia->addJoint(*(new eJoint(i, (int)((joints[i].Position.X + 1) * 200), (int)((joints[i].Position.Y - 1)*-200))));

		}
		//TODO deal with properly writing out quaternions and midSpine
		displayBodies[bodyCount-1] = *anorexia;
	}
	SafeRelease(pBodyFrame);

	for (int i = 0; i < _countof(ppBodies); ++i)
	{
		SafeRelease(ppBodies[i]);
	}

	return true;

}


bool Display::renderFrame() {
	int j;
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
		if (displayBodies[j].getCurrJointCount() != JOINT_TOTAL)
			continue;

	//	QuatFrame *proof = new QuatFrame(*displayBodies[j]);
	//	proof->initBodyFrame(*displayBodies[j]);
	//	delete proof;

		irr::core::vector3df **joints = displayBodies[j].getJoints();

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[j%2], 0xFF);
		renderBody(displayBodies[j]);
	}

	if (keyframeCaptured) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[j % 2], 0xFF);
		renderBody(prevKeyframe);
	}

    SDL_RenderPresent(renderer);

    return true;
}


void Display::renderBody(BodyFrame currBody) {
	irr::core::vector3df **joints = currBody.getJoints();

	for (int i = 0; i < currBody.getCurrJointCount(); i++) 
			if (getParent(i) != i)
				SDL_RenderDrawLine(renderer, joints[i]->X, joints[i]->Y, joints[getParent(i)]->X, joints[getParent(i)]->Y);
}

bool Display::getSingleFrameFromFile() {
    // If loaded file was empty, getCurrFrameCount will be 0
    // In that case, subtract one from bodyCount so that only 
    // the kinect body or no body will be rendered
	if (currMove->getCurrFrameCount() <= 0) {
		bodyCount--;
	}
	else {
		if (frameNumber >= currMove->getCurrFrameCount()) {
			frameNumber = frameNumber % currMove->getCurrFrameCount();
		}
		//TODO 
		//we will now be reading in a keyquatframe instead of a bodyframe
		//let's make that happen
		displayBodies[0] = currMove->getSingleFrame(frameNumber);
	}

	return true;
}


bool Display::init() {
    bool success = true;
    saveCount = 0;
	HRESULT hr;
	IBodyFrameSource* pBodyFrameSource = NULL;
	frameNumber = 0;

	displayBodies = new BodyFrame[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayBodies[i] = BodyFrame();

	keyframeCaptured = false;

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
	currMove = new Movement();
    
    //currMove->readPoints("movement1.dat");
	//currMove->readPoints("whereData.dat");
	currMove->readKeyframes("pleasework.dat");
    
    //initialize buttons
    gButtons[0] = new Button(BUTTON_SPRITE_BACK, 10, 10, "art/back.bmp");
    gButtons[1] = new Button(BUTTON_SPRITE_ADD, (SCREEN_WIDTH-BUTTON_WIDTH*3)-30, 10, "art/add.bmp");
    gButtons[2] = new Button(BUTTON_SPRITE_DELETE, (SCREEN_WIDTH-BUTTON_WIDTH*2)-20, 10, "art/delete.bmp");
    gButtons[3] = new Button(BUTTON_SPRITE_SAVE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/save.bmp");

    return success;
}

void Display::close() {
	if(playback == RECORDED || playback == LIVE_RECORD)
		delete currMove;

    
    for (int i = 0; i < TOTAL_BUTTONS; i++) {
        gButtons[i]->freeButton();
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
	//if (playback == LIVE || playback == LIVE_RECORD)
	//{
	//	writer.closePointLog();
	//	writer.closeQuatLog();
	//}
    
 
}

*/
