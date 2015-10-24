//
//  Joint.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef eJoint_h
#define eJoint_h

#include <stdio.h>
#include <SDL.h>

enum eJointType {
    SPINE_BASE,
    SPINE_MID,
    NECK,
    HEAD,
    SHOULDER_LEFT,
    ELBOW_LEFT,
    WRIST_LEFT,
    HAND_LEFT,
    SHOULDER_RIGHT,
    ELBOW_RIGHT,
    WRIST_RIGHT,
    HAND_RIGHT,
    HIP_LEFT,
    KNEE_LEFT,
    ANKLE_LEFT,
    FOOT_LEFT,
    HIP_RIGHT,
    KNEE_RIGHT,
    ANKLE_RIGHT,
    FOOT_RIGHT,
    SPINE_SHOULDER,
    HAND_TIP_LEFT,
    THUMB_LEFT,
    HAND_TIP_RIGHT,
    THUMB_RIGHT,
    JOINT_DEFAULT
};

class eJoint {
    
private:
    eJointType type;
    eJointType parent;
    int xPos;
    int yPos;
    int zPos;
    double xQuat;
    double yQuat;
    double zQuat;
    double wQuat;
    
    void setParent();
    
public:
    eJoint();
    eJoint(int jType, int x, int y);
    eJoint(int jType, int x, int y, int z);
    eJoint(int jType, double x, double y, double z, double w);
    void setType(int jType);
    eJointType getType();
    eJointType getParent();
    void setXPos(int x);
    void setYPos(int y);
    void setZPos(int y);
    SDL_Point* getSDLPoint();
    int getX();
    int getY();
    int getZ();
    void setQuaternion(double x, double y, double z, double w);
    double getXQuat();
    double getYQuat();
    double getZQuat();
    double getWQuat();
    void freeJoint();
};


#endif
