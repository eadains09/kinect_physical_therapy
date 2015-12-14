//
//  Movement.h
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include <deque>
#include "BodyFrame.h"
#include "FileReader.h"
#include "QuatFrame.h"

const int FRAME_TOTAL = 250;

//using namespace std;

class Movement {

private:
	//TODO
	//if this could just be a nice deque<BodyFrame> and
	//save all pointer headaches that would be really swell
	//once merge is sufficiently complete to determine whether
	//or not that's feasible, do, and then do
    //deque<BodyFrame> *frames;
	deque<QuatFrame> *qframes;
    //deque<BodyFrame> keyframeStack;
    int currFrameCount;
	bool finished;
	//void logMove(std::string fileName);
    
	//initbodyframe does this at the end, so this function
	//should only be used for the raw points from the kinect
	//if at all
    void transformPoints(double *xPos, double *yPos, double *zPos);

public:
    Movement();
    //void readPoints(std::string path); //Takes file path, reads joint positions out of it, creates joints array

	void readQuatFrame(std::string path);
    void readKeyframes(std::string path);
    irr::core::vector3df readJointPoints(FileReader *file);
    irr::core::quaternion readJointQuat(FileReader *file);
    //BodyFrame* getFrames();
	QuatFrame *getSingleFrame(double time);
	//it would be trivial to write a similar method
	//that instead returns a quatframe, this may be
	//useful for control flow after we change to 
	//immediately converting points to quaternions
    int getCurrFrameCount();
 //   void freeFrames();
	void logFrames(std::string fileName);
    void popBackFrame();
    void pushBackFrame(QuatFrame *frame);
	deque<QuatFrame> getFrames();
    QuatFrame* getBackFrame();
	bool isFinished();
	void resetFinished();
	virtual ~Movement();
};
