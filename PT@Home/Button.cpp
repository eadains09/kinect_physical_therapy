/*
 *  File:              Button.cpp
 *
 *  Purpose:           Class to give properties to any button being rendered on screen.
 *
 *  Member variables:  Button type, SDL surface and texture, default button size, 
 *                     button position in source (.bmp file) and destination (SDL_Renderer)
 *
 *  Created by Erika Dains on 11/8/15.
 */

#include "Button.h"
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "Display.h"


// Constructor - using default button size
/*  In vals:    type - what kind of button it is
 *              x - x position of button on displayed screen
 *              y - y positoin of button on displayed screen
 *              image - name of file where button image is stored
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

// Constructor - using specified button size
/*  In vals:    type - what kind of button it is
 *              w - width of button
 *              h - height of button
 *              x - x position of button on displayed screen
 *              y - y positoin of button on displayed screen
 *              image - name of file where button image is stored
 */
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

// Sets position of button in destination (SDL_Renderer)
// i.e. where the button will be seen on the displayed screen
void Button::setPosition( int x, int y )
{
    destR.x = x;
    destR.y = y;
}

/* Takes in an SDL_Event of MOUSEBUTTONDOWN, checks the position 
 * of the click against the position of the button on the screen.
 * Returns true if button has been clicked
 */
bool Button::isInside(SDL_Event *e) {
    //Get mouse position
    int x, y;
    SDL_GetMouseState( &x, &y );
        
    //Check if mouse is in button
    bool inside = true;
        
    //Mouse is left of the button
    if (x < destR.x)
    {
        inside = false;
    }
    //Mouse is right of the button
    else if (x > destR.x + destR.w)
    {
        inside = false;
    }
    //Mouse above the button
    else if (y < destR.y)
    {
        inside = false;
    }
    //Mouse below the button
    else if (y > destR.y + destR.h)
    {
        inside = false;
    }

    return inside;
}

// Renders the button on the specified renderer
// Once texture is copied over to renderer it must be deleted, 
// otherwise it will be a memory leak when function is called again
void Button::render(SDL_Renderer* renderer) {
    buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_RenderCopy(renderer, buttonTexture, NULL, &destR);
	SDL_DestroyTexture(buttonTexture);
}

// Deletes button's Surface and Texture, should any exist
void Button::freeButton() {
	SDL_FreeSurface(buttonSurface);
    SDL_DestroyTexture(buttonTexture);
}
