//
//  Joints.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "BodyFrame.h"

BodyFrame::BodyFrame() {
    currJointCount = 0;
}

bool BodyFrame::addJoint(Joint currJoint) {
    bool success = false;
    
    if (currJointCount < JOINT_TOTAL) {
        joints[currJointCount] = currJoint;
        currJointCount++;
        success = true;
    }
    
    return success;
}

Joint* BodyFrame::sortJointsByParent() {
    Joint *sortedJoints[JOINT_TOTAL];
    
    return *sortedJoints;
}

Joint* BodyFrame::getJoints() {
    return joints;
}

//void storePoints() {
//    bodyPoints[0] = *const_cast<const SDL_Point *>(new SDL_Point({-2, -5}));
//    bodyPoints[1] = *const_cast<const SDL_Point *>(new SDL_Point({0, 13}));
//    bodyPoints[2] = *const_cast<const SDL_Point *>(new SDL_Point({0, 29}));
//    bodyPoints[3] = *const_cast<const SDL_Point *>(new SDL_Point({0, 42}));
//    bodyPoints[4] = *const_cast<const SDL_Point *>(new SDL_Point({-17, 18}));
//    bodyPoints[5] = *const_cast<const SDL_Point *>(new SDL_Point({-24, 4}));
//    bodyPoints[6] = *const_cast<const SDL_Point *>(new SDL_Point({-8, -3}));
//    bodyPoints[7] = *const_cast<const SDL_Point *>(new SDL_Point({-3, -7}));
//    bodyPoints[8] = *const_cast<const SDL_Point *>(new SDL_Point({16, 19}));
//    bodyPoints[9] = *const_cast<const SDL_Point *>(new SDL_Point({23, 5}));
//    bodyPoints[10] = *const_cast<const SDL_Point *>(new SDL_Point({16, 0}));
//    bodyPoints[11] = *const_cast<const SDL_Point *>(new SDL_Point({10, -5}));
//    bodyPoints[12] = *const_cast<const SDL_Point *>(new SDL_Point({-9, -4}));
//    bodyPoints[13] = *const_cast<const SDL_Point *>(new SDL_Point({2, -11}));
//    bodyPoints[14] = *const_cast<const SDL_Point *>(new SDL_Point({-13, 3}));
//    bodyPoints[15] = *const_cast<const SDL_Point *>(new SDL_Point({-9, -4}));
//    bodyPoints[16] = *const_cast<const SDL_Point *>(new SDL_Point({5, -4}));
//    bodyPoints[17] = *const_cast<const SDL_Point *>(new SDL_Point({-26, 8}));
//    bodyPoints[18] = *const_cast<const SDL_Point *>(new SDL_Point({0, -11}));
//    bodyPoints[19] = *const_cast<const SDL_Point *>(new SDL_Point({8, -8}));
//    bodyPoints[20] = *const_cast<const SDL_Point *>(new SDL_Point({0, 25}));
//    bodyPoints[21] = *const_cast<const SDL_Point *>(new SDL_Point({2, -9}));
//    bodyPoints[22] = *const_cast<const SDL_Point *>(new SDL_Point({-2, -5}));
//    bodyPoints[23] = *const_cast<const SDL_Point *>(new SDL_Point({8, -6}));
//    bodyPoints[24] = *const_cast<const SDL_Point *>(new SDL_Point({13, -2}));
//    
//    //    for (int i = 0; i < 25; i++) {
//    //        bodyPoints[i] = readNextLineOfFile();
//    //    }
//}
