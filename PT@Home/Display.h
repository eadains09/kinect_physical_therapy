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

const int TOTAL_BODIES = 2;

class Display {

private:
	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	int frameNumber; //Which frame to read from file
	int bodyCount; //Number of bodies being displayed, right now only options are 1 or 2
	SDL_Window* window = NULL;  //The window we'll be rendering to
	SDL_Renderer* renderer = NULL;
	//SDL_Point bodyPoints[JOINT_TOTAL];
	BodyFrame displayBodies[TOTAL_BODIES];
	Movement currMove;


	std::ofstream log, moveData, whereData;

	//whether we will be displaying data live from kinect or recorded movement
	// 0 = just live playback
	// 1 = just recorded playback
	// 2 = live and recorded playback
	// ?3 = 2 recorded playbacks? - dont know if implementation will be necessary 
	int live = 1;


	bool init();  //Starts up SDL and creates window
	bool loadMedia();
	//bool getFramesFromFile(string filename);
	bool getSingleFrameFromFile();
	void close();  //Frees media and shuts down SDL
	void logPoint(float x, float y, float z);
	void openPointLog();
	void closePointLog();
	void openPointBodyFrame();
	void firstPointBodyFrame();
	void closePointBodyFrame();
	void subsequentPoint();
	bool framesFromKinect(bool firstRun);


	void logQuat(float x, float y, float z, float w);
	void openQuatLog();
	void closeQuatLog();
	void openQuatBodyFrame();
	void firstQuatBodyFrame();
	void closeQuatBodyFrame();
	void subsequentQuat();

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


#endif