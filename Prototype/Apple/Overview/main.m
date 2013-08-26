//
//  main.m
//  Overview
//
//  Created by Samir Sinha on 6/30/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include <Cocoa/Cocoa.h>
#include "SDL.h"

//  The main loop for an Overview SDL based application
extern int OverviewSDLMain(int argc, char* argv[]);


int main(int argc, char *argv[])
{
    int result = 0;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) >= 0)
    {
        result = OverviewSDLMain(argc, argv);
        SDL_Quit();
    }
    else
    {
        NSLog(@"SDL Failed to initialize : %s", SDL_GetError());
    }
    return result;
}
