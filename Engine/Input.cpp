//
//  Input.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "Input.hpp"
#include "Debug.hpp"

namespace cinek {
    namespace ove {

bool InputState::testKey(uint8_t key) const
{
    CK_ASSERT_RETURN_VALUE(key < keystateArraySize, false);
    return keystate[key];
}

bool InputState::testKeyMod(int mod) const
{
    return (keyModifiers & mod) != 0;
}


    } /* namespace ove */
} /* namespace cinek */

