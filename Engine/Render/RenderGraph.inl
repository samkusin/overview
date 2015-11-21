//
//  RenderGraph.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "RenderGraph.hpp"

namespace cinek {
    namespace ove {

template<typename _Vector, typename _Basis, typename _NodeDelegate>
RenderGraph<_Vector,_Basis,_NodeDelegate>::RenderGraph
(
    const gfx::NodeElementCounts& counts
)
{
}

template<typename _Vector, typename _Basis, typename _NodeDelegate>
void RenderGraph<_Vector,_Basis,_NodeDelegate>::cloneAndAddNodeWithDelegate
(
    Entity e,
    gfx::Node sourceNode,
    _NodeDelegate prepareDel
)
{
}

template<typename _Vector, typename _Basis, typename _NodeDelegate>
void RenderGraph<_Vector,_Basis,_NodeDelegate>::removeNode(Entity e)
{
}


template<typename _Vector, typename _Basis, typename _NodeDelegate>
void RenderGraph<_Vector,_Basis,_NodeDelegate>::prepare(double dt)
{
}

template<typename _Vector, typename _Basis, typename _NodeDelegate>
void RenderGraph<_Vector,_Basis,_NodeDelegate>::render
(
    gfx::NodeRenderer& renderer
)
{
}
    
    }   /* namesapce ove */
}   /* namespace cinek */