//
//  Context.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

#ifndef CK_Graphics_Context_hpp
#define CK_Graphics_Context_hpp

#include "GfxTypes.hpp"

#include <unordered_map>
#include <string>

namespace cinek {
    namespace gfx {
    
class Context
{
public:
    struct ResourceInitParams
    {
        uint32_t numMeshes;
        uint32_t numMaterials;
        uint32_t numTextures;
        uint32_t numAnimations;
        uint32_t numLights;
    };
    
    Context() = default;
    explicit Context(const ResourceInitParams& params);
    
    //  Run periodically to update the context's state.  State in this case
    //  includes file I/O, resource cleanup and other periodic functions
    void update();
    
    //  Generates a MeshHandle for system-wide persistance
    MeshHandle registerMesh(Mesh&& mesh);
    //  Generates a TextureHandle for system-wide persistance and maps it to
    //  a key name
    TextureHandle registerTexture(Texture&& texture, const char* name="");
    //  Loads a Texture using the file system (see cinek::file for details on
    //  customizing the file interface.)  The texture may not be loaded
    //  immediately - so the returned texture could be empty, but filled in
    //  at a later time (i.e. asynchronously, if the underlying system supports
    //  async file IO.)
    TextureHandle loadTexture(const char* pathname, const char* name="");
    //  Unregisters the named Texture from the Context's dictionary
    void unregisterTexture(const char* name);
    //  Finds a texture from the Context's dictionary
    TextureHandle findTexture(const char* name) const;
    //  Generates a MaterialHandle for system-wide persistance and maps it to
    //  a key name
    MaterialHandle registerMaterial(Material&& material, const char* name="");
    //  Unregisters the named Material
    void unregisterMaterial(const char* name);
    //  Finds a named Material
    MaterialHandle findMaterial(const char* name) const;
    //  Generates an AnimationSet handle for system-wide persistance and maps it
    //  optionally to a name
    AnimationSetHandle registerAnimationSet(AnimationSet&& animation, const char* name="");
    //  Unregisters the named AnimationSet
    void unregisterAnimationSet(const char* name);
    //  Finds an AnimationSet given its name
    AnimationSetHandle findAnimationSet(const char* name);
    //  Registers a light handle with an optional name
    LightHandle registerLight(Light&& light);
    
private:
    //  restrict Context access to pointer and reference -
    //  copying of contexts is prohibited and move operations limited to
    //  construction
    //
    //  contexts can only be explicitly created and destroyed
    Context(const Context&);
    Context& operator=(const Context&);
    Context& operator=(Context&&);
    
    template<typename Dictionary>
    typename Dictionary::mapped_type registerResource(
        typename Dictionary::mapped_type::Value&& value,
        typename Dictionary::mapped_type::Owner& pool,
        Dictionary& dictionary,
        const char* name);
    
    template<typename Dictionary>
    void unregisterResource(Dictionary& dictionary, const char* name);
    
private:
    MeshPool _meshes;
    MaterialPool _materials;
    TexturePool _textures;
    AnimationSetPool _animationSets;
    LightPool _lights;
    
    template<typename Handle>
    using ResourceDictionary =
        std::unordered_map<std::string, Handle,
                           std::hash<std::string>,
                           std::equal_to<std::string>,
                           std_allocator<std::pair<const std::string, Handle>>>;
    
    using TextureDictionary = ResourceDictionary<TextureHandle>;
    using MaterialDictionary = ResourceDictionary<MaterialHandle>;
    using AnimationSetDictionary = ResourceDictionary<AnimationSetHandle>;
    
    TextureDictionary _textureDictionary;
    MaterialDictionary _materialDictionary;
    AnimationSetDictionary _animationSetDictionary;
};

    
    }   // namespace gfx
}   //  namespace cinek

#endif
