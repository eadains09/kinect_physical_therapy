//
//  Joint.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef PlayBodyPoints_Joint_h
#define PlayBodyPoints_Joint_h

#include <stdio.h>
#include <SDL2/SDL.h>

enum JointType {
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

class Joint {
    
private:
    JointType type;
    JointType parent;
    int xPos;
    int yPos;
    
    void setParent();
    
public:
    Joint();
    Joint(int jType, int x, int y);
    void setType(int jType);
    JointType getType();
    JointType getParent();
    void setXPos(int x);
    void setYPos(int y);
    SDL_Point* getSDLPoint();
};


#endif
