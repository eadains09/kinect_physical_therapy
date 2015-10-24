//
//  Joint.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "eJoint.h"

eJoint::eJoint() {
    setType(JOINT_DEFAULT);
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
    type = (eJointType)jType;
    setParent();
}

eJointType eJoint::getType() {
    return type;
}

void eJoint::setParent() {
    switch (type) {
        case SPINE_BASE:
            parent = SPINE_MID;
            break;
        case SPINE_MID:
            parent = JOINT_DEFAULT;
            break;
        case NECK:
            parent = SPINE_SHOULDER;
            break;
        case HEAD:
            parent = NECK;
            break;
        case SHOULDER_LEFT:
            parent = SPINE_SHOULDER;
            break;
        case ELBOW_LEFT:
            parent = SHOULDER_LEFT;
            break;
        case WRIST_LEFT:
            parent = ELBOW_LEFT;
            break;
        case HAND_LEFT:
            parent = WRIST_LEFT;
            break;
        case HAND_TIP_LEFT:
            parent = HAND_LEFT;
            break;
        case THUMB_LEFT:
            parent = HAND_LEFT;
            break;
        case SHOULDER_RIGHT:
            parent = SPINE_SHOULDER;
            break;
        case ELBOW_RIGHT:
            parent = SHOULDER_RIGHT;
            break;
        case WRIST_RIGHT:
            parent = ELBOW_RIGHT;
            break;
        case HAND_RIGHT:
            parent = WRIST_RIGHT;
            break;
        case HAND_TIP_RIGHT:
            parent = HAND_RIGHT;
            break;
        case THUMB_RIGHT:
            parent = HAND_RIGHT;
            break;
        case HIP_LEFT:
            parent = SPINE_BASE;
            break;
        case KNEE_LEFT:
            parent = HIP_LEFT;
            break;
        case ANKLE_LEFT:
            parent = KNEE_LEFT;
            break;
        case FOOT_LEFT:
            parent = ANKLE_LEFT;
            break;
        case HIP_RIGHT:
            parent = SPINE_BASE;
            break;
        case KNEE_RIGHT:
            parent = HIP_RIGHT;
            break;
        case ANKLE_RIGHT:
            parent = KNEE_RIGHT;
            break;
        case FOOT_RIGHT:
            parent = ANKLE_RIGHT;
            break;
        case SPINE_SHOULDER:
            parent = SPINE_MID;
            break;
        default:
            parent = JOINT_DEFAULT;
            break;
    }
    
}

eJointType eJoint::getParent() {
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

SDL_Point* eJoint::getSDLPoint() {
    SDL_Point *currPoint = new SDL_Point({xPos, yPos});
    
    return currPoint;
}

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
    type = JOINT_DEFAULT;
    parent = JOINT_DEFAULT;
    xPos = 0;
    yPos = 0;
}