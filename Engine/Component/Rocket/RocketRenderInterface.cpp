/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 */

#include "RocketRenderInterface.hpp"
#include "Graphics/Renderer.hpp"

namespace cinekine {
    namespace ovengine {


RocketRenderInterface::RocketRenderInterface(glx::Renderer& renderer) :
    _renderer(renderer)
{

}

RocketRenderInterface::~RocketRenderInterface()
{

}

void RocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices,
                                           int num_vertices, int* indices, int num_indices,
                                           Rocket::Core::TextureHandle texture,
                                           const Rocket::Core::Vector2f& translation)
{

}

void RocketRenderInterface::EnableScissorRegion(bool enable)
{

}

void RocketRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{

}

bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle,
                                        Rocket::Core::Vector2i& texture_dimensions,
                                        const Rocket::Core::String& source)
{
    return true;
}

bool RocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
                                            const Rocket::Core::byte* source,
                                            const Rocket::Core::Vector2i& source_dimensions)
{
    return true;
}

void RocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture)
{

}

float RocketRenderInterface::GetHorizontalTexelOffset()
{
    return 0.0f;
}

float RocketRenderInterface::GetVerticalTexelOffset()
{
    return 0.0f;
}

float RocketRenderInterface::GetPixelsPerInch()
{
    return 100.0f;
}

void RocketRenderInterface::Release()
{

}

    }   // namespace ovengine
}   // namespace cinekine

