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

#include <array>

namespace cinek { namespace overview {

    /// @struct AABB
    /// @brief  An axis-aligned box in 3D space
    ///
    template<class _Point>
    struct AABB
    {
        typedef _Point                   point_type;
        typedef AABB<_Point>             this_type;

        point_type min;          ///< The min coordinate
        point_type max;          ///< The max coordinate

        /// Default Constructor
        ///
        AABB();
        /// Constructor
        /// @param halfDim  Radius of the box
        ///
        AABB(const point_type& halfDim);
        /// Constructor
        /// @param min  Min point of the box
        /// @param max  Max point of the box
        ///
        AABB(const point_type& min, const point_type& max);
        /// Checks if the box is valid.  The box could be flat (planar), or even
        /// a line if the min/max points share common coordinates.
        ///
        /// @return False if the box has a zero size
        ///
        operator bool() const;
        /// Calculates the size of the box as a Point
        /// @return The x,y,z sizes of the box
        ///
        point_type dimensions() const;
        /// Determines if this box lies within a box
        /// @param  box The box to test
        /// @return True if the this box lies within the supplied box
        ///
        bool inside(const AABB& box) const;
        /// Determines if this box contains a point
        /// @param  point The point to test
        /// @return True if the this box contains the supplied point
        ///
        bool contains(const point_type& pt) const;
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
        point_type center() const;
        /// Generate vertices from this AABB
        /// @param verts An array of vertices to be filled in
        ///
        void generateVertices(std::array<point_type, 8>& verts);
        /// Rotates, generating a new AABB bounding the rotated box
        /// @param mtxRot Rotation and Translation matrix
        ///
        template<class _Matrix>
        void rotate(const _Matrix& mtxRot);
        /// Merges another AABB with this AABB.  The result is stored in this
        /// @param  aabb  The AABB to merge
        /// @return A reference to this AABB
        ///
        AABB& merge(const AABB& aabb);
    };

    ////////////////////////////////////////////////////////////////////////////
    template<class _Point>
    AABB<_Point>::AABB()
    {
        clear();
    }

    template<class _Point>
    AABB<_Point>::AABB(const point_type& halfDim)
    {
        clear();
        min -= halfDim;
        max += halfDim;
    }

    template<class _Point>
    AABB<_Point>::AABB(const point_type& min, const point_type& max) : min(min), max(max)
    {
    }

    template<class _Point>
    AABB<_Point>::operator bool() const
    {
        return min != max;
    }

    template<class _Point>
    typename AABB<_Point>::point_type AABB<_Point>::dimensions() const
    {
        return max - min;
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
    typename AABB<_Point>::point_type AABB<_Point>::center() const
    {
        return (min + max)/2.f;
    }

    template<class _Point>
    AABB<_Point> operator+(const AABB<_Point>& box, const typename AABB<_Point>::point_type& off)
    {
        AABB<_Point> result;
        result.min = box.min + off;
        result.max = box.max + off;
        return result;
    }

} /* namespace overview */ } /* namespace cinek */

#endif

