//
//  IsoNodeGraph.hpp
//  Overview
//
//  Created by Samir Sinha on 7/27/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_IsoNodeGraph_hpp
#define Overview_IsoNodeGraph_hpp

#include "Game/Render/IsoNode.hpp"

#include "cinek/objectstack.hpp"
#include "cinek/vector.hpp"

#include <functional>

namespace cinekine { namespace ovengine {

    struct Tile;

    class IsoNodeGraph
    {
    public:
        IsoNodeGraph(size_t nodeLimit, const Allocator& allocator);

        /// Resets the graph's node lists
        ///
        void reset();

        /// Obtains an IsoNode from the graph's pool.
        /// @return A pointer to the IsoNode.  The IsoNodeGraph owns this
        ///         node.
        ///
        IsoNode* obtainNode(const cinek_bitmap& bitmap,
                            const glm::ivec2& viewPos, const AABB<glm::vec3>& box);

        /// Sort the graph based on the current node list
        /// The current version makes this operation an n^2 one.  we could probably
        /// optimize this sort, so that our algortihm gathers behind nodes B1...BN
        /// for node A, where B1...BN are nodes from a partition (rows) above node A,
        /// instead of the entire node list.  The result would be a n*log(n)
        /// operation.
        ///
        void sort();

        /// Iterates through all IsoNode objects in the container
        ///
        /// @param  fn The delegate to call for every tile
        ///
        const vector<IsoNode*> nodes() const { return _nodes; }

    private:
        /// Adds the IsoNode to the behind list.  IsoNode uses the returned
        /// index to keep track of its own behind list.
        ///
        /// @param  The IsoNode added to the common behind list
        /// @return The index to the node on the behind list
        ///
        uint32_t pushNodeBehind(IsoNode* node);

        /// Sorts the node and its behind nodes in the graph
        ///
        /// @param node  The IsoNode to sort
        ///
        void fixupNode(IsoNode* node);

        ObjectStack<IsoNode> _nodePool;
        vector<IsoNode*> _nodes;
        //  these are the collective nodes behind *each* IsoNode
        //  each IsoNode refers to a particular slice of this vector.
        //  the idea is for our graph to take advantage of a single
        //  continguous block of pointers that point to an (ideally)
        //  contingous pool of IsoNodes.
        vector<IsoNode*> _nodesBehind;

        uint32_t _sortDepth;
    };

} /* namespace ovengine */ } /* namespace cinekine */

#endif
