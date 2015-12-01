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
#include "Light.hpp"
#include "NodeGraph.hpp"

namespace cinek {
    namespace gfx {
    
Context::Context(const ResourceInitParams& params) :
    _meshes(params.numMeshes),
    _materials(params.numMaterials),
    _textures(params.numTextures),
    _animationSets(params.numAnimations),
    _lights(params.numLights)
{
}

void Context::setTextureLoadDelegate(TextureLoadDelegate delegate)
{
    _textureLoadDelegate = std::move(delegate);
}

auto Context::textureLoadDelegate() const -> TextureLoadDelegate
{
    return _textureLoadDelegate;
}

MeshHandle Context::registerMesh(Mesh&& mesh)
{
    return _meshes.add(std::move(mesh));
}

TextureHandle Context::registerTexture(Texture&& texture, const char* name)
{
    return registerResource(std::move(texture), _textures, _textureDictionary, name);
}

TextureHandle Context::loadTexture(const char* pathname)
{
    Texture texture;
    if (_textureLoadDelegate) {
        texture = _textureLoadDelegate(*this, pathname);
    }
    else {
        texture = Texture::loadTextureFromFile(pathname);
    }
    return registerTexture(std::move(texture), pathname);
}

void Context::unregisterTexture(const char* name)
{
    unregisterResource(_textureDictionary, name);
}

TextureHandle Context::findTexture(const char* name) const
{
    auto it = _textureDictionary.find(name);
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


AnimationSetHandle Context::findAnimationSet(const char* name) const
{
    auto it = _animationSetDictionary.find(name);
    if (it == _animationSetDictionary.end())
        return nullptr;
    
    return it->second;
}

LightHandle Context::registerLight(Light&& light)
{
    return _lights.add(std::move(light));
}

NodeGraphHandle Context::registerModel
(
    NodeGraph&& graph,
    const char* name
)
{
    return registerResource(std::move(graph), _models, _modelDictionary, name);
}

void Context::unregisterModel(const char *name)
{
    unregisterResource(_modelDictionary, name);
}

NodeGraphHandle Context::findModel(const char* name) const
{
    auto it = _modelDictionary.find(name);
    if (it == _modelDictionary.end())
        return nullptr;
    
    return it->second;
}

void Context::update()
{
}
    
    }   // namespace gfx
}   //  namespace cinek