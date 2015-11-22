//
//  BodyFrame.h
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef _BODYFRAME_H_
#define _BODYFRAME_H_

#include <stdio.h>
#include "Joint.h"
#include "FileWriter.h"

const int JOINT_TOTAL = 25;

class BodyFrame {
    
private:
    eJoint joints[BodyFrame::JOINT_TOTAL];
    int currJointCount;
    
public:
    BodyFrame();
    bool addJoint(eJoint currJoint);
    eJoint* sortJointsByParent(); //Sorts joints array by the parent of each joint: parent x must be defined above the joint with x as parent, returns sorted array
    eJoint* getJoints(); //Returns joints as stored in file
    int getCurrJointCount();
    void freeJoints();
    void writeFrame(FileWriter *currFile);
};

#endif /* _BODYFRAME_H_ */

