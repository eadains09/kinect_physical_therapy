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
	//TODO(distant and not entirely necessary) it seems like it might
	//be nice to have a second constructor that takes frame_total as an
	//argument 
	//frames = new BodyFrame*[FRAME_TOTAL];
	frames = new deque<BodyFrame>(FRAME_TOTAL);
	deque<BodyFrame> qframes(FRAME_TOTAL);
	//for (int i = 0; i < FRAME_TOTAL; i++)
//		frames[i] = NULL;
}

//TODO Change this to return a success boolean?
/*void Movement::readPoints(std::string path) {
    //ifstream jointFile;
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
                        //transformPoints(&xPos, &yPos, &zPos);
                    
                        //Declare a joint using count for type and points and add to joints array of current bodyFrame
                        //eJoint currJoint(currFrame.getCurrJointCount(), (int)xPos, (int)yPos);
						//TODO here and elsewhere get rid of joint type(already encoded as array index)
						//and instead pass z=0, then proper x and y
						//irr::core::vector3df *currJoint = new irr::core::vector3df(currFrame->getCurrJointCount(), (int)xPos, (int)yPos);
						irr::core::vector3df *currJoint = new irr::core::vector3df((int)xPos, (int)yPos, 0);
                        currFrame->addJoint(currJoint);
                    }
                }
<<<<<<< HEAD
				if (currFrame != NULL)
	                frames[currFrameCount++] = currFrame;
=======
                //frames[currFrameCount] = currFrame;
                frames.push_back(currFrame);
                currFrameCount++;
>>>>>>> master
            }
        }
    } else {
        cout << "Could not open file " << path << endl;
    }
    
    jointFile.close();

}*/

void Movement::readQuatFrame(std::string path) {
    FileReader file(path);
    
    if(file.isOpen()) {
        file.findFileStart();





       file.findJointStart();

        while (file.findKeyframeStart()) {
            QuatFrame currFrame;



            while (file.findJointStart()) {
                // Just change this part to switch to reading quaternions
                irr::core::vector3df currJoint = readJointPoints(&file);
                //eJoint currJoint = readJointQuats(&file, currFrame);
				if (currJoint.X == 0 && currJoint.Y == 0 && currJoint.Z == 0) {
					continue;
				}
				currFrame.addJoint(currJoint);
				
            }
			if (!currFrame.isReady()) {
				continue;
			}
			else {
				qframes.push_back(currFrame);
				currFrameCount++;
			}
        }
    }

    file.closeFile();
}

deque<BodyFrame> Movement::getFrames() {
    return *frames;
}

//TODO check w/ erika
void Movement::readKeyframes(std::string path) {
    FileReader file(path);
    
    if(file.isOpen()) {
        file.findFileStart();

        while (file.findKeyframeStart()) {
            QuatFrame currFrame;
			//TODO
			//FOR SURE MAKE SURE OF ORDERING of frame struct
			//as stored in file
            currFrame.setTimestamp(file.findTimestamp());

			file.findJointStart();
			//I think I might need one more findJointStart() here
			//ask erika?
			currFrame.addMidSpine(readJointPoints(&file));

            file.findJointStart();
            while (file.findJointStart())
				currFrame.addQuatJoint(readJointQuat(&file));

            qframes.push_back(currFrame);
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

	//transformPoints(&xPos, &yPos, &zPos);
//    eJoint currJoint(currFrame.getCurrJointCount(), (int)xPos, (int)yPos, (int)zPos);
	irr::core::vector3df currJoint(xPos, yPos, zPos);

    return currJoint;
}

irr::core::quaternion Movement::readJointQuat(FileReader *file) {
    double xQuat, yQuat, zQuat, wQuat;

    xQuat = (*file).findDouble();
    yQuat = (*file).findDouble();
    zQuat = (*file).findDouble();
    wQuat = (*file).findDouble();

	irr::core::quaternion currJoint(xQuat, yQuat, zQuat, wQuat);

    return currJoint;
}

//so here maybe is where we do the slerping?
BodyFrame Movement::getSingleFrame(int i) {

	if (i < currFrameCount) {
        return frames->at(i);
	}
	else {
		//returns last frame in array when called on int greater than number of frames (instead of returning null)
        return frames->at(currFrameCount-1);
	}
}

int Movement::getCurrFrameCount() {
    return currFrameCount;
}


Movement::~Movement()
{
//	freeFrames();
	delete frames;
}

void Movement::logMove(std::string fileName)
{
    while (frames->size() > 0) {
		//TODO I think there's a fancy deque foreach destructor
		//that one is supposed to use, look up and implement
        //frames->front().freeJoints();
        frames->pop_front();
    }
}

/*neat fact to bear in mind: we will never, under any circumstances,
need to log raw points*/
//maybe here after constructing file before if we do a check to see
//if we have qframes, and if not we go ahead and convert frames into qframes
//the basic idea being that we continue making movement an opaque layer that deals with
//quatframe/bodyframe stuff
//idk
void Movement::logFrames(std::string fileName)
{
    FileWriter file(fileName, "keyframes");

    if (qframes.size() > 0) {
		qframes.front().writeFrame(&file);
		qframes.pop_front();
        while (frames->size() > 0) {
            file.addComma();
            qframes.front().writeFrame(&file);
			qframes.pop_front();
        }
    }

    file.closeFile();
}

void Movement::popBackFrame() {
    frames->pop_back();
    currFrameCount--;
}

void Movement::pushBackFrame(BodyFrame frame) {
    frames->push_back(frame);
    currFrameCount++;
}

BodyFrame Movement::getBackFrame() {
    return frames->back();
}

void Movement::transformPoints(double *xPos, double *yPos, double *zPos) {
    *xPos = (*xPos + 1) * 200;
    *yPos = (*yPos - 1) * -200;
    *zPos = (*zPos + 1) * 200;
}

