//
//  Movement.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/13/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Movement.h"


using namespace std;

Movement::Movement() {
    currFrameCount = 0;
	//TODO(distant and not entirely necessary) it seems like it might
	//be nice to have a second constructor that takes frame_total as an
	//argument 
	frames = new BodyFrame*[FRAME_TOTAL];
	for (int i = 0; i < FRAME_TOTAL; i++)
		frames[i] = NULL;
}

//Change this to return a success boolean
void Movement::readPoints(std::string path) {
    ifstream jointFile;
    string currLine;
    string jointLine;
    string::size_type start, end;
    double xPos, yPos, zPos;
    char * pEnd;
    
    //Open file
    jointFile.open(path);
    
    if (jointFile.is_open()) {
        
        //Loop until end of file - at end will have a full array of frames, each with a full array of joint positions
        while (getline(jointFile, currLine) && currFrameCount < FRAME_TOTAL) {
            
            //Trim currLine of newline characters
            currLine.erase(currLine.find_last_not_of(" \n\r\t")+1);
            
            //Find starting place of bodyframe
            if (currLine.compare("[") == 0) {
                
                //Declare a bodyFrame, add to frames array
				BodyFrame *currFrame = NULL;// = new BodyFrame();
                
                //Read in each joint's points, looping until all 25 joints have been read in.
                for (int i = 0; i < JOINT_TOTAL; i++) {
                    
                    if (getline(jointFile, jointLine)) {
                    
                        //Trimming jointLine of all extra values so that it is ready for strtod
                        start = jointLine.find_first_not_of("[");
                        end = jointLine.find_last_of("]");
						//if this is an empty point
						//then the entire body frame
						//is invalid and we skip to the next one
						if (start == -1 || end == -1 ||start >= end-1)
							break;
						if (currFrame == NULL)
							currFrame = new BodyFrame();
                        jointLine = jointLine.substr(start, end-start);
                        jointLine.erase(jointLine.find(",", start), 1);
                        jointLine.erase(jointLine.find(",", start), 1);

                        //Parse string and convert to doubles
                        xPos = strtod(jointLine.c_str(), &pEnd);
                        yPos = strtod(pEnd, &pEnd);
                        zPos = strtod(pEnd, NULL);
                    
                        //Account for Kinect offset
                        transformPoints(&xPos, &yPos, &zPos);
                    
                        //Declare a joint using count for type and points and add to joints array of current bodyFrame
                        //eJoint currJoint(currFrame.getCurrJointCount(), (int)xPos, (int)yPos);
						//TODO here and elsewhere get rid of joint type(already encoded as array index)
						//and instead pass z=0, then proper x and y
						//irr::core::vector3df *currJoint = new irr::core::vector3df(currFrame->getCurrJointCount(), (int)xPos, (int)yPos);
						irr::core::vector3df *currJoint = new irr::core::vector3df((int)xPos, (int)yPos, 0);
                        currFrame->addJoint(currJoint);
                    }
                }
				if (currFrame != NULL)
	                frames[currFrameCount++] = currFrame;
            }
        }
    } else {
        cout << "Could not open file " << path << endl;
    }
    
    jointFile.close();
}

BodyFrame** Movement::getFrames() {
    return frames;
}

BodyFrame *Movement::getSingleFrame(int i) {
	if (i < currFrameCount) {
		return frames[i];
	}
	else {
		//returns last frame in array when called on int greater than number of frames (instead of returning null)
		return frames[currFrameCount-1];
	}
}

int Movement::getCurrFrameCount() {
    return currFrameCount;
}

//TODO: Should this be a destructor? Is it even necessary?
void Movement::freeFrames() {
    for (int i = 0; i < currFrameCount; i++) {
		delete frames[i];
    }
}

Movement::~Movement()
{
	freeFrames();
	delete[] frames;
}

void Movement::logMove(std::string fileName)
{
	ofstream jointFile;
	jointFile.open(fileName);
	//TODO
}

void Movement::transformPoints(double *xPos, double *yPos, double *zPos) {
    *xPos = (*xPos + 1) * 200;
    *yPos = (*yPos - 1) * -200;
    *zPos = (*zPos + 1) * 200;
}

