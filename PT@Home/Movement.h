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

const int FRAME_TOTAL = 500;

using namespace std;

class Movement {

private:
    BodyFrame frames[FRAME_TOTAL];
    deque<BodyFrame> keyframeStack;
//    BodyFrame *frames;
    int currFrameCount;
    
    void transformPoints(double *xPos, double *yPos, double *zPos);

public:
    Movement();
    void readPoints(std::string path); //Takes file path, reads joint positions out of it, creates joints array
    BodyFrame* getFrames();
	BodyFrame getSingleFrame(int i);
    int getCurrFrameCount();
    void freeFrames();
	void logKeyframes(std::string fileName);
    void popBackKeyframe();
    void pushBackKeyframe(BodyFrame frame);



};

#endif /* defined(__Movement_H__) */
