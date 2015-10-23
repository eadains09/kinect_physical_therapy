//
//  Joint.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "Joint.h"

Joint::Joint() {
    setType(JOINT_DEFAULT);
    xPos = NULL;
    yPos = NULL;
    zPos = NULL;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

Joint::Joint(int jType, int x, int y) {
    setType(jType);
    xPos = x;
    yPos = y;
    zPos = NULL;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

Joint::Joint(int jType, int x, int y, int z) {
    setType(jType);
    xPos = x;
    yPos = y;
    zPos = z;
    xQuat = NULL;
    yQuat = NULL;
    zQuat = NULL;
    wQuat = NULL;
}

Joint::Joint(int jType, double x, double y, double z, double w) {
    setType(jType);
    xPos = NULL;
    yPos = NULL;
    zPos = NULL;
    xQuat = x;
    yQuat = y;
    zQuat = z;
    wQuat = w;
}


void Joint::setType(int jType) {
    type = (JointType)jType;
    setParent();
}

JointType Joint::getType() {
    return type;
}

void Joint::setParent() {
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

JointType Joint::getParent() {
    return parent;
}

void Joint::setXPos(int x) {
    xPos = x;
}

void Joint::setYPos(int y) {
    yPos = y;
}

void Joint::setZPos(int z) {
    zPos = z;
}

SDL_Point* Joint::getSDLPoint() {
    SDL_Point *currPoint = new SDL_Point({xPos, yPos});
    
    return currPoint;
}

int Joint::getX() {
    return xPos;
}

int Joint::getY() {
    return yPos;
}

int Joint::getZ() {
    return zPos;
}

void Joint::setQuaternion(double x, double y, double z, double w) {
    xQuat = x;
    yQuat = y;
    zQuat = z;
    wQuat = w;
}

double Joint::getXQuat() {
    return xQuat;
}

double Joint::getYQuat() {
    return yQuat;
}

double Joint::getZQuat() {
    return zQuat;
}

double Joint::getWQuat() {
    return wQuat;
}

//TODO: Is this whole chain of freeing even necessary? Or does built in garbage collection/destructors handle it?
void Joint::freeJoint() {
    type = JOINT_DEFAULT;
    parent = JOINT_DEFAULT;
    xPos = 0;
    yPos = 0;
}