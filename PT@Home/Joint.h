//
//  Joint.h
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef _JOINT_H__
#define _JOINT_H__

#include <stdio.h>
#include <SDL.h>
#include <Kinect.h>

class eJoint {
    
private:
    JointType type;
    JointType parent;
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
    JointType getType();
    JointType getParent();
    void setXPos(int x);
    void setYPos(int y);
    void setZPos(int y);
    //SDL_Point* getSDLPoint();
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


#endif /* _JOINT_H__ */
