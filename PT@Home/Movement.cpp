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
	//so deques are supposed to deal with all the allocationy stuff on their own
	//so if we could transition to that I think it would be for the best
	qframes = new deque<QuatFrame>();
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
	std::ofstream log;
	log.open("testPlayback.txt", std::ofstream::app);

	//possible issue: the timestamp we get could be 0
	quotient = time / qframes->at(frameNumber + 1).getTimestamp();

	log << "frameNumber: " << frameNumber << " quotient: " << quotient << " time: " << time << " next frame timestamp: " << qframes->at(frameNumber + 1).getTimestamp() << endl;

	log.close();
	return qframes->at(frameNumber).slerp(QuatFrame(qframes->at(frameNumber+1)), quotient);
}



int Movement::getCurrFrameCount() {
    return currFrameCount;
}


Movement::~Movement()
{
	//I am ~82% sure the destructor for a deque call the destructor
	//method on all its members
	delete qframes;
}

void Movement::logFrames(std::string fileName)
{
    FileWriter file(fileName, "keyframes");

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

void Movement::pushBackFrame(QuatFrame *frame) {
//    qframes->push_back(*new QuatFrame(*frame));
	//will this non-leaky version work?
	qframes->push_back(QuatFrame(*frame));
	currFrameCount++;
}


QuatFrame* Movement::getBackFrame() {
    return new QuatFrame(qframes->back());
}


