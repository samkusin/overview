//
//  main.m
//  Overview
//
//  Created by Samir Sinha on 6/30/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include <Cocoa/Cocoa.h>
#include <SDL2/SDL.h>

//  The main loop for an Overview SDL based application
extern int OverviewMain(SDL_Window* window, int argc, char* argv[]);

int main(int argc, char *argv[])
{
    int result = 0;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) >= 0)
    {
        SDL_Window* window = SDL_CreateWindow("Overview Engine",
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              1280, 720,
                                              SDL_WINDOW_SHOWN);
        if (window)
        {
            result = OverviewMain(window, argc, argv);
        }
        else
        {
            NSLog(@"SDL Failed to initialize window : %s", SDL_GetError());
            result = 1;
        }
        
        SDL_Quit();
    }
    else
    {
        NSLog(@"SDL Failed to initialize : %s", SDL_GetError());
    }
    return result;
}
