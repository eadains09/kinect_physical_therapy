//
//  Display.h
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef Display_h
#define Display_h
//TODO change other ifdefs to drop PlayBodyPoints name

#include <fstream>
#include <iostream>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include "Movement.h"
#include <Kinect.h>

using namespace std;


class Display {

private:
	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	SDL_Window* window = NULL;  //The window we'll be rendering to
	SDL_Renderer* renderer = NULL;
	SDL_Point bodyPoints[JOINT_TOTAL];
	Movement currMove;

	std::ofstream log, moveData, whereData;

	//whether we will be displaying data live from kinect or recorded movement
	bool live = false;


	bool init();  //Starts up SDL and creates window
	bool loadMedia();
	bool getFramesFromFile(string filename);
	void close();  //Frees media and shuts down SDL
	void logPoint(float x, float y, float z);
	void openPointLog();
	void closePointLog();
	void openPointBodyFrame();
	void firstPointBodyFrame();
	void closePointBodyFrame();
	void subsequentPoint();
	bool framesFromKinect(bool firstRun);

	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;


public:
	bool renderFrame(BodyFrame currFrame);
	bool run();

};


#endif