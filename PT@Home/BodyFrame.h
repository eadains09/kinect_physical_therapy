//
//  Body.h
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "Joint.h"
#include <quaternion.h>

const int JOINT_TOTAL = 25;

class BodyFrame {
    
private:

    
public:
	irr::core::vector3df **joints;
//	eJoint joints[JOINT_TOTAL];
	int currJointCount;
    BodyFrame();
    bool addJoint(irr::core::vector3df *currJoint);
    irr::core::vector3df** sortJointsByParent(); //Sorts joints array by the parent of each joint: parent x must be defined above the joint with x as parent, returns sorted array
	irr::core::vector3df** getJoints(); //Returns joints as stored in file
    int getCurrJointCount();
	virtual ~BodyFrame();
};


