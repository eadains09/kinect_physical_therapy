//
//  Display.cpp
//
//  Created by Erika Dains on 10/23/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "Display.h"
#include "stdafx.h"

bool Display::run() {
	HRESULT hr;

    if (!init()) {
        printf("Failed to initialize!\n");
        return false;
    }
    
    //Load media
    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return false;
    }

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


		if (!m_pBodyFrameReader)
		{
			return false;
		}

		IBodyFrame* pBodyFrame = NULL;

		if (!SUCCEEDED(m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame)))
			continue;

		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}
		//TODO checkk to make sure at least one body is present

		BodyFrame *anorexia = new BodyFrame();

		Joint *joints = new Joint[JointType_Count];
		for (int j = 0; j < _countof(ppBodies); j++)
		{
			ppBodies[j]->GetJoints(JointType_Count, joints);

			for (int i = 0; i < JointType_Count; i++)
				anorexia->addJoint(*(new eJoint(i, joints[i].Position.X, joints[i].Position.Y)));


        //BodyFrame *frames = currMove.getFrames();
        //for (int i = 0; i < currMove.getCurrFrameCount(); i++) {
        	renderFrame(*anorexia);

            //Wait briefly
            SDL_Delay(50);
			std::cout << "Rendering points " << endl;// << i << endl;
        }

        // Alternatively:
        // getFramesFromFile("whereData.dat");
        // If we do this, could pass in the file string to run,
        // and have another method that just calls renderFrame on frames as they arrive for live playback

    }
    
    //Free resources and close SDL
    close();
    
    return true;
}

bool Display::renderFrame(BodyFrame currFrame) {

	eJoint *joints = currFrame.getJoints();

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);

    for (int i = 0; i < currFrame.getCurrJointCount(); i++) {
//      cout << joints[j].getX() << " " << joints[j].getY() << endl;
    	JointType parent = joints[i].getParent();

        if (parent != joints[i].getType()) {
            SDL_RenderDrawLine(renderer, joints[i].getX(), joints[i].getY(), joints[parent].getX(), joints[parent].getY());
            // if (joints[j].getType() == HEAD) {
            //     SDL_RenderDrawPoint(renderer, joints[j].getX(), joints[j].getY());
            // }
        }
    }

    SDL_RenderPresent(renderer);

    return true;
}

bool Display::getFramesFromFile(string filename) {
    currMove.readPoints(filename);

    BodyFrame *frames = currMove.getFrames();
    for (int i = 0; i < currMove.getCurrFrameCount(); i++) {
        renderFrame(frames[i]);

        //Wait briefly
        SDL_Delay(50);
        cout << "Rendering points " << i << endl;
    }

    return true;

}


bool Display::init() {
    bool success = true;
	HRESULT hr;
	IBodyFrameSource* pBodyFrameSource = NULL;

	GetDefaultKinectSensor(&m_pKinectSensor);
	if (!m_pKinectSensor || 
		!SUCCEEDED(m_pKinectSensor->Open()) || 
		!SUCCEEDED(m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper)) || 
		!SUCCEEDED(m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource)) ||
		!SUCCEEDED(pBodyFrameSource->OpenReader(&m_pBodyFrameReader)))
	{
		printf("Could not find a connected kinect\n");
		return false;
	}
	SafeRelease(pBodyFrameSource);


    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow("Kinect Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    
    return success;
}

bool Display::loadMedia() {
    bool success = true;
    
    currMove.readPoints("whereData.dat");
    
    return success;
}

void Display::close() {
    //This may not be necessary:
    currMove.freeFrames();
    
    SDL_DestroyRenderer(renderer);
    
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
    
}