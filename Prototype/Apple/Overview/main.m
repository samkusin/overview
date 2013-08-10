//
//  main.m
//  Overview
//
//  Created by Samir Sinha on 6/30/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDL2/SDL.h"
#include <Cocoa/Cocoa.h>

//  The main loop for an Overview SDL based application
extern void OverviewSDLMain(SDL_Renderer* renderer);


int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        NSLog(@"SDL Failed to initialize : %s", SDL_GetError());
        return 1;
    }
    
    SDL_Window *win;
    win = SDL_CreateWindow("Overview Engine",
                           SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                           1024, 768,
                           SDL_WINDOW_SHOWN);
    if (win)
    {
        SDL_Renderer* renderer;
        renderer = SDL_CreateRenderer(win, -1,
                                      SDL_RENDERER_ACCELERATED |
                                      SDL_RENDERER_PRESENTVSYNC);
        
        OverviewSDLMain(renderer);
        
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
    }
    else
    {
        NSLog(@"SDL Failed to initialize Window.");
        return 1;
    }
   
    SDL_Quit();
    
    return 0;
}
