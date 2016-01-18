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

#include <cinek/managed_dictionary.hpp>

#include <functional>

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
        uint32_t numModelSets;
    };
    
    Context() = default;
    explicit Context(const ResourceInitParams& params);
    
    //  Used for texture loading by an external system.  The external system
    //  uses the supplied name to load a Texture, and then to use the context
    //  to register it.
    using TextureLoadDelegate = std::function<Texture(Context&, const std::string&)>;
    
    //  Sets the Texture load delegate issued when a call to loadTexture is
    //  made.
    void setTextureLoadDelegate(TextureLoadDelegate delegate);
    TextureLoadDelegate textureLoadDelegate() const;
    
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
    TextureHandle loadTexture(const char* pathname);
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
    AnimationSetHandle findAnimationSet(const char* name) const;
    //  Registers a light handle
    LightHandle registerLight(Light&& light);
    //  Registers a NodeGraph Model with an noptinoal name
    ModelSetHandle registerModelSet(ModelSet&& modelSet, const char* name="");
    //  Unregisters a Model
    void unregisterModelSet(const char *name);
    //  Finds a Model given its name
    ModelSetHandle findModelSet(const char* name) const;
    
private:
    //  restrict Context access to pointer and reference -
    //  copying of contexts is prohibited and move operations limited to
    //  construction
    //
    //  contexts can only be explicitly created and destroyed
    Context(const Context&);
    Context& operator=(const Context&);
    Context& operator=(Context&&);
    
private:
    MeshPool _meshes;
    MaterialPool _materials;
    TexturePool _textures;
    AnimationSetPool _animationSets;
    LightPool _lights;
    ModelSetPool _modelSets;
    
    using TextureDictionary = ManagedDictionary<TextureHandle>;
    using MaterialDictionary = ManagedDictionary<MaterialHandle>;
    using AnimationSetDictionary = ManagedDictionary<AnimationSetHandle>;
    using ModelSetDictionary = ManagedDictionary<ModelSetHandle>;
    
    TextureDictionary _textureDictionary;
    MaterialDictionary _materialDictionary;
    AnimationSetDictionary _animationSetDictionary;
    ModelSetDictionary _modelSetDictionary;
    
    TextureLoadDelegate _textureLoadDelegate;
};

    
    }   // namespace gfx
}   //  namespace cinek

#endif
