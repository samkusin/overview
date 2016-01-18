//
//  ModelSet.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/1/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_ModelSet_hpp
#define CK_Graphics_ModelSet_hpp

#include "GfxTypes.hpp"
#include "NodeGraph.hpp"

#include <vector>
#include <string>

namespace cinek {
    namespace gfx {
    
class ModelSet
{
public:
    ModelSet() = default;
    ModelSet(NodeGraph&& nodeGraph,
             std::vector<std::pair<std::string, NodeHandle>>&& models);
    
    NodeHandle find(const std::string& name) const;
    
private:
    NodeGraph _nodeGraph;
    std::vector<std::pair<std::string, NodeHandle>> _models;
};
    
    } /* namespace gfx */
} /* namespace cinek */


#endif /* ModelSet_hpp */
