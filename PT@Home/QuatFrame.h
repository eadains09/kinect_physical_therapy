#pragma once
#include <stdio.h>
#include <quaternion.h>
#include <irrlicht.h>
#include "Joint.h"
#include "BodyFrame.h"
//#include "FloatJoint.h"


//const int JOINT_TOTAL = 25;

class QuatFrame {

private:
	irr::core::quaternion **jointQuats;
	irr::core::vector3df **joints;
	irr::core::vector3df midSpine;
	int currJointCount;
	void init();
	irr::core::vector3df *getPoint(int);

public:
	QuatFrame();
	QuatFrame(BodyFrame);
	bool addJoint(float x, float y, float z);
	bool isReady();
	void initBodyFrame(BodyFrame *frame);
	virtual ~QuatFrame();

};