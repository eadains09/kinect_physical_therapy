//
//  Display.h
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef _DISPLAY_H_
#define _DISPLAY_H_
//TODO change other ifdefs to drop PlayBodyPoints name

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


using namespace std;

const int TOTAL_BODIES = 2;
const int TOTAL_BUTTONS = 2;

enum playbackType {
	LIVE = 0,
	RECORDED = 1,
	LIVE_RECORD = 2
};

class Display {

private:
	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	int saveCount;
	int frameNumber; //Which frame to read from file
	int bodyCount; //Number of bodies being displayed, right now only options are 1 or 2
	SDL_Window* window = NULL;  //The window we'll be rendering to
	SDL_Renderer* renderer = NULL;
	//SDL_Point bodyPoints[JOINT_TOTAL];
	BodyFrame displayBodies[TOTAL_BODIES];
	Movement currMove;
	Movement keyframes;
    Button* gButtons[TOTAL_BUTTONS];
	FileWriter writer;


	std::ofstream log; //, moveData, whereData;

	int playback = LIVE_RECORD;


	bool init();  //Starts up SDL and creates window
	bool loadMedia();
	//bool getFramesFromFile(string filename);
	bool getSingleFrameFromFile();
	void close();  //Frees media and shuts down SDL
	bool framesFromKinect(bool firstRun);
	bool framesFromQuaternions(bool firstRun);
	void handleKeyPresses(SDL_Event e);
	void captureKeyframe();
	void flashScreen();
	void saveKeyframes();



/* Moved to FileWriter.h 
	void openQuatLog();
	void closeQuatLog();
	void openQuatBodyFrame();
	void firstQuatBodyFrame();
	void closeQuatBodyFrame();
	void subsequentQuat();
	void logQuat(float x, float y, float z, float w);

	void logPoint(float x, float y, float z);
	void openPointLog();
	void closePointLog();
	void openPointBodyFrame();
	void firstPointBodyFrame();
	void closePointBodyFrame();
	void subsequentPoint();
*/



	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;


public:
	//bool renderFrame(BodyFrame currFrame);
	bool renderFrame();
	bool run();

};
int getParent(int);

#endif /* _DISPLAY_H_ */