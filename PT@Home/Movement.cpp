//
//  Movement.cpp
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Movement.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "QuatFrame.h"


using namespace std;

Movement::Movement() {
    currFrameCount = 0;
	finished = false;
	qframes = new deque<QuatFrame>();
}

void Movement::readQuatFrame(std::string path) {
    FileReader file(path);
    
    if(file.isOpen()) {
        file.findFileStart();

       file.findJointStart();

        while (file.findKeyframeStart()) {
            QuatFrame currFrame;
			file.findJointStart();

            while (file.findJointStart()) {
                irr::core::vector3df currJoint = readJointPoints(&file);

				if (currJoint.X == 0 && currJoint.Y == 0 && currJoint.Z == 0) {//if readJointPoints returned a bogus point
					continue;
				}
				currFrame.addJoint(currJoint);
				
            }
			if (!currFrame.isReady()) {
				continue;
			}
			else {
				qframes->push_back(currFrame);
				currFrameCount++;
			}
        }
    }

    file.closeFile();
}

deque<QuatFrame> Movement::getFrames() {
    return *qframes;
}

//I have had an idea that I think might be useful:
//use the deque allocator and modify the values of
//the instance the deque allocator created instead 
//of trying to allocate then push into the deque
void Movement::readKeyframes(std::string path) {
    FileReader file(path);
    
    if(file.isOpen()) {
        file.findFileStart();

        while (file.findKeyframeStart()) {
            QuatFrame *currFrame = new QuatFrame();
			
            currFrame->setTimestamp(file.findTimestamp());

			file.findJointStart();
			currFrame->addMidSpine(readJointPoints(&file));

            file.findJointStart();
            while (file.findJointStart())
				currFrame->addQuatJoint(readJointQuat(&file));

            qframes->push_back(*currFrame);
            currFrameCount++;
        }
    }

    file.closeFile();
}

irr::core::vector3df Movement::readJointPoints(FileReader *file) {
    double xPos, yPos, zPos;

    xPos = (*file).findDouble();
    yPos = (*file).findDouble();
    zPos = (*file).findDouble();

	irr::core::vector3df *currJoint = new irr::core::vector3df(xPos, yPos, zPos);

    return *currJoint;
}

irr::core::quaternion Movement::readJointQuat(FileReader *file) {
    double xQuat, yQuat, zQuat, wQuat;

    xQuat = (*file).findDouble();
    yQuat = (*file).findDouble();
    zQuat = (*file).findDouble();
    wQuat = (*file).findDouble();

	irr::core::quaternion *currJoint = new irr::core::quaternion(xQuat, yQuat, zQuat, wQuat);

    return *currJoint;
}

//time is the total movement time that has transpired
//QuatFrame *Movement::getSingleFrame(double time)
//{
//	double sum = 0;
//	int i;
//
//	if (qframes->size() == 0)
//		return new QuatFrame();
//	if (qframes->size() == 1)
//	{
//		return new QuatFrame(qframes->at(0));
//	}
//	for (i = 0; i < qframes->size(); i++)
//	{
//		sum += qframes->at(i).getTimestamp();
//		if (sum >= time)
//			break;
//	}
//	if (i == qframes->size())//if we ran off the end
//		return new QuatFrame();
//	else if (i == 0)//if my picture of things is right enough, we should hit this case exactly once
//	{
//		return new QuatFrame(qframes->at(0));
//	}
//	else
//	{
//		double diff = sum - time;
//		diff = 1-diff/ qframes->at(i).getTimestamp();
//
//		return qframes->at(i - 1).slerp(QuatFrame(qframes->at(i)), diff);
//	}
//}

//time is the amount of time that has transpired while in
//a playing state since the current keyframe was displayed
//raw rather than being the initial state of the slerp
QuatFrame *Movement::getSingleFrame(int frameNumber, double time)
{
	double quotient;
	int i;

	if (qframes->size() == 0)
		return new QuatFrame();
	if (qframes->size() == 1)
	{
		return new QuatFrame(qframes->at(0));
	}
	if (frameNumber >= qframes->size()-1)//if we ran off the end
		return new QuatFrame();
	if (time >= qframes->at(frameNumber + 1).getTimestamp())//if we hit the end of a keyframe
		return new QuatFrame();

	//this has two possible issues, the timestamp we get could be 0
	//or the time argument we got passed could be larger than the timestamp
	//and while one of them is an error and the other a bug they are equally disastrous
	//TODO make sure they don't happen or protect against them
	quotient = time / qframes->at(frameNumber + 1).getTimestamp();

	QuatFrame *temp = qframes->at(frameNumber).slerp(QuatFrame(qframes->at(frameNumber+1)), quotient);
//	temp->setTimestamp(quotient);
	return temp;
}



int Movement::getCurrFrameCount() {
    return currFrameCount;
}

bool Movement::isFinished() {
	return finished;
}

void Movement::resetFinished() {
	finished = false;
}


Movement::~Movement()
{
	delete qframes;
//	delete frames;
}

/*neat fact to bear in mind: we will never, under any circumstances,
need to log raw points*/
void Movement::logFrames(std::string fileName)
{
    FileWriter file(fileName, "keyframes");
	/*
	while (frames->size() > 0)
	{
		//TODO change qframes and frames to deques of pointers
		//so we can destruct properly
		qframes->push_back(*new QuatFrame(frames->front()));
		frames->pop_front();
	}*/

    if (qframes->size() > 0) {
		qframes->front().writeFrame(&file);
		qframes->pop_front();
        while (qframes->size() > 0) {
            file.addComma();
            qframes->front().writeFrame(&file);
			qframes->pop_front();
        }
    }

    file.closeFile();
}

void Movement::popBackFrame() {
    qframes->pop_back();
	currFrameCount--;
}

//changing to BodyFrame * to avoid automatically calling destructor on frame
//at end of function
void Movement::pushBackFrame(QuatFrame *frame) {
    qframes->push_back(*new QuatFrame(*frame));
    currFrameCount++;
}

//Changing to return BodyFrame * to avoid automatic destructor on joints - where it gets returned calls transform points, meaning what will later be converted to quats are transformed points
QuatFrame* Movement::getBackFrame() {
    return new QuatFrame(qframes->back());
}
/*
void Movement::transformPoints(double *xPos, double *yPos, double *zPos) {
    *xPos = (*xPos + 1) * 200;
    *yPos = (*yPos - 1) * -200;
    *zPos = (*zPos + 1) * 200;
}*/

