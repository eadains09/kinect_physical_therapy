//
//  Movement.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include "BodyFrame.h"

const int FRAME_TOTAL = 500;

class Movement {

private:
//    BodyFrame frames[FRAME_TOTAL];
    BodyFrame **frames;
    int currFrameCount;
    
    void transformPoints(double *xPos, double *yPos, double *zPos);

public:
    Movement();
    void readPoints(std::string path); //Takes file path, reads joint positions out of it, creates joints array
    BodyFrame** getFrames();
	BodyFrame* getSingleFrame(int i);
    int getCurrFrameCount();
    void freeFrames();
	void logMove(std::string fileName);
	virtual ~Movement();

};

