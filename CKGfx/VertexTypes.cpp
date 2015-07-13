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
    
    void initialize()
    {
        s_nullDecl.begin().end();
    
        // kVec3
        s_decls[kVec3].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
        
        // kVec3_RGBA
        s_decls[kVec3_RGBA].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        
        // kVec3_Normal
        s_decls[kVec3_Normal].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .end();
        
        // kVec3_Normal_RGBA
        s_decls[kVec3_Normal_RGBA].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        
        // kVec3_Normal_Tex0
        s_decls[kVec3_Normal_Tex0].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        
        s_decls[kVec3_RGBA_Tex0].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        
        //  allow custom presets starting at the preset limit index
        for (int i = kPresetCount; i < kFormatLimit; ++i)
        {
            s_decls[i].begin().end();
        }
    }
    
    const bgfx::VertexDecl& declaration(int tex)
    {
        if (tex >= kFormatLimit)
        {
            CK_ASSERT(false);
            return s_nullDecl;
        }
        return s_decls[tex];
    }
    
    bgfx::VertexDecl* createCustomPreset(int formatId)
    {
        if (formatId < 0 || formatId >= kFormatLimit)
        {
            CK_ASSERT(false);
            return nullptr;
        }
        return &s_decls[formatId];
    }
}
    
    }   // namespace gfx
}   // namespace cinek

