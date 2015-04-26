//
//  VertexTypes.cpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "VertexTypes.hpp"

namespace cinek {
    namespace gfx {
    
VertexDefintions createVertexDefinitions()
{
    VertexDefintions defs;
    
    defs.reserve(8);
    
    // struct VertexTypes::PosColor
    defs.emplace_back();
    defs.back().begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    
    return std::move(defs);
}
    
    }   // namespace gfx
}   // namespace cinek

