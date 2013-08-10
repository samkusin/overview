/*
 *  Main.c
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Game.hpp"

#include "SDL2/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

void OverviewSDLMain(SDL_Renderer* renderer)
{
    cinekine::ovengine::Game* game = OVEngine_CreateGame();

    bool active = true;
    while (active)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                active = false;
                continue;
            }
        }
        
        game->update();

        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }
    
    delete game;
}

#ifdef __cplusplus
}
#endif
