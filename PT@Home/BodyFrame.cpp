//
//  BodyFrame.cpp
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

//since we are going to immediately transform points to quaternions
//upon reading them in from kinect and we save keyframes as quaternions
//this means that a BodyFrame can only be in one of two valid states
//transformed and untransformed, ie just read in from kinect or about
//to be written to sdl, and it seems likely that keeping track of which
//state a particular BodyFrame is in with a little boolean would be useful

#include "BodyFrame.h"

BodyFrame::BodyFrame() {
    currJointCount = 0;
	joints = new irr::core::vector3df*[JOINT_TOTAL];

	for (int i = 0; i < JOINT_TOTAL; i++)
		joints[i] = new irr::core::vector3df();
    timestamp = 0;
}
//
BodyFrame::BodyFrame(const BodyFrame& source)
{
	currJointCount = source.currJointCount;
	joints = new irr::core::vector3df*[JOINT_TOTAL];

//	irr::core::vector3df ** points = source.getJoints();

	for (int i = 0; i < currJointCount; i++)
	{
		joints[i] = new irr::core::vector3df();
		joints[i]->set(*source.joints[i]); //= new irr::core::vector3df(*source.joints[i]);
	}

	timestamp = source.timestamp;
}


bool BodyFrame::addJoint(irr::core::vector3df *currJoint) {
    bool success = false;
    
    if (currJointCount < JOINT_TOTAL) {
		joints[currJointCount]->set(*currJoint); //= currJoint;
        currJointCount++;
        success = true;
    }
    
    return success;
}

irr::core::vector3df** BodyFrame::getJoints() {
    return joints;
}

int BodyFrame::getCurrJointCount() {
    return currJointCount;
}


BodyFrame::~BodyFrame()
{
	delete[] joints;
}

void BodyFrame::setTimestamp(double ts) {
    timestamp = ts;
}

double BodyFrame::getTimestamp() {
    return timestamp;
}

void BodyFrame::transformPoints()
{
	if (joints == NULL || currJointCount != JOINT_TOTAL)
		return;

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		joints[i]->X = (joints[i]->X + 1) * 200;
		joints[i]->Y = (joints[i]->Y - 1) * -200;
		joints[i]->Z = (joints[i]->Z + 1) * 200;
	//	*xPos = (*xPos + 1) * 200;
	//	*yPos = (*yPos - 1) * -200;
	//	*zPos = (*zPos + 1) * 200;
	}
}
//this function should be useless now
/*
void BodyFrame::writeFrame(FileWriter *currFile) {
    (*currFile).logTimestamp(timestamp);
    (*currFile).openBodyFrame();
    if (currJointCount > 0) {
        (*currFile).logDataPoint(joints[0]->X, joints[0]->Y, joints[0]->Z);
        for (int i = 1; i < currJointCount; i++) {
            (*currFile).addComma();
            (*currFile).logDataPoint(joints[i]->X, joints[i]->Y, joints[i]->Z);
        }
    }
    (*currFile).closeBodyFrame();
    (*currFile).closeKeyframe();
}
*/