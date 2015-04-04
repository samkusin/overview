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

#define OVENGINE_OSX_OPENGL_3_2_PROFILE 1


int main(int argc, char *argv[])
{
    int result = 0;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) >= 0)
    {
        // Request an OpenGL 3.2 context for maximum non-fixed pipeline OSX compatibility (10.7+)
        // This must occur before SDL window creation so that OpenGL is properly initialized with
        // our desired core profile.
        //
    #if OVENGINE_OSX_OPENGL_3_2_PROFILE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    #endif
        SDL_Window* window = SDL_CreateWindow("Overview Engine",
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              1280, 720,
                                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (window)
        {
            //result = OverviewSDLMain(window, argc, argv);
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
