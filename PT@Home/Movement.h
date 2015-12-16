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
	deque<QuatFrame> *qframes;
    int currFrameCount;

public:
    Movement();
    void readKeyframes(std::string path);
    irr::core::vector3df readJointPoints(FileReader *file);
    irr::core::quaternion readJointQuat(FileReader *file);
	QuatFrame *getSingleFrame(int frameNumber, double time);
    int getCurrFrameCount();
	void logFrames(std::string fileName);
    void popBackFrame();
    void pushBackFrame(QuatFrame *frame);
	deque<QuatFrame> getFrames();
    QuatFrame* getBackFrame();
	virtual ~Movement();
};
