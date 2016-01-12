//
//  Common.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#ifndef Overview_Common_hpp
#define Overview_Common_hpp

#include "CKGfx/GfxTypes.hpp"
#include <SDL2/SDL_Keycode.h>

enum
{
    kPollSDLEvent_Quit = 0x0001
};

namespace cinek {
    namespace ove {

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

    } /* namespace ove */
} /* namespace cinek */

extern uint32_t pollSDLEvents(cinek::ove::InputState& state);

extern int runSample(int viewWidth, int viewHeight);

#endif /* Common_hpp */
