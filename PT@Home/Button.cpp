//
//  Button.cpp
//
//  Created by Erika Dains on 11/8/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "Button.h"
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "Display.h"


/*Button::Button()
{
    mCurrentSprite = BUTTON_SPRITE_PLAY;
    
    srcR.x = 0;
    srcR.y = 0;
    srcR.w = BUTTON_WIDTH;
    srcR.h = BUTTON_HEIGHT;
    
    destR.x = 0;
    destR.y = 0;
    destR.w = BUTTON_WIDTH;
    destR.h = BUTTON_HEIGHT;
    
    buttonSurface = NULL;

}
*/
Button::Button(ButtonSprite type, int x, int y, std::string image) {
	mCurrentSprite = type;
    
    srcR.x = 0;
    srcR.y = 0;
    srcR.w = BUTTON_WIDTH;
    srcR.h = BUTTON_HEIGHT;
    
    destR.x = x;
    destR.y = y;
    destR.w = BUTTON_WIDTH;
    destR.h = BUTTON_HEIGHT;
    
    buttonSurface = SDL_LoadBMP(image.c_str());
}

Button::Button(ButtonSprite type, int w, int h, int x, int y, std::string image) {
    mCurrentSprite = type;
    
    srcR.x = 0;
    srcR.y = 0;
    srcR.w = w;
    srcR.h = h;
    
    destR.x = x;
    destR.y = y;
    destR.w = w;
    destR.h = h;
    
    buttonSurface = SDL_LoadBMP(image.c_str());
}

void Button::setType(ButtonSprite type)
{
    mCurrentSprite = type;
}

ButtonSprite Button::getType() {
	return mCurrentSprite;
}

void Button::setPosition( int x, int y )
{
    destR.x = x;
    destR.y = y;
}

bool Button::isInside(SDL_Event *e) {
    //Get mouse position
    int x, y;
    SDL_GetMouseState( &x, &y );
        
    //Check if mouse is in button
    bool inside = true;
        
    //Mouse is left of the button
    //if( x < mPosition.x )
    if (x < destR.x)
    {
        inside = false;
    }
    //Mouse is right of the button
    //else if( x > mPosition.x + BUTTON_WIDTH )
    else if (x > destR.x + destR.w)
    {
        inside = false;
    }
    //Mouse above the button
    //else if( y < mPosition.y )
    else if (y < destR.y)
    {
        inside = false;
    }
    //Mouse below the button
    //else if( y > mPosition.y + BUTTON_HEIGHT )
    else if (y > destR.y + destR.h)
    {
        inside = false;
    }

    return inside;
}

void Button::render(SDL_Renderer* renderer) {
    buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_RenderCopy(renderer, buttonTexture, NULL, &destR);
	SDL_DestroyTexture(buttonTexture);
}

void Button::freeButton() {
	//if (buttonSurface != NULL) {
		SDL_FreeSurface(buttonSurface);
	//}
    SDL_DestroyTexture(buttonTexture);
}