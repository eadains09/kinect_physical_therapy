//
//  Display.h
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#pragma once


#include <fstream>
#include <iostream>
#include <Kinect.h>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <quaternion.h>
#include "Button.h"
#include "Movement.h"
#include "FileWriter.h"


//using namespace std;

const int TOTAL_BODIES = 2;
const int TOTAL_BUTTONS = 4;

enum playbackType {
	LIVE = 0,
	RECORDED = 1,
	LIVE_RECORD = 2
};

class Display {

private:
	//Screen dimension constants
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	int saveCount;
	int frameNumber; //Which frame to read from file
	int bodyCount; //Number of bodies being displayed, right now only options are 1 or 2
	SDL_Window* window = NULL;  //The window we'll be rendering to
	SDL_Renderer* renderer = NULL;

	//SDL_Point bodyPoints[JOINT_TOTAL];
	BodyFrame *displayBodies;// [TOTAL_BODIES];
	Movement *currMove;

	//BodyFrame displayBodies[TOTAL_BODIES];

	bool keyframeCaptured;
	BodyFrame prevKeyframe;
	//Movement currMove;
	Movement keyframes;
	time_t prevTime = NULL;

    Button* gButtons[TOTAL_BUTTONS];
	
	FileWriter writer;
	std::ofstream log;
	std::ofstream buttonLog;

	int playback;


	bool init();  //Starts up SDL and creates window
	bool loadMedia();
	bool getSingleFrameFromFile();
	void close();  //Frees media and shuts down SDL
	bool framesFromKinect(bool firstRun);
	void handleKeyPresses(SDL_Event e);
	void handleButtonEvent(SDL_Event* e, Button *currButton);
	void captureKeyframe();
	void deleteLastKeyframe();
	void flashScreen();
	void saveKeyframes();
	void renderBody(BodyFrame currBody);



	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;


public:
	bool renderFrame();
	bool run();

};
int getParent(int);
