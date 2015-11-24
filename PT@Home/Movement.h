//
//  Movement.h
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef __Movement_H__
#define __Movement_H__

#include <stdio.h>
#include <string>
#include <deque>
#include "BodyFrame.h"

const int FRAME_TOTAL = 250;

using namespace std;

class Movement {

private:
    //BodyFrame frames[FRAME_TOTAL];
    deque<BodyFrame> frames;
    //deque<BodyFrame> keyframeStack;
    int currFrameCount;
    
    void transformPoints(double *xPos, double *yPos, double *zPos);

public:
    Movement();
    void readPoints(std::string path); //Takes file path, reads joint positions out of it, creates joints array
    //BodyFrame* getFrames();
	BodyFrame getSingleFrame(int i);
    int getCurrFrameCount();
    void freeFrames();
	void logFrames(std::string fileName);
    void popBackFrame();
    void pushBackFrame(BodyFrame frame);
    BodyFrame getBackFrame();




};

#endif /* defined(__Movement_H__) */
