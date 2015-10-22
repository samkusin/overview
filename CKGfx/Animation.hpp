//
//  Animation.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/22/15.
//
//

#ifndef CK_Graphics_Animation_hpp
#define CK_Graphics_Animation_hpp

#include "GfxTypes.hpp"

namespace cinek {
    namespace gfx {
    
struct Animation
{
    TextureHandle diffuseTex;
    
    Color4 diffuseColor;
    
    Color4 specularColor;
    float specularPower;
};
    
    }   // namespace gfx
}   // namespace cinek

#endif /* Animation_hpp */
