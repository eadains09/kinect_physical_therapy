//
//  Body.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef PlayBodyPoints_BodyFrame_h
#define PlayBodyPoints_BodyFrame_h

#include <stdio.h>
#include "Joint.h"

const int JOINT_TOTAL = 25;

class BodyFrame {
    
private:
    Joint joints[JOINT_TOTAL];
    int currJointCount;
    
public:
    BodyFrame();
    bool addJoint(Joint currJoint);
    Joint* sortJointsByParent(); //Sorts joints array by the parent of each joint: parent x must be defined above the joint with x as parent, returns sorted array
    Joint* getJoints(); //Returns joints as stored in file
    int getCurrJointCount();
    void freeJoints();
};

#endif

