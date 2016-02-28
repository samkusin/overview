//
//  Input.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Overview_Input_hpp
#define Overview_Input_hpp

#include <SDL2/SDL_Keycode.h>

namespace cinek {
    namespace uicore {

    enum
    {
        kPollSDLEvent_Quit = 0x0001
    };

    struct InputState
    {
        int mx, my;
        int mdx, mdy;
        int mxWheel, myWheel;
        uint32_t mbtn;
        
        //  key mappings
        const uint8_t* keystate;
        int keystateArraySize;
        SDL_Keymod keyModifiers;
        
        bool testKey(uint8_t key) const;
        bool testKeyMod(int mod) const;
    };


    uint32_t pollSDLEvents(InputState& state);

    } /* namespace uicore */
} /* namespace cinek */



#endif /* Input_hpp */
