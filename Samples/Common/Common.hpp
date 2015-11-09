//
//  Common.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#ifndef Common_hpp
#define Common_hpp

#include "CKGfx/GfxTypes.hpp"

enum
{
    kPollSDLEvent_Quit = 0x0001
};

struct PollStateSDL
{
    int mx, my;
    int mdx, mdy;
    int mxWheel, myWheel;
    uint32_t mbtn;
    
    //  key mappings
    const uint8_t* keystate;
    int keystateArraySize;
};

extern uint32_t pollSDLEvents(PollStateSDL& state);

extern int runSample(int viewWidth, int viewHeight);

#endif /* Common_hpp */
