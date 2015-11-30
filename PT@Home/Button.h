//
//  Button.h
//
//  Created by Erika Dains on 11/8/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#ifndef __Button_H__
#define __Button_H__

#include <stdio.h>
#include <SDL.h>
#include <string>
#include <fstream>
#include <iostream>

//Button constants
const int BUTTON_WIDTH = 40;
const int BUTTON_HEIGHT = 40;

enum ButtonSprite
{
    BUTTON_SPRITE_PLAY = 0,
    BUTTON_SPRITE_RECORD = 1,
    BUTTON_SPRITE_PAUSE = 2,
    BUTTON_SPRITE_BACK = 3,
    BUTTON_SPRITE_PHYSICIAN = 4,
    BUTTON_SPRITE_PATIENT = 5,
    BUTTON_SPRITE_ADD = 6,
    BUTTON_SPRITE_DELETE = 7,
    BUTTON_SPRITE_SAVE = 8,
    BUTTON_SPRITE_KEYFRAME = 9,
    BUTTON_SPRITE_PLAYBACK = 10,
	BUTTON_SPRITE_FULLWORKOUT = 11,
    BUTTON_SPRITE_TOTAL = 12
};

class Button
{
public:
    //Initializes internal variables
    // Button();
    
    Button(ButtonSprite type, int x, int y, std::string image);
    Button(ButtonSprite type, int w, int h, int x, int y, std::string image);

    void setType(ButtonSprite type);
    ButtonSprite getType();

    //Sets top left position
    void setPosition( int x, int y );
    void render(SDL_Renderer* renderer);
	bool isInside(SDL_Event *e);

    void freeButton();
    
private:
    //Top left position
    SDL_Point mPosition;
    SDL_Rect srcR;
    SDL_Rect destR;
    SDL_Surface* buttonSurface;
    SDL_Texture* buttonTexture;

    std::ofstream log;

    
    //Button type sprite
    ButtonSprite mCurrentSprite;


};

#endif /* defined(__Button_H__) */
