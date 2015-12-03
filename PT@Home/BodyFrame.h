//
//  BodyFrame.h
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "Joint.h"
#include <quaternion.h>
#include "FileWriter.h"

const int JOINT_TOTAL = 25;

class BodyFrame {
    
private:
    double timestamp;
	int currJointCount;
    
public:
	irr::core::vector3df **joints;
    BodyFrame();
	BodyFrame(const BodyFrame& source);
    bool addJoint(irr::core::vector3df *currJoint);
	irr::core::vector3df** getJoints(); //Returns joints as stored in file
    int getCurrJointCount();
	virtual ~BodyFrame();
    void setTimestamp(double ts);
    double getTimestamp();
	void transformPoints();
//    void writeFrame(FileWriter *currFile);
};