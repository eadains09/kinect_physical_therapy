//
//  Display.h
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef Display_h
#define Display_h
//TODO change other ifdefs to drop PlayBodyPoints name

#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include "Movement.h"

using namespace std;


class Display {

private:
	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	SDL_Window* window = NULL;  //The window we'll be rendering to
	SDL_Renderer* renderer = NULL;
	SDL_Point bodyPoints[JOINT_TOTAL];
	Movement currMove;

	bool init();  //Starts up SDL and creates window
	bool loadMedia();
	bool getFramesFromFile(string filename);
	void close();  //Frees media and shuts down SDL

public:
	bool renderFrame(BodyFrame currFrame);
	bool run();

};


#endif