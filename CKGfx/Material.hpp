//
//  Material.hpp
//
//  Created by Samir Sinha on 9/25/15.
//
//

#ifndef CK_Graphics_Material_hpp
#define CK_Graphics_Material_hpp

#include "GfxTypes.hpp"

namespace cinek {
    namespace gfx {
    
struct Material
{
    TextureHandle diffuseTex;
    
    Color4 diffuseColor;
    
    Color4 specularColor;           // unused at the momemtn
    float specularPower;
    float specularIntensity;
};
    
    }   // namespace gfx
}   // namespace cinek

#endif
