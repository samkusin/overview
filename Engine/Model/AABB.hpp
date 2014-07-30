///
/// @file
/// Utilities for an Axis Aligned Bounding Box (AABB)
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_AABB_hpp
#define Overview_Model_AABB_hpp

#include "Engine/Model/ModelTypes.hpp"

namespace cinekine { namespace ovengine {

    /// @struct AABB
    /// @brief  An axis-aligned box in 3D space
    ///
    template<class _Point>
    struct AABB
    {
        typedef typename _Point::value_type  ValueType;
        typedef _Point                       Type;
        Type min;          ///< The min coordinate
        Type max;          ///< The max coordinate

        /// Checks if the box is valid.  The box could be flat (planar), or even
        /// a line if the min/max points share common coordinates.
        ///
        /// @return False if the box has a zero size
        ///
        operator bool() const;
        /// Calculates the size of the box as a Point
        /// @return The x,y,z sizes of the box
        ///
        Type size() const;
        /// Determines if this box lies within a box
        /// @param  box The box to test
        /// @return True if the this box lies within the supplied box
        ///
        bool inside(const AABB& box) const;
        /// Determines if this box contains a point
        /// @param  point The point to test
        /// @return True if the this box contains the supplied point
        ///
        bool contains(const Type& pt) const;
        /// Determines if this box lies outside a box
        /// @param  box The box to test
        /// @return True if this box lies outside the supplied box
        ///
        bool outside(const AABB& box) const;
        /// Determines if a box intersects this box
        /// @param  box The box to test
        /// @return True if the box intersects this box
        ///
        bool intersects(const AABB& box) const;
        /*
         * /// Calculates the intersection between a box and this box
         * /// @param  box The box to check
         * /// @return The intersection between this box and the passed in box
         * ///
         * AABB intersection(const AABB& box) const;
        */
        /// Calculates the bounding box from the supplied box and this box
        /// @param  bounds  The box included within our calculated bounding
        ///                 volume
        /// @return The bounding box between this box and the supplied box
        AABB boundTo(const AABB& box) const;
        /// @param  box The box to test
        /// @return True if the box equals this box
        bool operator==(const AABB& box) const;
        /// @param  box The box to test
        /// @return True if the box does not equal this box
        bool operator!=(const AABB& box) const ;
        /// Zeros out the box
        ///
        void clear();
        /// Calculates the center point of our box
        /// @return The center point
        ///
        Type center() const;
    };

    ////////////////////////////////////////////////////////////////////////////


    template<class _Point>
    AABB<_Point>::operator bool() const
    {
        return min != max;
    }

    template<class _Point>
    typename AABB<_Point>::Type AABB<_Point>::size() const
    {
        return max - min;
    }

    template<class _Point>
    bool AABB<_Point>::inside(const AABB<_Point>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y &&
                min.z >= box.min.z && max.z <= box.max.z);
    }
    
    template<class _Point>
    bool AABB<_Point>::contains(const Type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y &&
                pt.z >= min.z && pt.z <= max.z);
    }

    template<class _Point>
    bool AABB<_Point>::outside(const AABB<_Point>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y ||
                min.z > box.max.z || box.min.z > max.z);
    }

    template<class _Point>
    bool AABB<_Point>::intersects(const AABB<_Point>& box) const
    {
        return !outside(box);
    }

    template<class _Point>
    bool AABB<_Point>::operator==(const AABB<_Point>& box) const
    {
        return min != box.min || max != box.max;
    }

    template<class _Point>
    bool AABB<_Point>::operator!=(const AABB<_Point>& box) const
    {
        return min != box.min || max != box.max;
    }

    template<class _Point>
    void AABB<_Point>::clear()
    {
        min = Point();
        max = Point();
    }

    template<class _Point>
    typename AABB<_Point>::Type AABB<_Point>::center() const
    {
        return (min + max)/2.f;
    }

    template<class _Point>
    AABB<_Point> operator+(const AABB<_Point>& box, const typename AABB<_Point>::Type& off)
    {
        AABB<_Point> result;
        result.min = box.min + off;
        result.max = box.max + off;
        return result;
    }

} /* namespace ovengine */ } /* namespace cinekine */

#endif

