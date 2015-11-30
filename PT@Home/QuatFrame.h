//
//  QuatFrame.h
//

#pragma once

#include <stdio.h>
#include <quaternion.h>
#include <irrlicht.h>
//#include "Joint.h"
#include "BodyFrame.h"
#include "FileWriter.h"


//const int JOINT_TOTAL = 25;

class QuatFrame {

private:
	double timestamp;
	//TODO could probably do a(couple) nice neat deque(s) with considerably less
	//indirection
	irr::core::quaternion **jointQuats;
	irr::core::vector3df **joints;
	irr::core::vector3df **bones;
	irr::core::vector3df midSpine;
	int currJointCount;
	int currQuatCount;
	void init();
	irr::core::vector3df *getPoint(int);
	irr::core::vector3df *getBone(int);

public:
	QuatFrame();
	QuatFrame(BodyFrame);
	QuatFrame *slerp(QuatFrame next, irr::f32 time);
	bool addJoint(float x, float y, float z);
	bool addJoint(irr::core::vector3df joint);
	bool addQuatJoint(irr::core::quaternion joint);
	bool isReady();
	void initBodyFrame(BodyFrame frame);
	void addMidSpine(irr::core::vector3df mid);
	irr::f32 getBoneLength(int i);
	void setTimestamp(double ts);
	double getTimestamp();
	void writeFrame(FileWriter *currFile);
	virtual ~QuatFrame();

};