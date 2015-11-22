//
//  Joint.cpp
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "Joint.h"

eJoint::eJoint() {
    setType(JointType_SpineMid);
    xPos = NULL;
    yPos = NULL;
    zPos = NULL;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

eJoint::eJoint(int jType, int x, int y) {
    setType(jType);
    xPos = x;
    yPos = y;
    zPos = NULL;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

eJoint::eJoint(int jType, int x, int y, int z) {
    setType(jType);
    xPos = x;
    yPos = y;
    zPos = z;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

eJoint::eJoint(int jType, double x, double y, double z, double w) {
    setType(jType);
    xPos = NULL;
    yPos = NULL;
    zPos = NULL;
    xQuat = x;
    yQuat = y;
    zQuat = z;
    wQuat = w;
}


void eJoint::setType(int jType) {
    type = (JointType)jType;
    setParent();
}

JointType eJoint::getType() {
    return type;
}

void eJoint::setParent() {
    switch (type) {
	case JointType_SpineBase:// SPINE_BASE:
		parent = JointType_SpineMid;// SPINE_MID;
            break;
	case JointType_SpineMid:// SPINE_MID:
		parent = JointType_SpineMid;// JOINT_DEFAULT;
            break;
	case JointType_Neck:// NECK:
		parent = JointType_SpineShoulder;// SPINE_SHOULDER;
            break;
	case JointType_Head:// HEAD:
		parent = JointType_Neck;// NECK;
            break;
	case JointType_ShoulderLeft:// SHOULDER_LEFT:
		parent = JointType_SpineShoulder;// SPINE_SHOULDER;
            break;
	case JointType_ElbowLeft:// ELBOW_LEFT:
		parent = JointType_ShoulderLeft;// SHOULDER_LEFT;
            break;
	case JointType_WristLeft:// WRIST_LEFT:
		parent = JointType_ElbowLeft;// ELBOW_LEFT;
            break;
	case JointType_HandLeft:// HAND_LEFT:
		parent = JointType_WristLeft;// WRIST_LEFT;
            break;
	case JointType_HandTipLeft:// HAND_TIP_LEFT:
		parent = JointType_HandLeft;// HAND_LEFT;
            break;
	case JointType_ThumbLeft:// THUMB_LEFT:
		parent = JointType_HandLeft;// HAND_LEFT;
            break;
	case JointType_ShoulderRight:// SHOULDER_RIGHT:
		parent = JointType_SpineShoulder; //SPINE_SHOULDER;
            break;
	case JointType_ElbowRight:// ELBOW_RIGHT:
		parent = JointType_ShoulderRight;// SHOULDER_RIGHT;
            break;
	case JointType_WristRight:// WRIST_RIGHT:
		parent = JointType_ElbowRight;// ELBOW_RIGHT;
            break;
	case JointType_HandRight:// HAND_RIGHT:
		parent = JointType_WristRight;// WRIST_RIGHT;
            break;
	case JointType_HandTipRight:// HAND_TIP_RIGHT:
		parent = JointType_HandRight;// HAND_RIGHT;
            break;
	case JointType_ThumbRight:// THUMB_RIGHT:
		parent = JointType_HandRight;// HAND_RIGHT;
            break;
	case JointType_HipLeft:// HIP_LEFT:
		parent = JointType_SpineBase;// SPINE_BASE;
            break;
	case JointType_KneeLeft:// KNEE_LEFT:
		parent = JointType_HipLeft;// HIP_LEFT;
            break;
	case JointType_AnkleLeft:// ANKLE_LEFT:
		parent = JointType_KneeLeft;// KNEE_LEFT;
            break;
	case JointType_FootLeft:// FOOT_LEFT:
		parent = JointType_AnkleLeft;// ANKLE_LEFT;
            break;
	case JointType_HipRight:// HIP_RIGHT:
		parent = JointType_SpineBase;// SPINE_BASE;
            break;
	case JointType_KneeRight:// KNEE_RIGHT:
		parent = JointType_HipRight;// HIP_RIGHT;
            break;
	case JointType_AnkleRight:// ANKLE_RIGHT:
		parent = JointType_KneeRight;// KNEE_RIGHT;
            break;
	case JointType_FootRight:// FOOT_RIGHT:
		parent = JointType_AnkleRight;// ANKLE_RIGHT;
            break;
	case JointType_SpineShoulder:// SPINE_SHOULDER:
		parent = JointType_SpineMid;// SPINE_MID;
            break;
        default:
            parent = type;
            break;
    }
    
}

JointType eJoint::getParent() {
    return parent;
}

void eJoint::setXPos(int x) {
    xPos = x;
}

void eJoint::setYPos(int y) {
    yPos = y;
}

void eJoint::setZPos(int z) {
    zPos = z;
}

/*SDL_Point* eJoint::getSDLPoint() {
    SDL_Point *currPoint = new SDL_Point({xPos, yPos});
    
    return currPoint;
}
*/

int eJoint::getX() {
    return xPos;
}

int eJoint::getY() {
    return yPos;
}

int eJoint::getZ() {
    return zPos;
}

void eJoint::setQuaternion(double x, double y, double z, double w) {
    xQuat = x;
    yQuat = y;
    zQuat = z;
    wQuat = w;
}

double eJoint::getXQuat() {
    return xQuat;
}

double eJoint::getYQuat() {
    return yQuat;
}

double eJoint::getZQuat() {
    return zQuat;
}

double eJoint::getWQuat() {
    return wQuat;
}

//TODO: Is this whole chain of freeing even necessary? Or does built in garbage collection/destructors handle it?
void eJoint::freeJoint() {
    type = (JointType)0;
    parent = (JointType)0;
    xPos = 0;
    yPos = 0;
}