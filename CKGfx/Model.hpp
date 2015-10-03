//
//  Model.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/28/15.
//
//

#ifndef CK_Graphics_Model_hpp
#define CK_Graphics_Model_hpp

#include "GfxTypes.hpp"
#include "Node.hpp"

#include <cinek/objectpool.hpp>

namespace cinek {
    namespace gfx {
 
class Model
{
    CK_CLASS_NON_COPYABLE(Model);
    
public:    
    explicit Model(const NodeElementCounts& params);
    Model() = default;
    Model(Model&& model) noexcept;
    Model& operator=(Model&& other) noexcept;
    ~Model();

    NodeHandle root() const { return _root; }
    NodeHandle createTransformNode();
    NodeHandle createMeshNode(uint32_t elementCnt);
    
    NodeHandle addChildNodeToNode(NodeHandle child, NodeHandle node);
    NodeHandle removeNode(NodeHandle node);
  
private:
    //  Invoked when Nodes are beling released.
    void onReleaseManagedObject(Node& node);
    
private:
    friend NodePool;
    
    ObjectPool<TransformElement> _transformElementsPool;
    ObjectPool<MeshElement> _meshElementPool;
    NodePool _nodes;
    NodeHandle _root;
};

    }   // namespace gfx
}   // namespace cinek

#endif
