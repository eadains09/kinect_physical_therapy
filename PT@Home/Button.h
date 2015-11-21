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

//Button constants
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 100;

enum ButtonSprite
{
    BUTTON_SPRITE_PLAY = 0,
    BUTTON_SPRITE_RECORD = 1,
    BUTTON_SPRITE_STOP = 2,
    BUTTON_SPRITE_BACK = 3,
    BUTTON_SPRITE_TOTAL = 4
};

class Button
{
public:
    //Initializes internal variables
   // Button();
    
    Button(ButtonSprite type, int x, int y, std::string image);

    void setType(ButtonSprite type);
    
    //Sets top left position
    void setPosition( int x, int y );
        
    //Handles mouse event
    void handleEvent( SDL_Event* e );
    
    //Shows button sprite
    //void render(LTexture gButtonSpriteSheetTexture, SDL_Renderer* renderer, SDL_Rect* gSpriteClips);
    void render(SDL_Renderer* renderer);
    
    void freeButton();
    
private:
    //Top left position
    SDL_Point mPosition;
    SDL_Rect srcR;
    SDL_Rect destR;
    SDL_Surface* buttonSurface;
    SDL_Texture* buttonTexture;
    
    //Button type sprite
    ButtonSprite mCurrentSprite;
};


#endif /* defined(__Button_H__) */