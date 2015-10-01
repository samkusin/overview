//
//  Context.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

#include "Context.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Mesh.hpp"


namespace cinek {
    namespace gfx {

Context::Context(const ResourceInitParams& params) :
    _meshes(params.numMeshes),
    _materials(params.numMaterials),
    _textures(params.numTextures)
{
}

MeshHandle Context::registerMesh(Mesh&& mesh)
{
    return _meshes.add(std::move(mesh));
}

TextureHandle Context::registerTexture(Texture&& texture, const char* name)
{
    TextureHandle h;
    
    auto it = _textureDictionary.end();
    
    if (name && *name) {
        std::string key = name;
        it = _textureDictionary.find(key);
        if (it != _textureDictionary.end()) {
            h = it->second;
        }
        else {
            it = _textureDictionary.emplace(key, h).first;
        }
    }
    if (h) {
        //  existing handle, emplacing a new texture into it
        *h = std::move(texture);
    }
    else {
        //  new handle - add it to the dictionary if applicable
        h = _textures.add(std::move(texture));
        if (it != _textureDictionary.end()) {
            it->second = h;
        }
    }
    
    return h;
}

TextureHandle Context::loadTexture(const char* pathname, const char* name)
{
    Texture texture = Texture::loadTextureFromFile(pathname);
    
    return registerTexture(std::move(texture), name);
}

void Context::unregisterTexture(const char* name)
{
    std::string key = name;
    auto it = _textureDictionary.find(key);
    if (it != _textureDictionary.end()) {
        _textureDictionary.erase(it);
    }
}

TextureHandle Context::findTexture(const char* name) const
{
    std::string key = name;
    
    auto it = _textureDictionary.find(key);
    if (it == _textureDictionary.end())
        return nullptr;
    
    return it->second;
}

MaterialHandle Context::registerMaterial(Material&& material, const char* name)
{
    MaterialHandle h;
    
    auto it = _materialDictionary.end();
    
    if (name && *name) {
        std::string key = name;
        it = _materialDictionary.find(key);
        if (it != _materialDictionary.end()) {
            h = it->second;
        }
        else {
            it = _materialDictionary.emplace(key, h).first;
        }
    }
    if (h) {
        //  existing handle, emplacing a new material into it
        *h = std::move(material);
    }
    else {
        //  new handle - add it to the dictionary if applicable
        h = _materials.add(std::move(material));
        if (it != _materialDictionary.end()) {
            it->second = h;
        }
    }
    return h;
}

void Context::unregisterMaterial(const char* name)
{
    std::string key = name;
    auto it = _materialDictionary.find(key);
    if (it != _materialDictionary.end()) {
        _materialDictionary.erase(it);
    }
}

MaterialHandle Context::findMaterial(const char* name) const
{
    std::string key = name;
    
    auto it = _materialDictionary.find(key);
    if (it == _materialDictionary.end())
        return nullptr;
    
    return it->second;
}

void Context::update()
{
}
    
    }   // namespace gfx
}   //  namespace cinek