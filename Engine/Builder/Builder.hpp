/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Samir Sinha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef Overview_Builder_Hpp
#define Overview_Builder_Hpp

#include "BuilderTypes.hpp"
#include "Engine/Model/Map.hpp"

#include "cinek/framework/vector.hpp"

namespace cinekine {
    namespace rendermodel {
        class TileDatabase;
    }
}

namespace cinekine { namespace ovengine {

    /// @struct NewRegionInstruction
    /// @brief Instructs the builder to construct a segment within a room
    ///
    struct NewRegionInstruction
    {
        enum Policy
        {
            /// Finalize the generated map signaling the end of region building
            kNone,
            /// Attach segment to a random side.  If the first *randomized*
            /// segment in a room then the segment is applied as is, being the
            /// root node of all subsequent segments in the room
            kRandomize,
            /// Create segment as defined - this is a brute force method,
            /// overriding the current segment tree for the room.
            kFixed
        }
        policy;                 ///< The segment's build policy
        Box box;                ///< The segment to build from the box
        bool terminal;          ///< Indicates a leaf in the segment tree
        NewRegionInstruction():
            policy(kNone),
            box(),
            terminal(false) {}
        NewRegionInstruction(Policy policy) :
            policy(policy),
            box(),
            terminal(false) {}
    };

    /// @class Builder
    /// @brief The Builder responsible for generating and connecting Rooms
    //
    class Builder
    {
    public:
        Builder(Map& map,
                const rendermodel::TileDatabase& tileTemplates,
                uint32_t roomLimit,
                const Allocator& allocator);

        /// @return The region handle
        int makeRegion(const TileBrush& floorBrush,
                       const TileBrush& wallBrush,
                       const vector<NewRegionInstruction>& instructions);
        /// @return The connection handle
        int connectRegions(const TileBrush& floorBrush,
                           const TileBrush& wallBrush,
                    int startRegionHandle, int endRegionHandle,
                    const vector<MapPoint>& connectPoints);

    private:
        struct Segment
        {
            Box box;
            Segment();
            Segment(const Box& box);

        };
        struct Region
        {
            Box bounds;
            vector<Segment*> segments;

            Region(): bounds() {}
            Segment* closestSegmentToPoint(const MapPoint& pt);
        };
        struct Connection
        {
            int regionA;
            int regionB;
        };

        void paintConnectionOntoMap(const TileBrush& floorBrush,
                                    const TileBrush& wallBrush,
                                    const MapPoint& p0,
                                    const MapPoint& p1);
        void paintBoxOntoMap(const TileBrush& floorBrush, const TileBrush& wallBrush,
                             const Box& box);
        void paintTileWalls(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                    const TileBrush& brush);
        void paintTileWallCorners(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                    const TileBrush& brush);
        bool tileFloorsClassIdEqual(const Tile& tile, uint8_t thisClassId) const;
        bool tileWallsEqual(const Tile& tile, uint16_t roleFlags, uint8_t classId) const;

    private:
        Allocator _allocator;
        Map& _map;
        const rendermodel::TileDatabase& _tileTemplates;

        vector<Region> _regions;
        vector<Segment> _segments;
        vector<Connection> _connections;
    };
} /* namespace overview */ } /* namespace cinekine */

#endif
