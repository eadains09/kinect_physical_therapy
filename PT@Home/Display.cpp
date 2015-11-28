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
			}
            
            for (int i = 0; i < TOTAL_BUTTONS; i++) {
                (*gButtons[i]).handleEvent(&e);
            }
		}

		if (live == 0) {
			bodyCount = 1;
			if (framesFromKinect(firstRun))
				firstRun = false;
		}
		else if (live == 1) {
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

	if (firstRun)
	{
		firstPointBodyFrame();
		firstQuatBodyFrame();
	}
	else
	{
		openPointBodyFrame();
		openQuatBodyFrame();
	}

	INT64 nTime = 0;

	hr = pBodyFrame->get_RelativeTime(&nTime);

	IBody* ppBodies[BODY_COUNT] = { 0 };

	hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

	BodyFrame *anorexia = new BodyFrame();
	QuatFrame *cleverPun = new QuatFrame();

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
				subsequentPoint();
				subsequentQuat();
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
			logQuat(quat->X, quat->Y, quat->Z, quat->W);
			if (cleverPun->addQuatJoint(*quat, i))
				log << "addQuatJoint() says we succesfully initialized the QuatFrame" << std::endl;
			//cleverPun->addQuatJoint(*new irr::core::quaternion(0, 0, 0, 0), i);
			delete quat;

			logPoint(joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z);
			log << joints[i].Position.X << joints[i].Position.Y << joints[i].Position.Z << std::endl;
			//TODO i is not a valid x value, either make sure that it never gets used that way or remove it
			irr::core::vector3df *finish = new irr::core::vector3df((int)((joints[i].Position.X + 1) * 200), (int)((joints[i].Position.Y - 1)*-200), 0);
			
			if (i == JointType_SpineMid)
				cleverPun->addMidSpine(*new irr::core::vector3df(0, 0, 0));
				//cleverPun->addMidSpine(*finish);

			//anorexia->addJoint(finish);

		}
		//QuatFrame *demo = new QuatFrame(*anorexia);
		cleverPun->initBodyFrame(*anorexia);
		for (int k = 0; k < JOINT_TOTAL; k++)
			log << anorexia->getJoints()[k]->X << "," << anorexia->getJoints()[k]->Y << "," << anorexia->getJoints()[k]->Z << std::endl;

		displayBodies[bodyCount-1] = anorexia;
	}
	SafeRelease(pBodyFrame);
	closePointBodyFrame();
	closeQuatBodyFrame();

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
		if (displayBodies[j] == NULL)
			continue;

		QuatFrame *proof = new QuatFrame(*displayBodies[j]);
		proof->initBodyFrame(*displayBodies[j]);

		irr::core::vector3df **joints = displayBodies[j]->getJoints();
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[j%2], 0xFF);

		for (int i = 0; i < displayBodies[j]->getCurrJointCount(); i++) {
			//log << joints[i]->X << " " << joints[i]->Y << endl;
			int parent = getParent(i);

			if (parent != i) 
			{
				SDL_RenderDrawLine(renderer, joints[i]->X, joints[i]->Y, joints[parent]->X, joints[parent]->Y);
			}
		}
	}

    SDL_RenderPresent(renderer);

    return true;
}


bool Display::getSingleFrameFromFile() {
    //check if getCurrFrameCount is 0, and figure out what needs to be done to display a blank if it is 0
	if (frameNumber >= currMove->getCurrFrameCount()) {
		frameNumber = frameNumber % currMove->getCurrFrameCount();
	}
	displayBodies[0] = currMove->getSingleFrame(frameNumber);

	return true;
}


bool Display::init() {
    bool success = true;
	HRESULT hr;
	IBodyFrameSource* pBodyFrameSource = NULL;
	frameNumber = 0;
	displayBodies = new BodyFrame*[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayBodies[i] = NULL;

	GetDefaultKinectSensor(&m_pKinectSensor);
	if (live == 0 || live == 2)
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

		openPointLog();
		openQuatLog();
	}

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow("Kinect Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    
    //currMove.readPoints("movement1.dat");
	currMove->readPoints("whereData.dat");
    
    //initialize buttons
    gButtons[0] = new Button(BUTTON_SPRITE_BACK, 0, 0, "back.bmp");
    gButtons[1] = new Button(BUTTON_SPRITE_RECORD, SCREEN_WIDTH-BUTTON_WIDTH, 0, "play.bmp");

    
    return success;
}

void Display::close() {
    //This may not be necessary:
	if (live == 1 || live == 2)
		delete currMove;
//		currMove.freeFrames();
    
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

	//TODO this should be closed in response to user event, not the program closing
	if (live == 0 || live == 2)
	{
		closePointLog();
		closeQuatLog();
	}
    
}


void Display::subsequentPoint()
{
	whereData << "," << std::endl;
}

void Display::firstPointBodyFrame()
{
	whereData << "[" << std::endl;
}


void Display::openPointBodyFrame()
{
	whereData << "," << std::endl << "[" << std::endl;
}

void Display::closePointBodyFrame()
{
	whereData << std::endl << "]";
}



void Display::logPoint(float x, float y, float z)
{
	whereData
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< "]";
}

void Display::openPointLog()
{
	whereData.open("whereData.dat");
	whereData << "{" << std::endl << "\"joints\": [" << std::endl;
}

void Display::closePointLog()
{
	whereData << std::endl << "]" << std::endl << "}";
	whereData.close();
}


void Display::subsequentQuat()
{
	moveData << "," << std::endl;
}

void Display::firstQuatBodyFrame()
{
	moveData << "[" << std::endl;
}


void Display::openQuatBodyFrame()
{
	moveData << "," << std::endl << "[" << std::endl;
}

void Display::closeQuatBodyFrame()
{
	moveData << std::endl << "]";
}



void Display::logQuat(float x, float y, float z, float w)
{
	moveData
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< ", "
		<< w
		<< "]";
}

void Display::openQuatLog()
{
	moveData.open("moveData.dat");
	moveData << "{" << std::endl << "\"jointQuats\": [" << std::endl;
}

void Display::closeQuatLog()
{
	moveData << std::endl << "]" << std::endl << "}";
	moveData.close();
}