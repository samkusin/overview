//
//  ModelSet.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/1/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "ModelSet.hpp"

#include <algorithm>

namespace cinek {
    namespace gfx {


ModelSet::ModelSet
(
    NodeGraph&& nodeGraph,
    std::vector<std::pair<std::string, NodeHandle>>&& models
) :
    _nodeGraph(std::move(nodeGraph)),
    _models(std::move(models))
{
    typedef decltype(_models)::value_type entry;
    std::sort(_models.begin(), _models.end(),
        [](const entry &v0, const entry &v1) -> bool {
            return v0.first < v1.first;
        });
}

    
NodeHandle ModelSet::find(const std::string& name) const
{
    typedef decltype(_models)::value_type entry;
    auto it = std::lower_bound(_models.begin(), _models.end(), name,
        [](const entry& v0, const std::string& name) -> bool {
            return v0.first < name;
        });
    if (it == _models.end() || it->first != name)
        return nullptr;
    
    return it->second;
}
    

    } /* namespace gfx */
} /* namespace cinek */
