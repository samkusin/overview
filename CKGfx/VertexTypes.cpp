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
    static bgfx::VertexDecl s_decls[kFormatLimit];
    static uint32_t s_declCount = 0;
    
    void initialize()
    {
        s_declCount = 0;
        
        // struct VertexTypes::Pos
        s_decls[s_declCount].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
        ++s_declCount;
        
        // struct VertexTypes::PosColor
        s_decls[s_declCount].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        ++s_declCount;
    }
    
    const bgfx::VertexDecl& declaration(Format tex)
    {
        static bgfx::VertexDecl kNullDecl;
        if (tex >=s_declCount)
        {
            CK_ASSERT(false);
            return kNullDecl;
        }
        return s_decls[tex];
    }
}
    
    }   // namespace gfx
}   // namespace cinek

