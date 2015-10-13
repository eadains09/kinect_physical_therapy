//
//  Movement.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef __PlayBodyPoints__Movement__
#define __PlayBodyPoints__Movement__

#include <stdio.h>
#include <string>
#include "BodyFrame.h"

const int FRAME_TOTAL = 50;

class Movement {

private:
    BodyFrame frames[FRAME_TOTAL];
    int currFrameCount;

public:
    Movement();
    void readPoints(std::string path); //Takes file path, reads joint positions out of it, creates joints array
};

#endif /* defined(__PlayBodyPoints__Movement__) */
