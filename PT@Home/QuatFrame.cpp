//
//  QuatFrame.cpp
//

#include "QuatFrame.h"
#include "Display.h"
#include "DisplayAction.h"

irr::f32 QuatFrame::getBoneLength(int i)
{
	switch (i) {
	case JointType_SpineBase:// to spine_mid
		return 1;
	case JointType_SpineMid:// should never get called... maybe
		return 1;
	case JointType_Neck:// to spine shoulder
		return .5;
	case JointType_Head:// to neck
		return .5;
	case JointType_ShoulderLeft:// to spine shoulder
		return .5;
	case JointType_ElbowLeft:// to shoulder left
		return 1;
	case JointType_WristLeft:// to elbow left
		return 1;
	case JointType_HandLeft:// to wrist left
		return .2;
	case JointType_HandTipLeft:// to hand left
		return .2;
	case JointType_ThumbLeft:// to hand left
		return .2;
	case JointType_ShoulderRight:// to spine shoulder
		return .5;
	case JointType_ElbowRight:// to shoulder right
		return 1;
	case JointType_WristRight:// to elbow right
		return 1;
	case JointType_HandRight:// to wrist right
		return .2;
	case JointType_HandTipRight:// to hand right
		return .2;
	case JointType_ThumbRight:// to hand right
		return .2;
	case JointType_HipLeft:// to spine base
		return .5;
	case JointType_KneeLeft:// to hip left
		return 1;
	case JointType_AnkleLeft:// to knee left
		return 1;
	case JointType_FootLeft:// to ankle left
		return .5;
	case JointType_HipRight:// to spine base
		return .5;
	case JointType_KneeRight:// to hip right
		return 1;
	case JointType_AnkleRight:// to knee right
		return 1;
	case JointType_FootRight:// to ankle right
		return .5;
	case JointType_SpineShoulder:// to spine mid
		return 1;
	default://make stupid warning go away
		return 1;
	}
}

QuatFrame::QuatFrame()
{
	joints = new irr::core::vector3df*[JOINT_TOTAL];
	jointQuats = new irr::core::quaternion*[JOINT_TOTAL];
	bones = new irr::core::vector3df*[JOINT_TOTAL];
	currJointCount = 0;
	currQuatCount = 0;

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		jointQuats[i] = NULL;
		joints[i] = NULL;
	}
}

QuatFrame::QuatFrame(BodyFrame base)
{
	joints = new irr::core::vector3df*[JOINT_TOTAL];
	jointQuats = new irr::core::quaternion*[JOINT_TOTAL];
	bones = new irr::core::vector3df*[JOINT_TOTAL];
	currJointCount = 0;
	currQuatCount = 0;

	timestamp = base.getTimestamp();

	irr::core::vector3df **points = base.getJoints();
	for (int i = 0; i < base.getCurrJointCount(); i++)
		addJoint(*points[i]);
}

void QuatFrame::initFromBodyFrame(BodyFrame source)
{
	irr::core::vector3df **points = source.getJoints();
	bool reInit = true;
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		if (joints[i] != NULL)
			joints[i]->set(*points[i]);
		else
		{
			addJoint(*points[i]);
			reInit = false;
		}
	}
	if (reInit)
		init();
}

QuatFrame *QuatFrame::slerp(QuatFrame next, irr::f32 time)
{
	QuatFrame *inter = new QuatFrame();
	for (int i = 0; i < JOINT_TOTAL; i++)
		inter->addQuatJoint(this->jointQuats[i]->slerp(*this->jointQuats[i], *next.jointQuats[i], time));

	irr::core::vector3df interSpine = this->joints[JointType_SpineMid]->getInterpolated(*next.joints[JointType_SpineMid], time);

	inter->addMidSpine(interSpine);

	return inter;
}

bool QuatFrame::addJoint(float x, float y, float z)
{
	joints[currJointCount++] = new irr::core::vector3df(x, y, z);
	if (!isReady())
		return false;

	init();
	return true;
}

bool QuatFrame::addJoint(irr::core::vector3df joint)
{
	joints[currJointCount++] = new irr::core::vector3df(joint);
	if (!isReady())
		return false;

	init();
	return true;
}

bool QuatFrame::addQuatJoint(irr::core::quaternion joint)
{
	bool success = true;
	this->jointQuats[currQuatCount] = new irr::core::quaternion(joint);
	for (int i = 0; i < JOINT_TOTAL; i++)
		if (jointQuats[i] == NULL)
			success = false;

	this->jointQuats[currJointCount]->normalize();

	return isReady();
}

bool QuatFrame::isReady()
{
	//math and addition logic could be generalized more so that joints and quaternions
	//can be interleaved any which way and added in any order and as long as we have
	//enough information we can proceed. However this requires a good deal of math
	//to deal with deriving things from things and crazily convoluted compresssed logic 
	//and those things are super not happening right now.

	return joints[JointType_SpineMid] && (currJointCount == JOINT_TOTAL || currQuatCount == JOINT_TOTAL);
}

void QuatFrame::initBodyFrame(BodyFrame frame)
{
	if (!isReady())
	{
		return;
	}

	irr::core::vector3df **points = frame.getJoints();

	//flush points
//	for (int i = 0; i < JOINT_TOTAL; i++)
//		if (i != JointType_SpineMid && points[i] != NULL)
//			points[i] = NULL;

	//build points from our quaternions
//	for (int i = 0; i < JOINT_TOTAL; i++)
//		if (points[i] == NULL)
//			points[i] = getPoint(i);
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		irr::core::vector3df *temp = getPoint(i);
		points[i]->set(*temp);
		delete temp;
	}
}

void QuatFrame::addMidSpine(irr::core::vector3df mid)
{
	bool success = true;
	joints[JointType_SpineMid] = new irr::core::vector3df(mid);

	for (int i = 0; i < JOINT_TOTAL; i++)
		if (jointQuats[i] == NULL)
			success = false;

	if (success)
	{
		currJointCount = JOINT_TOTAL;
	}
}

//this is now a memory leak waiting to happen
irr::core::vector3df *QuatFrame::getPoint(int i)
{
	if (i == JointType_SpineMid)
	{
		return joints[JointType_SpineMid];
	}

	irr::core::quaternion *inv = new irr::core::quaternion(*jointQuats[i]);
	inv->makeInverse();
	irr::core::vector3df *temp = new irr::core::vector3df(*getBone(getParent(i)));
	temp->normalize();

	irr::core::quaternion *fake = new irr::core::quaternion(temp->X, temp->Y, temp->Z);

	fake->set(((*jointQuats[i]) * (*fake)*(*inv)));
	temp->X = fake->X;
	temp->Y = fake->Y;
	temp->Z = fake->Z;
	delete fake, inv;

	temp->setLength(getBoneLength(i)*60);
	*temp += *getPoint(getParent(i));

	return temp;
//	joints[i] = temp;

//	return joints[i];
}

irr::core::vector3df *QuatFrame::getBone(int i)
{
	//TODO body is rotated about midSpine vector, make that not happen anymore
	if (i == JointType_SpineMid)
	{
		bones[JointType_SpineMid] = new irr::core::vector3df(*joints[JointType_SpineMid]);
		return bones[JointType_SpineMid];
	}
	bones[i] = new irr::core::vector3df(*joints[i] - *joints[getParent(i)]);
	return bones[i];
}



void QuatFrame::init()
{
	for (int i = 0; i < JointType_Count; i++)
		getBone(i);

	for (int i = 0; i < JointType_Count; i++)
	{
		irr::core::quaternion *quat;
		irr::core::matrix4 *mat = new irr::core::matrix4();

		irr::core::vector3df *ourBone;
		irr::core::vector3df *parentBone;
		ourBone = new irr::core::vector3df(*bones[i]);
		ourBone->normalize();

		if (getParent(i) == i)
			parentBone = new irr::core::vector3df(0, 1, 0);
		else
			parentBone = new irr::core::vector3df(*bones[getParent(i)]);
		
		parentBone->normalize();


		mat->buildRotateFromTo(*parentBone, *ourBone);

		quat = new irr::core::quaternion(*mat);
		delete mat, parentBone, ourBone;
		quat->normalize();
		jointQuats[i] = quat;

	}
}

void QuatFrame::setTimestamp(double ts) {
	timestamp = ts;
}

double QuatFrame::getTimestamp() {
	return timestamp;
}


void QuatFrame::writeFrame(FileWriter *currFile) {
	currFile->logTimestampMidspine(timestamp, *joints[JointType_SpineMid]);
	currFile->openBodyFrame();
	if (isReady()) {
		currFile->logDataQuat(jointQuats[0]->X, jointQuats[0]->Y, jointQuats[0]->Z, jointQuats[0]->W);
		for (int i = 1; i < currJointCount; i++) {
			currFile->addComma();
			currFile->logDataQuat(jointQuats[i]->X, jointQuats[i]->Y, jointQuats[i]->Z, jointQuats[i]->W);
		}
	}
	currFile->closeBodyFrame();
	currFile->closeKeyframe();
}


QuatFrame::~QuatFrame()
{
	if (isReady())
		delete[] jointQuats;

	delete[] joints;
}