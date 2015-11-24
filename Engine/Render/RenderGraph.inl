//
//  RenderGraph.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "RenderGraph.hpp"

#include "CKGfx/AnimationController.hpp"

namespace cinek {
    namespace ove {

template<typename _Delegate>
void RenderGraph::prepare(double dt, _Delegate del)
{
    updateRenderNodes(dt);
    
    for (auto& node : _renderNodes) {
        if (del && node.gfxNode) {
            del(*node.gfxNode.resource(), node.context);
        }
    }    
}

    }   /* namespace ove */
}   /* namespace cinek */
