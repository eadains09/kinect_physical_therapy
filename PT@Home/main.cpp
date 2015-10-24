//
//  main.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

<<<<<<< HEAD
#include "stdafx.h"
#include <iostream>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include "Movement.h"
=======
#include "Display.h"
>>>>>>> origin/master

using namespace std;


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	main(NULL, 0);

}


int main(int argc, char* args[]) {
    Display currDisplay;
    
<<<<<<< HEAD
    if (!init()) {
        printf("Failed to initialize!\n");
    } else {
        //Load media
        if (!loadMedia()) {
            printf("Failed to load media!\n");
        } else {
            //Main loop flag
            bool quit = false;
            //Event Handler
            SDL_Event e;

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                BodyFrame *frames = testMove.getFrames();
                for (int i = 0; i < testMove.getCurrFrameCount(); i++) {
                    eJoint *joints = frames[i].getJoints();
//                    for (int j = 0; j < frames[i].getCurrJointCount(); j++) {
////                        cout << joints[j].getX() << " " << joints[j].getY() << endl;
//                        bodyPoints[j] = *joints[j].getSDLPoint();
//                    }
    
                    //Clear screen
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(renderer);
                    
                    //Render points array
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
                    SDL_RenderDrawLines(renderer, bodyPoints, JOINT_TOTAL);

                    for (int j = 0; j < frames[i].getCurrJointCount(); j++) {
//                        cout << joints[j].getX() << " " << joints[j].getY() << endl;
//                        bodyPoints[j] = *joints[j].getSDLPoint();
                        if (joints[j].getParent() != JOINT_DEFAULT) {
                            SDL_RenderDrawLine(renderer, joints[j].getX(), joints[j].getY(), joints[joints[j].getParent()].getX(), joints[joints[j].getParent()].getY());
                            if (joints[j].getType() == HEAD) {
                                SDL_RenderDrawPoint(renderer, joints[j].getX(), joints[j].getY());
                            }
                        }
                    }
                    SDL_RenderPresent(renderer);
                    //Wait half of a second
                    SDL_Delay(50);
                    cout << "Rendering points " << i << endl;
                }
//                quit = true;
=======
    currDisplay.run();
>>>>>>> origin/master

    
    return 0;
}