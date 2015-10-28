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
#include "Animation.hpp"

namespace cinek {
    namespace gfx {
    

template<typename Dictionary>
typename Dictionary::mapped_type Context::registerResource
(
    typename Dictionary::mapped_type::Value&& value,
    typename Dictionary::mapped_type::Owner& pool,
    Dictionary& dictionary,
    const char* name
)
{
    typename Dictionary::mapped_type h;
    
    auto it = dictionary.end();
    
    if (name && *name) {
        std::string key = name;
        it = dictionary.find(key);
        if (it != dictionary.end()) {
            h = it->second;
        }
        else {
            it = dictionary.emplace(key, h).first;
        }
    }
    if (h) {
        //  existing handle, emplacing a new material into it
        h.setValue(std::move(value));
    }
    else {
        //  new handle - add it to the dictionary if applicable
        h = pool.add(std::move(value));
        if (it != dictionary.end()) {
            it->second = h;
        }
    }
    return h;
}


template<typename Dictionary>
void Context::unregisterResource(Dictionary& dictionary, const char* name)
{
    std::string key = name;
    auto it = dictionary.find(key);
    if (it != dictionary.end()) {
        dictionary.erase(it);
    }
}


Context::Context(const ResourceInitParams& params) :
    _meshes(params.numMeshes),
    _materials(params.numMaterials),
    _textures(params.numTextures),
    _animationSets(params.numAnimations)
{
}

MeshHandle Context::registerMesh(Mesh&& mesh)
{
    return _meshes.add(std::move(mesh));
}

TextureHandle Context::registerTexture(Texture&& texture, const char* name)
{
    return registerResource(std::move(texture), _textures, _textureDictionary, name);
}

TextureHandle Context::loadTexture(const char* pathname, const char* name)
{
    Texture texture = Texture::loadTextureFromFile(pathname);
    
    return registerTexture(std::move(texture), name);
}

void Context::unregisterTexture(const char* name)
{
    unregisterResource(_textureDictionary, name);
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
    return registerResource(std::move(material), _materials, _materialDictionary, name);
}

void Context::unregisterMaterial(const char* name)
{
    unregisterResource(_materialDictionary, name);
}

MaterialHandle Context::findMaterial(const char* name) const
{
    std::string key = name;
    
    auto it = _materialDictionary.find(key);
    if (it == _materialDictionary.end())
        return nullptr;
    
    return it->second;
}

AnimationSetHandle Context::registerAnimationSet
(
    AnimationSet&& animation,
    const char* name
)
{
    return registerResource(std::move(animation), _animationSets, _animationSetDictionary, name);
}

void Context::unregisterAnimationSet(const char* name)
{
    unregisterResource(_animationSetDictionary, name);
}


AnimationSetHandle Context::findAnimationSet(const char* name)
{
    std::string key = name;
    
    auto it = _animationSetDictionary.find(key);
    if (it == _animationSetDictionary.end())
        return nullptr;
    
    return it->second;
}

void Context::update()
{
}
    
    }   // namespace gfx
}   //  namespace cinek