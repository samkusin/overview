//
//  main.m
//  Overview
//
//  Created by Samir Sinha on 6/30/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include <Cocoa/Cocoa.h>
#include "SDL2/SDL.h"

//  The main loop for an Overview SDL based application
extern int OverviewSDLMain(SDL_Window* window, int argc, char* argv[]);


int main(int argc, char *argv[])
{
    int result = 0;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) >= 0)
    {
        // Request an OpenGL 3.2 context for maximum non-fixed pipeline OSX compatibility (10.7+)
        // SDL will use our custom GL setup parameters when initializing our window, which is
        //
        /*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        */
        
        SDL_Window* window = SDL_CreateWindow("Overview Engine",
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              1024, 768,
                                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (window)
        {
            result = OverviewSDLMain(window, argc, argv);
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
