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
#include "Graphics/RendererCLI.hpp"
#include "Engine/Debug.hpp"

#include "cinek/vector.hpp"

namespace cinekine {
    namespace ovengine {


RocketRenderInterface::RocketRenderInterface(glx::RendererCLI& renderer,
                                             Allocator& allocator) :
    _renderer(renderer),
    _textures(std_allocator<std::pair<const glx::Texture*, std::shared_ptr<glx::Texture>>>(allocator)),
    _meshes(std_allocator<std::pair<const glx::Mesh*, std::shared_ptr<glx::Mesh>>>(allocator))
{
}

RocketRenderInterface::~RocketRenderInterface()
{
}

glx::Rect RocketRenderInterface::getViewport() const
{
    return _renderer.getViewport();
}

void RocketRenderInterface::buildArrays(Rocket::Core::Vertex* pvertices, int num_vertices,
                                        int* pindices, int num_indices,
                                        vector<glm::vec2>& pos,
                                        vector<glm::vec2>& uvs,
                                        vector<glm::vec4>& colors,
                                        vector<uint16_t>& indices)
{
    pos.reserve(num_vertices);
    uvs.reserve(num_vertices);
    colors.reserve(num_vertices);
    indices.reserve(num_indices);

    for(int iv = 0; iv < num_vertices; ++iv)
    {
        Rocket::Core::Vertex* v = &pvertices[iv];
        pos.emplace_back(v->position.x, v->position.y);
        uvs.emplace_back(v->tex_coord.x, v->tex_coord.y);
        colors.emplace_back(v->colour.red/255.0f,
                            v->colour.green/255.0f,
                            v->colour.blue/255.0f,
                            v->colour.alpha/255.0f);
    }
    for(int i = 0; i < num_indices; ++i)
    {
        indices.push_back(pindices[i]);
    }
}

Rocket::Core::CompiledGeometryHandle RocketRenderInterface::CompileGeometry(
                                                        Rocket::Core::Vertex* vertices, int num_vertices,
                                                        int* indices, int num_indices,
                                                        Rocket::Core::TextureHandle texture)
{
    auto textureIt = _textures.find(reinterpret_cast<glx::Texture*>(texture));
    if (textureIt == _textures.end())
        return (Rocket::Core::CompiledGeometryHandle)0;

    vector<glm::vec2> pos;
    vector<glm::vec2> uvs;
    vector<glm::vec4> colors;
    vector<uint16_t> vindices;

    buildArrays(vertices, num_vertices,
                indices, num_indices,
                pos, uvs, colors, vindices);

    auto mesh = _renderer.createMesh((*textureIt).second,
                                     glx::Mesh::kTriangles,
                                     pos, uvs, colors, vindices);
    glx::Mesh* pmesh = mesh.get();
    _meshes.emplace(pmesh, mesh);

    return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(pmesh);
}

void RocketRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry,
                                                   const Rocket::Core::Vector2f& translation)
{
    glx::Mesh* pmesh = reinterpret_cast<glx::Mesh*>(geometry);
    _renderer.drawMesh(*pmesh, glm::vec2(translation.x, translation.y));
}

void RocketRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
    glx::Mesh* pmesh = reinterpret_cast<glx::Mesh*>(geometry);
    _meshes.erase(pmesh);
}

void RocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices,
                                           int num_vertices, int* indices, int num_indices,
                                           Rocket::Core::TextureHandle texture,
                                           const Rocket::Core::Vector2f& translation)
{
    auto textureIt = _textures.find(reinterpret_cast<glx::Texture*>(texture));
    if (textureIt == _textures.end())
        return;

    vector<glm::vec2> pos;
    vector<glm::vec2> uvs;
    vector<glm::vec4> colors;
    vector<uint16_t> vindices;

    buildArrays(vertices, num_vertices,
                indices, num_indices,
                pos, uvs, colors, vindices);

    _renderer.drawMeshVertices(*(*textureIt).second.get(),
                               glx::Mesh::kTriangles,
                               pos,
                               uvs,
                               colors,
                               vindices);
}

void RocketRenderInterface::EnableScissorRegion(bool enable)
{
    if (enable)
    {
        _renderer.enableScissor();
    }
    else
    {
        _renderer.disableScissor();
    }
}

void RocketRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
    _renderer.setScissor(glx::Rect::rectFromDimensions(x,y,width,height));
}


bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle,
                                        Rocket::Core::Vector2i& texture_dimensions,
                                        const Rocket::Core::String& source)
{
    auto texture = _renderer.loadTexture(source.CString());
    if (!(*texture))
        return false;
    glx::Texture* ptexture = texture.get();
    _textures.emplace(ptexture, texture);

    texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(ptexture);
    texture_dimensions.x = (int)ptexture->width();
    texture_dimensions.y = (int)ptexture->height();

    return true;
}

bool RocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
                                            const Rocket::Core::byte* source,
                                            const Rocket::Core::Vector2i& source_dimensions)
{
    auto texture = _renderer.createTextureFromBuffer(
                                                source_dimensions.x, source_dimensions.y,
                                                kCinekPixelFormat_ABGR8888, (const uint8_t*)source
                                            );
    if (!(*texture))
        return false;

    glx::Texture* ptexture = texture.get();
    _textures.emplace(ptexture, texture);
    texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(ptexture);

    return true;
}

void RocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture)
{
    glx::Texture* ptexture = reinterpret_cast<glx::Texture*>(texture);
    _textures.erase(ptexture);
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

    }   // namespace ovengine
}   // namespace cinekine

