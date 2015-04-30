//
//  VertexTypes.cpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "VertexTypes.hpp"

#include <cinek/debug.hpp>

namespace cinek {
    namespace gfx {
    
    
namespace VertexTypes
{
    static bgfx::VertexDecl s_nullDecl;
    static bgfx::VertexDecl s_decls[kFormatLimit];
    static uint32_t s_declCount = 0;
    
    void initialize()
    {
        s_nullDecl.begin().end();
    
        s_declCount = 0;
        
        // kVec3
        s_decls[s_declCount].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
        ++s_declCount;
        
        // kVec3_RGBA
        s_decls[s_declCount].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        ++s_declCount;
        
        // kVec3_Normal
        s_decls[s_declCount].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .end();
        ++s_declCount;
    }
    
    const bgfx::VertexDecl& declaration(Format tex)
    {
        if (tex >=s_declCount)
        {
            CK_ASSERT(false);
            return s_nullDecl;
        }
        return s_decls[tex];
    }
}
    
    }   // namespace gfx
}   // namespace cinek

