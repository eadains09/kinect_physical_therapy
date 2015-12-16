//
//  QuatFrame.h
//

#pragma once

#include <stdio.h>
#include <quaternion.h>
#include <irrlicht.h>
#include "BodyFrame.h"
#include "FileWriter.h"

bool getBit(int bitField, int n);
#define QUAT_COMPARE_THRESHOLD  .6

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
	QuatFrame(const QuatFrame& source);
	int compare(QuatFrame *other);
	void initFromBodyFrame(BodyFrame source);
	QuatFrame *slerp(const QuatFrame& next, irr::f32 time);
	bool addJoint(irr::f32 x, irr::f32 y, irr::f32 z);
	bool addJoint(const irr::core::vector3df& joint);
	bool addQuatJoint(const irr::core::quaternion& joint);
	bool isReady();
	void initBodyFrame(BodyFrame *frame);
	void addMidSpine(const irr::core::vector3df& mid);
	irr::f32 getBoneLength(int i);
	void setTimestamp(double ts);
	double getTimestamp();
	void writeFrame(FileWriter *currFile);
	virtual ~QuatFrame();

};