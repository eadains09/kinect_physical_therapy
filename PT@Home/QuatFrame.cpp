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
		jointQuats[i] = new irr::core::quaternion();
		joints[i] = new irr::core::vector3df();
		bones[i] = new irr::core::vector3df();
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

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		jointQuats[i] = new irr::core::quaternion();
		joints[i] = new irr::core::vector3df();
		bones[i] = new irr::core::vector3df();
	}

	irr::core::vector3df **points = base.getJoints();
	for (int i = 0; i < base.getCurrJointCount(); i++)
		addJoint(*points[i]);
}

QuatFrame::QuatFrame(const QuatFrame& source)
{
	joints = new irr::core::vector3df*[JOINT_TOTAL];
	jointQuats = new irr::core::quaternion*[JOINT_TOTAL];
	bones = new irr::core::vector3df*[JOINT_TOTAL];
	currJointCount = 0;
	currQuatCount = 0;
	timestamp = source.timestamp;

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		jointQuats[i] = new irr::core::quaternion();
		joints[i] = new irr::core::vector3df();
		bones[i] = new irr::core::vector3df();
	}
	for (int i = 0; i < JOINT_TOTAL; i++)
		addQuatJoint(irr::core::quaternion(*source.jointQuats[i]));

	addMidSpine(irr::core::vector3df(*source.bones[JointType_SpineMid]));
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
//slerps between two QuatFrames. 
//Time is the time argument
//passed to irrlicht's slerp method, which expects a value
//between 0 and 1.
QuatFrame *QuatFrame::slerp(const QuatFrame& next, irr::f32 time)
{
	QuatFrame *inter = new QuatFrame();
	for (int i = 0; i < JOINT_TOTAL; i++)
		inter->addQuatJoint(this->jointQuats[i]->slerp(*this->jointQuats[i], *next.jointQuats[i], time, .00000005f));

	irr::core::vector3df interSpine = this->joints[JointType_SpineMid]->getInterpolated(*next.joints[JointType_SpineMid], time);

	inter->addMidSpine(interSpine);

	return inter;
}

bool QuatFrame::addJoint(float x, float y, float z)
{
	joints[currJointCount]->X = x;
	joints[currJointCount]->Y = y;
	joints[currJointCount]->Z = z;
	currJointCount++;

	if (!isReady())
		return false;

	init();
	return true;
}

bool QuatFrame::addJoint(const irr::core::vector3df& joint)
{
	joints[currJointCount++]->set(joint);
	if (!isReady())
		return false;

	init();
	return true;
}


bool QuatFrame::addQuatJoint(const irr::core::quaternion& joint)
{
	if (currQuatCount >= JOINT_TOTAL)
		return false;
	this->jointQuats[currQuatCount]->set(joint);
//	this->jointQuats[currQuatCount]->normalize();
	currQuatCount++;

	if (isReady())
		init();

	return true;
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

void QuatFrame::initBodyFrame(BodyFrame *frame)
{
	if (!isReady())
	{
		return;
	}

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		irr::core::vector3df *temp = getPoint(i);
		frame->addJoint(temp);
		delete temp;
	}
}

void QuatFrame::addMidSpine(const irr::core::vector3df& mid)
{
	//increment currJointCount?
	joints[JointType_SpineMid]->set(mid); //= new irr::core::vector3df(mid);


	if (isReady())
		init();
}

//for getPoint and getBone it would be nice
//to more effectively memoize, right now the 
//only memos we have are the midspines

//this is now a memory leak waiting to happen
irr::core::vector3df *QuatFrame::getPoint(int i)
{
	//TODO semi-immediate next steps:
	//1. the mid spine member shall now be
	//a vector3df that is set by a call
	//to setMidSpine and returned here
	//instead of a constant (400, 300, 0)
	//vector
	//2. we will eliminate the joints array
	//instead building up the bones and
	//jointQuats arrays(/deques?) directly
	//of the BodyFrame joints array
	if (i == JointType_SpineMid)
	{
		//return new irr::core::vector3df(*joints[JointType_SpineMid]);
		return new irr::core::vector3df(400, 300, 0);
	}

	irr::core::quaternion inv = irr::core::quaternion(*jointQuats[i]);
	inv.makeInverse();
	irr::core::vector3df *temp = getBone(getParent(i));
	temp->normalize();

	irr::core::quaternion fake = irr::core::quaternion(temp->X, temp->Y, temp->Z);

	fake.set((*jointQuats[i]) * fake * inv);
	temp->X = fake.X;
	temp->Y = fake.Y;
	temp->Z = fake.Z;

	temp->setLength(getBoneLength(i)*60);
	irr::core::vector3df *tempParent = getPoint(getParent(i));
	*temp += *tempParent;
	delete tempParent;
	irr::core::vector3df *retVal = new irr::core::vector3df(*temp);
	delete temp;

	return retVal;
}

int genMask(int n)
{
	return 1 << n;
}

bool getBit(int field,int n)
{
	return field&genMask(n);
}

int QuatFrame::compare(QuatFrame *other)
{
	if (!(other->isReady() && isReady()))
	{
		return 0;
	}
	int bitField = 0;
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		double total_diff;
		total_diff = abs(other->jointQuats[i]->X - jointQuats[i]->X);
		total_diff += abs(other->jointQuats[i]->Y - jointQuats[i]->Y);
		total_diff += abs(other->jointQuats[i]->Z - jointQuats[i]->Z);
		total_diff += abs(other->jointQuats[i]->W - jointQuats[i]->W);
		if (total_diff >= QUAT_COMPARE_THRESHOLD)
			bitField |= genMask(i);
	}
	return bitField;
}

irr::core::vector3df *QuatFrame::getBone(int i)
{
	//TODO body is rotated about midSpine vector, make that not happen anymore
	if (i == JointType_SpineMid)
	{
		bones[JointType_SpineMid]->set(*joints[JointType_SpineMid]);
		return new irr::core::vector3df(*bones[JointType_SpineMid]);
	}
	if (!bones[i]->equals(irr::core::vector3df(), 0))
		return new irr::core::vector3df(*bones[i]);

	if (currJointCount == JOINT_TOTAL)
		bones[i]->set(*joints[i] - *joints[getParent(i)]);
	else if (currQuatCount == JOINT_TOTAL)
	{
		//generate bone with only the midspine point, quaternions, and the power of recursion
		//TODO double check and make sure this makes sense
		irr::core::quaternion inv = irr::core::quaternion(*jointQuats[i]);
		inv.makeInverse();
		irr::core::vector3df *temp = getBone(getParent(i));
		temp->normalize();

		irr::core::quaternion fake = irr::core::quaternion(temp->X, temp->Y, temp->Z);

		fake.set((*jointQuats[i]) * fake * inv);
		temp->X = fake.X;
		temp->Y = fake.Y;
		temp->Z = fake.Z;

		temp->setLength(getBoneLength(i) * 60);
		bones[i]->set(*temp);
		return new irr::core::vector3df(*temp);

	}
	else
	{
		//we weren't ready, not sure if this case will ever get hit
		return new irr::core::vector3df();
	}
	return new irr::core::vector3df(*bones[i]);
}



void QuatFrame::init()
{
	irr::core::matrix4 mat = irr::core::matrix4();

	irr::core::vector3df ourBone = irr::core::vector3df();
	irr::core::vector3df parentBone = irr::core::vector3df();

	for (int i = 0; i < JointType_Count; i++)
		//TODO this is messy as hell, fix
		//easy way out: special initBone function
		//that works basically the same as getBone
		//but doesnt return anything and has 0
		//net heap change
		//but there must be a better way
		delete getBone(i);

	for (int i = currQuatCount; i < JointType_Count; i++)
	{
		irr::core::quaternion quat;

		ourBone.set(*bones[i]);
		ourBone.normalize();

		//so this is the thing I thought would prevent the
		//bizarre rotationy behavior, as we know it did not
		//TODO find out why it didn't work and do something that does
		if (getParent(i) == i)
		{
			parentBone.set(ourBone);
			ourBone.set(irr::core::vector3df(0, -1, 0));
			ourBone.normalize();
		}
		else
			parentBone.set(*bones[getParent(i)]);
		
		parentBone.normalize();


	//	mat.buildRotateFromTo(parentBone, ourBone);
		mat.buildRotateFromTo(ourBone, parentBone);

		quat = irr::core::quaternion(mat);
		quat.normalize();
		jointQuats[i]->set(quat);
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
		for (int i = 1; i < currQuatCount; i++) {
			currFile->addComma();
			currFile->logDataQuat(jointQuats[i]->X, jointQuats[i]->Y, jointQuats[i]->Z, jointQuats[i]->W);
		}
	}
	currFile->closeBodyFrame();
	currFile->closeKeyframe();
}


QuatFrame::~QuatFrame()
{
	delete[] jointQuats;
	delete[] bones;
	delete[] joints;
}