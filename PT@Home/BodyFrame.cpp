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
	joints = new irr::core::vector3df*[JOINT_TOTAL];

	for (int i = 0; i < JOINT_TOTAL; i++)
		joints[i] = NULL;//new irr::core::vector3df();
}

bool BodyFrame::addJoint(irr::core::vector3df *currJoint) {
    bool success = false;
    
    if (currJointCount < JOINT_TOTAL) {
        joints[currJointCount] = currJoint;
        currJointCount++;
        success = true;
    }
    
    return success;
}

irr::core::vector3df** BodyFrame::sortJointsByParent() {
    //eJoint *sortedJoints[JOINT_TOTAL];
	irr::core::vector3df** sortedJoints = new irr::core::vector3df *[JOINT_TOTAL];

    return sortedJoints;
}

irr::core::vector3df** BodyFrame::getJoints() {
    return joints;
}

int BodyFrame::getCurrJointCount() {
    return currJointCount;
}


BodyFrame::~BodyFrame()
{
//	for (int i = 0; i < currJointCount; i++) 
//		delete joints[i];

//	delete [] joints;
}
