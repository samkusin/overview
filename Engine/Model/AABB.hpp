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
    struct AABB
    {
        typedef Point::value_type  ValueType;
        typedef Point              Type;
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


    inline AABB::operator bool() const
    {
        return min != max;
    }

    inline AABB::Type AABB::size() const
    {
        return max - min;
    }

    inline bool AABB::inside(const AABB& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y &&
                min.z >= box.min.z && max.z <= box.max.z);
    }

    inline bool AABB::outside(const AABB& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y ||
                min.z > box.max.z || box.min.z > max.z);
    }

    inline bool AABB::intersects(const AABB& box) const
    {
        return !outside(box);
    }

    inline bool AABB::operator==(const AABB& box) const
    {
        return min != box.min || max != box.max;
    }

    inline bool AABB::operator!=(const AABB& box) const
    {
        return min != box.min || max != box.max;
    }

    inline void AABB::clear()
    {
        min = Point();
        max = Point();
    }

    inline AABB::Type AABB::center() const
    {
        return (min + max)/2.f;
    }

    inline AABB operator+(const AABB& box, const AABB::Type& off)
    {
        AABB result;
        result.min = box.min + off;
        result.max = box.max + off;
        return result;
    }

} /* namespace ovengine */ } /* namespace cinekine */

#endif

