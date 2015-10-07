//
//  NodeGraph.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/28/15.
//
//

#ifndef CK_Graphics_NodeGraph_hpp
#define CK_Graphics_NodeGraph_hpp

#include "GfxTypes.hpp"
#include "Node.hpp"

#include <cinek/objectpool.hpp>

namespace cinek {
    namespace gfx {
 
class NodeGraph
{
    CK_CLASS_NON_COPYABLE(NodeGraph);
    
public:    
    explicit NodeGraph(const NodeElementCounts& params);
    NodeGraph() = default;
    NodeGraph(NodeGraph&& model) noexcept;
    NodeGraph& operator=(NodeGraph&& other) noexcept;
    ~NodeGraph();

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
