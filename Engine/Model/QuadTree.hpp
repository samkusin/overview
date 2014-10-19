///
/// @file
/// A generic QuadTree implementation
///
/// @author     Samir Sinha
/// @date       09/18/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_QuadTree_hpp
#define Overview_Model_QuadTree_hpp

#include "Engine/Model/ModelTypes.hpp"
#include <array>

namespace cinek {
    namespace overview {

    /**
     * @class   QuadTree
     * @brief   A generic QuadTree that places Value objects within a quadrant
     *          based on a 2D graph position.
     * @details The Value must have certain traits allowing it to be compared
     *          against the X and Y axis of a QuadTree
     *          Value::AABB must be defined where AABB has a center() method,
     *          and AABB must have a point_type typedef.
     *
     *          Value must have a method:
     *          Quadrant determineQuadrant(const Value::point_type& center)
     *
     *          TreeAllocator must have at least two methods:
     *          this_type create() and destroy(this_type)
     */
    template<typename Value>
    class QuadTree
    {
    public:
        typedef QuadTree<Value, TreeAllocator>  this_type;
        typedef typename Value::AABB            AABB;

        QuadTree(const AABB& bounds, TreeAllocator* allocator);

    private:
        TreeAllocator* _allocator;
        AABB _bounds;
        std::array<this_type*, Quadrant::kCount> _nodes;
    };

    } /* namespace overview */
} /* namespace cinek */

