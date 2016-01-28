//
//  VertexTypes.cpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "VertexTypes.hpp"

#if defined(_MSC_VER)
#include <cassert>
#endif
#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {
    
namespace VertexTypes
{
    static bgfx::VertexDecl s_nullDecl;
    static bgfx::VertexDecl s_decls[kFormatLimit];

#if !defined(_MSC_VER)
    static_assert(offsetof(Vector4, comp[1]) == offsetof(Vector4, y) &&
        offsetof(Vector4, comp[1]) == offsetof(Vector4, g),
        "ensure array and struct values are at the same offsets");

    static_assert(offsetof(Vector2, comp[1]) == offsetof(Vector2, y),
        "ensure array and struct values are at the same offsets");

    static_assert(offsetof(Vector3, comp[1]) == offsetof(Vector3, y) &&
        offsetof(Vector3, comp[1]) == offsetof(Vector3, g),
        "ensure array and struct values are at the same offsets");
#endif
    void initialize()
    {
    #if defined(_MSC_VER)
        //  getting around the fact that for some reason, offsetof isn't a constexpr even in
        //  the latest VS 2015.  our static-asserts are useless
        assert(offsetof(Vector4, comp[1]) == offsetof(Vector4, y) &&
            offsetof(Vector4, comp[1]) == offsetof(Vector4, g));
        assert(offsetof(Vector2, comp[1]) == offsetof(Vector2, y));
        assert(offsetof(Vector3, comp[1]) == offsetof(Vector3, y) &&
            offsetof(Vector3, comp[1]) == offsetof(Vector3, g));
    #endif
        s_nullDecl.begin().end();

        // kVTex0
        s_decls[kVTex0].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        // kVNormal_Tex0
        s_decls[kVNormal_Tex0].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        
        // kVNormal_Tex0_Weights
        s_decls[kVNormal_Tex0_Weights].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float)
            .end();
        
        // kVNormal_Tex0_Weights
        s_decls[kVNormal_Weights].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float)
            .end();
        
        //  kVPosition
        s_decls[kVPosition].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();

        //  kVPositionNormal
        s_decls[kVPositionNormal].begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
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

