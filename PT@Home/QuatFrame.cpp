//
//  QuatFrame.cpp
//

#include "QuatFrame.h"
#include "Display.h"
#include "DisplayAction.h"

//returns length for the specified bone
//lengths arrived at by trial and error(mostly error)
//and should not be taken as gospel
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

//Default constructor, allocates, initializes, etc
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

//Constructor that initializes from BodyFrame,
//results in a QuatFrame representing the
//same body as BodyFrame base
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

//Copy constuctor, performs deep copy of source
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

//slerps between two QuatFrames. 
//Time is the time argument
//passed to irrlicht's slerp method, a value
//between 0 and 1 indicating the progress of the
//movement
QuatFrame *QuatFrame::slerp(const QuatFrame& next, irr::f32 time)
{
	//the fact that this works is very confusing
	//it looks like the time we're passing to slerp is too big
	//and I have no idea why that scaling would exist
	//since we're passing in the correct value betweeen 0 and 1
	//so why would slerp do all its slerping in the first ~1/85th
	//is a mystery
//	time /= (double)85;
	time /= 20;
	QuatFrame *inter = new QuatFrame();
	for (int i = 0; i < JOINT_TOTAL; i++)
		inter->addQuatJoint(this->jointQuats[i]->slerp(*this->jointQuats[i], *next.jointQuats[i], time));

	irr::core::vector3df interSpine = this->joints[JointType_SpineMid]->getInterpolated(*next.joints[JointType_SpineMid], time);

	inter->addMidSpine(interSpine);
	inter->setTimestamp(time);

	return inter;
}

//Adds a vector joint from xyz-coordinates
bool QuatFrame::addJoint(irr::f32 x, irr::f32 y, irr::f32 z)
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

//adds a vector joint from an existing vector
bool QuatFrame::addJoint(const irr::core::vector3df& joint)
{
	joints[currJointCount++]->set(joint);

	if (!isReady())
		return false;

	init();
	return true;
}

//adds a quaternion joint from an existing quaternion
bool QuatFrame::addQuatJoint(const irr::core::quaternion& joint)
{
	if (currQuatCount >= JOINT_TOTAL)
		return false;

	this->jointQuats[currQuatCount++]->set(joint);

	if (isReady()) 
		init();

	return true;
}

//returns a boolean value indicating whether or not the QuatFrame
//contains enough information to construct a body. If called outside
//this class it also indicates whether or not the QuatFrame has been inited
//since whenever a piece of information is added we check to see if we isReady()
//and init() if we are
bool QuatFrame::isReady()
{

	//math and addition logic could be generalized more so that joints and quaternions
	//can be interleaved any which way and added in any order and as long as we have
	//enough information we can proceed. However this requires a good deal of math
	//to deal with deriving things from things and crazily convoluted compresssed logic 
	//and those things are super not happening right now.

	return joints[JointType_SpineMid] && (currJointCount == JOINT_TOTAL || currQuatCount == JOINT_TOTAL);
}

//Initialize an empty BodyFrame with the points required to
//display the body represented by this QuatFrame
void QuatFrame::initBodyFrame(BodyFrame *frame)
{
	if (!isReady())
		return;


	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		irr::core::vector3df *temp = getPoint(i);
		frame->addJoint(temp);
		delete temp;
	}
}

//Adds the midSpine joint, not sure if this is worth keeping
//as right now its usefulness is anything but well established
void QuatFrame::addMidSpine(const irr::core::vector3df& mid)
{
	joints[JointType_SpineMid]->set(mid);

	if (isReady())
		init();
}

//returns the vector representing the position of a given joint
irr::core::vector3df *QuatFrame::getPoint(int i)
{
	//Eliminate the joints array
	//instead building up the bones and
	//jointQuats arrays(/deques?) directly
	//of the BodyFrame joints array
	if (i == JointType_SpineMid)
	{
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
	irr::core::vector3df *retVal = new irr::core::vector3df(*temp);
	delete tempParent;
	delete temp;

	return retVal;
}

//returns a mask for the nth to least significant bit
int genMask(int n)
{
	return 1 << n;
}

//returns a boolean value indicating whther or not the provided 
//bitfield's nth to least significant bit was 1
//if using the bitfield generated by the compare method this
//is equivalent to whether the nth joint was in error
bool getBit(int field,int n)
{
	return field&genMask(n);
}

//returns a bitfield in which the 25 least significant
//bits, from least to most signifcant, represent whether
//or not the corresponding joint had enough differences
//between the two QuatFrames to be considered to be
//in error
int QuatFrame::compare(QuatFrame *other)
{
	if (!(other->isReady() && isReady()))
		return 0;

	int bitField = 0;
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		double total_diff;
		total_diff = 2.5 * abs(other->jointQuats[i]->X - jointQuats[i]->X);
		total_diff += 2.5 * abs(other->jointQuats[i]->Y - jointQuats[i]->Y);
		total_diff += 2.5 * abs(other->jointQuats[i]->Z - jointQuats[i]->Z);
		total_diff += abs(other->jointQuats[i]->W - jointQuats[i]->W);
		if (total_diff >= QUAT_COMPARE_THRESHOLD)
			bitField |= genMask(i);
	}
	return bitField;
}

//Returns a vector representing the difference between the ith joint
//and it's parent, ie the ith "bone"
irr::core::vector3df *QuatFrame::getBone(int i)
{
	if (i == JointType_SpineMid)
	{
		bones[JointType_SpineMid]->set(*joints[JointType_SpineMid]);
		return new irr::core::vector3df(*bones[JointType_SpineMid]);
	}
	//returning trhe existing bone if it has already been
	//initialized is faster but makes reinitializing problematic
	//keep around a reInit bool?
	if (bones[i]->X != 0 || bones[i]->Y != 0 || bones[i]->Z != 0)
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
		return temp;
	}
	else
	{
		//we weren't ready, not sure if this case will ever get hit
		return new irr::core::vector3df();
	}
	return new irr::core::vector3df(*bones[i]);
}

//Initializes the QuatFrame, deriving all needed information from
//the information we have
void QuatFrame::init()
{
	irr::core::matrix4 mat = irr::core::matrix4();

	irr::core::vector3df ourBone = irr::core::vector3df();
	irr::core::vector3df parentBone = irr::core::vector3df();

	for (int i = 0; i < JointType_Count; i++)
		//TODO this is messy as hell, fix
		//easy way out: special initBone function
		//that works basically the same as getBone
		//but doesn't return anything and has 0
		//net heap change
		//but there must be a better way
		delete getBone(i);

	for (int i = currQuatCount; i < JointType_Count; i++)
	{
		irr::core::quaternion quat;

		ourBone.set(*bones[i]);
		ourBone.normalize();

		if (getParent(i) == i)
		{
			parentBone.set(ourBone);
			ourBone.set(irr::core::vector3df(0, -1, 0));
			ourBone.normalize();
		}
		else
			parentBone.set(*bones[getParent(i)]);
		
		parentBone.normalize();

		mat.buildRotateFromTo(ourBone, parentBone);

		quat = irr::core::quaternion(mat);
		quat.normalize();
		jointQuats[i]->set(quat);
	}
}

//set the timestamp for this QuatFrame
void QuatFrame::setTimestamp(double ts) {
	timestamp = ts;
}

//gets the timestamp for this QuatFrame
double QuatFrame::getTimestamp() {
	return timestamp;
}

//writes the contents of this QuatFrame as a JSON struct
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

//Destructor method, deallocates arrays and call destructors of contents
QuatFrame::~QuatFrame()
{
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		delete jointQuats[i];
		delete joints[i];
		delete bones[i];
	}
	delete[] jointQuats;
	delete[] bones;
	delete[] joints;
}