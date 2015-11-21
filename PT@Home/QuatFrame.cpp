#include "QuatFrame.h"
#include "Display.h"



QuatFrame::QuatFrame()
{
	joints = new irr::core::vector3df*[JOINT_TOTAL];
	jointQuats = new irr::core::quaternion*[JOINT_TOTAL];

	currJointCount = 0;

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		jointQuats[i] = NULL;
		joints[i] = NULL;
	}
}

//temporary conveneince method
QuatFrame::QuatFrame(BodyFrame base)
{
	joints = new irr::core::vector3df*[JOINT_TOTAL];
	jointQuats = new irr::core::quaternion*[JOINT_TOTAL];
	currJointCount = 0;
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		eJoint point = base.getJoints()[i];
		this->addJoint(point.getX(), point.getY(), point.getZ());
	}
}

bool QuatFrame::addJoint(float x, float y, float z)
{
	joints[currJointCount++] = new irr::core::vector3df(x, y, z);
	if (isReady())
		init();
	else
		return false;
	return true;
}

bool QuatFrame::isReady()
{
	return (currJointCount == JOINT_TOTAL);
}

void QuatFrame::initBodyFrame(BodyFrame * frame)
{
	if (!isReady())
	{
		frame = NULL;
		return;
	}

	//This hurts to do, but here we go anyways, we flush the 
	//existing points, then build them back up
	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		if (i != JointType_SpineMid && joints[i] != NULL)
		{
			delete joints[i];
			joints[i] = NULL;
		}
	}

	for (int i = 0; i < JOINT_TOTAL; i++)
	{
		if (joints[i] != NULL)
		{
			joints[i] = getPoint(i);
		}
	}

}

irr::core::vector3df *QuatFrame::getPoint(int i)
{
	if (i == JointType_SpineMid)
	{
		return joints[JointType_SpineMid];
	}
//	new irr::core::quaternion(getPoint(getParent(i))->X, getPoint(getParent(i))->Y, getPoint(getParent(i))->Z);
//	joints[i] = *jointQuats[i] * *(new irr::core::quaternion(getPoint(getParent(i))->X, getPoint(getParent(i))->Y, getPoint(getParent(i))->Z));
//	irr::core::matrix4 inv;
//	(&jointQuats[i]->getMatrix())->getInverse(inv);
//	(jointQuats[i]->getMatrix())*(inv);

//	jointQuats[i] * *joints[getParent(i)];
	irr::core::quaternion *inv = new irr::core::quaternion(*jointQuats[i]);
	inv->makeInverse();
	irr::core::vector3df *temp = getPoint(getParent(i));
	irr::core::quaternion *fake = new irr::core::quaternion(temp->X, temp->Y, temp->Z);

//	(*jointQuats[i]) * (*getPoint(getParent(i)));// *(*inv);
	fake = &((*jointQuats[i]) * (*fake) *(*inv));
	temp->X = fake->X;
	temp->Y = fake->Y;
	temp->Z = fake->Z;
	delete fake;

		joints[i] = temp;
		//getPoint(getParent(i))->invert();
	//pretty sure this assumes quaternion and vector have been normalized
	//which may not be true
	//return (jointQuats[i])*(*getPoint(getParent(i))).invert();
	return joints[i];
}



void QuatFrame::init()
{
	for (int i = 0; i < JointType_Count; i++)
	{
		irr::core::quaternion *quat;
		if (getParent(i) == i)
		{
			//mid_spine is at 0 for now
			quat = new irr::core::quaternion(0, 0, 0, 0);
		}
		else
		{
			irr::core::matrix4 *mat = new irr::core::matrix4();
			//it would be really nice if buildRotateFromTo worked.
			//but I get the distinct feeling this is too easy
			mat->buildRotateFromTo(*joints[i], *joints[getParent(i)]);
			quat = new irr::core::quaternion(*mat);
			//this alternative makes slightly more sense
				//but does not account for gimbal lock
				//which is a bad thing
			//float d_x = joints[i]->X - joints[getParent(i)]->X;
			//float d_y = joints[i]->Y - joints[getParent(i)]->Y;
			//float d_z = joints[i]->Z - joints[getParent(i)]->Z;
			//float yaw = atan2(d_x, d_z) *180.0 / 3.141592653;
			//float padj = sqrt(pow(d_x, 2) + pow(d_z, 2));
			//float pitch = atan2(padj, d_y) *180.0 / 3.141592653;

			//quat = new irr::core::quaternion(0, pitch, yaw);
			//normalize is this easy?
			quat->normalize();
			jointQuats[i] = quat;
		}
	}


}

QuatFrame::~QuatFrame()
{
	if (isReady())
		for (int i = 0; i < JOINT_TOTAL; i++)
			delete jointQuats[i];

		
	for (int i = 0; i < currJointCount; i++)
		delete joints[i];

	delete[] jointQuats;
	delete[] joints;
}