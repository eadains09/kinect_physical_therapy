//
//  main.cpp
//  PlayBodyPoints
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include "Movement.h"

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;  //The window we'll be rendering to
SDL_Renderer* renderer = NULL;
SDL_Point bodyPoints[JOINT_TOTAL];
Movement testMove;

bool init();  //Starts up SDL and creates window
bool loadMedia();
void close();  //Frees media and shuts down SDL


int main(int argc, char* args[]) {
    
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
                    Joint *joints = frames[i].getJoints();
                    for (int j = 0; j < frames[i].getCurrJointCount(); j++) {
//                        cout << joints[j].getX() << " " << joints[j].getY() << endl;
                        bodyPoints[j] = *joints[j].getSDLPoint();
                    }
    
                    //Clear screen
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(renderer);
                    
                    //Render points array
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
                    SDL_RenderDrawLines(renderer, bodyPoints, JOINT_TOTAL);
                    SDL_RenderPresent(renderer);
                    //Wait half of a second
                    SDL_Delay(500);
                    cout << "Rendering points " << i << endl;
                }
//                quit = true;

            }
        }
    }
    
    //Free resources and close SDL
    close();
    
    return 0;
}

bool init() {
    bool success = true;
    
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

bool loadMedia() {
    bool success = true;
    
    testMove.readPoints("whereData.dat");
    
    return success;
}

void close() {
    //This may not be necessary:
    testMove.freeFrames();
    
    SDL_DestroyRenderer(renderer);
    
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
    
}