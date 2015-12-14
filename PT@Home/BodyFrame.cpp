/*
 *  File:              BodyFrame.cpp
 *  Purpose:           This class is the representation of the body when it's in point (X, Y, Z) form.
 *                     Currently it is only used as the object we display to screen using SDL.
 *
 *  Member variables:  timestamp, joint count, and array of joints
 *
 *  Created by Erika Dains on 10/12/15.
 */

#include "BodyFrame.h"

// Constructor
BodyFrame::BodyFrame() {
    currJointCount = 0;
    timestamp = 0;
    joints = new irr::core::vector3df*[JOINT_TOTAL];

    for (int i = 0; i < JOINT_TOTAL; i++)
        joints[i] = new irr::core::vector3df();
}

// Copy Constructor
BodyFrame::BodyFrame(const BodyFrame& source)
{
	currJointCount = source.currJointCount;
	joints = new irr::core::vector3df*[JOINT_TOTAL];


	for (int i = 0; i < currJointCount; i++)
		joints[i] = new irr::core::vector3df(*source.joints[i]);

	timestamp = source.timestamp;
}


// Adds a single joint to the array of joints, increments currJointCount
// Will return false if there are already the correct amount of joints in joint array
bool BodyFrame::addJoint(irr::core::vector3df *currJoint) {
    bool success = false;
    
    if (currJointCount < JOINT_TOTAL) {
		joints[currJointCount]->set(*currJoint);
        currJointCount++;
        success = true;
    }
    
    return success;
}

// Returns true if all necessary joints have been stored into joints array
bool BodyFrame::isReady()
{
	return currJointCount == JOINT_TOTAL;
}

irr::core::vector3df** BodyFrame::getJoints() {
    return joints;
}

int BodyFrame::getCurrJointCount() {
    return currJointCount;
}

// Destructor
BodyFrame::~BodyFrame()
{
	for (int i = 0; i < JOINT_TOTAL; i++)
		delete joints[i];

	delete[] joints;
}

void BodyFrame::setTimestamp(double ts) {
    timestamp = ts;
}

double BodyFrame::getTimestamp() {
    return timestamp;
}

// This function was a hack to convert points from Kinect representation to SDL's
// It should now be unnecessary
void BodyFrame::transformPoints()
{
	if (joints == NULL || currJointCount != JOINT_TOTAL)
		return;

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		joints[i]->X = (joints[i]->X + 1) * 200;
		joints[i]->Y = (joints[i]->Y - 1) * -200;
		joints[i]->Z = (joints[i]->Z + 1) * 200;
	}
}
