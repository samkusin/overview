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
    
struct NodeElementCounts
{
    uint32_t transformNodeCount;
    uint32_t objectNodeCount;
    uint32_t meshNodeCount;
    uint32_t armatureNodeCount;
    uint32_t lightNodeCount;
};
 
class NodeGraph
{
    CK_CLASS_NON_COPYABLE(NodeGraph);
    
public:    
    explicit NodeGraph(const NodeElementCounts& params);
    NodeGraph() = default;
    NodeGraph(NodeGraph&& model);
    NodeGraph& operator=(NodeGraph&& other);
    ~NodeGraph();

    NodeHandle root() const { return _root; }
    NodeHandle createObjectNode(NodeId nodeId);
    NodeHandle createMeshNode(uint32_t elementCnt);
    NodeHandle createArmatureNode();
    NodeHandle createLightNode();
    
    void setRoot(NodeHandle node);
    NodeHandle addChildNodeToNode(NodeHandle child, NodeHandle node);
    NodeHandle removeNode(NodeHandle node);
    
    //  a way to clone the contents of a Node and its children that belong to
    //  another node graph, into this graph
    NodeHandle clone(NodeHandle handle);
  
private:
    //  Invoked when Nodes are beling released.
    void onReleaseManagedObject(Node& node);
    
private:
    friend NodePool;
    
    ObjectPool<MeshElement> _meshElementPool;
    ObjectPool<ArmatureElement> _armatureElementPool;
    ObjectPool<LightElement> _lightElementPool;
    NodePool _nodes;
    NodeHandle _root;
};

    }   // namespace gfx
}   // namespace cinek

#endif
