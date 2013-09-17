//
//  FontLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "FontLibrary.hpp"
#include "Renderer.hpp"
#include "Engine/Debug.hpp"

namespace cinekine {
    namespace ovengine {
    
FontLibrary::FontLibrary(Renderer& renderer, size_t fontLimit) :
    _renderer(renderer),
    _fonts(std_allocator<Font>(_renderer.getAllocator()))
{
    //  TODO - make reserve size configurable
    //  NOTE, we're using this method to prepopulate the font library
    //  since Font objects have their copy constructor disabled.  Vector initialization
    //  requires that the Vector object have a copy constructor enabled, though we already
    //  have a move constructor for Font (hence why push_back works.
    //  
    //  Is this really the case?  Isn't there a better way to do this in C++11?
    _fonts.reserve(10);
    while (_fonts.size() < 10)
    {
        _fonts.push_back(Font());
    }
}
    
void FontLibrary::loadFont(FontHandle slot, const char* fontname)
{
    if (slot >= _fonts.size())
    {
        OVENGINE_LOG_ERROR("Failed to load slot [%u].  Font '%s' not found.",
                           slot, fontname);
        return;
    }
    //  clear out the slot first to release some resources.
    //
    unloadFont(slot);
    
}

void FontLibrary::unloadFont(FontHandle slot)
{
    if (slot >= _fonts.size())
        return;
    OVENGINE_LOG_INFO("Unloaded slot [%u].", slot);
    _fonts[slot] = Font();
}   
    
    }   // namespace ovengine
}   // namespace cinekine
