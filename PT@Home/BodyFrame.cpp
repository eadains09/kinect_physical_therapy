//
//  Joints.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "BodyFrame.h"

BodyFrame::BodyFrame() {
    currJointCount = 0;
}

bool BodyFrame::addJoint(eJoint currJoint) {
    bool success = false;
    
    if (currJointCount < JOINT_TOTAL) {
        joints[currJointCount] = currJoint;
        currJointCount++;
        success = true;
    }
    
    return success;
}

eJoint* BodyFrame::sortJointsByParent() {
    eJoint *sortedJoints[JOINT_TOTAL];
    
    return *sortedJoints;
}

eJoint* BodyFrame::getJoints() {
    return joints;
}

int BodyFrame::getCurrJointCount() {
    return currJointCount;
}

void BodyFrame::freeJoints() {
    for (int i = 0; i < currJointCount; i++) {
        joints[i].freeJoint();
    }
}
