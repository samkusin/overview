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

#include "Builder.hpp"
#include "Engine/MapTypes.hpp"

#include <cinek/rendermodel/tiledatabase.hpp>

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

namespace cinekine { namespace ovengine {

    Builder::Segment::Segment()
    {
    }

    Builder::Segment::Segment(const Box& box) :
        box(box)
    {
    }

    auto Builder::Region::closestSegmentToPoint(const MapPoint& pt) -> Segment*
    {
        //  pick closest segment to the point
        Segment* closest = nullptr;
        int d2Closest = INT32_MAX;
        for (auto segment: segments)
        {
            MapPoint ptDelta = segment->box.center() - pt;
            int d2 = glm::dot(ptDelta, ptDelta);
            if (d2 < d2Closest)
            {
                closest = segment;
                d2Closest = d2;
            }
        }
        return closest;
    }

    ////////////////////////////////////////////////////////////////////////////

    Builder::Builder(Map& map,
                const rendermodel::TileDatabase& tileTemplates,
                uint32_t roomLimit,
                const Allocator& allocator) :
        _allocator(allocator),
        _map(map),
        _tileTemplates(tileTemplates),
        _regions(std_allocator<Region>(_allocator)),
        _segments(std_allocator<Segment>(_allocator)),
        _connections(std_allocator<Connection>(_allocator))
    {
        _regions.reserve(roomLimit);
        _segments.reserve(roomLimit*8);
        _connections.reserve(roomLimit*8);

        //  clear all tilemaps
        const cinekine::ovengine::MapBounds& bounds = _map.bounds();

        for (uint32_t z = 0; z < bounds.zUnits; ++z)
        {
            Tilemap* tilemap = _map.tilemapAtZ(z);
            Tile zeroTile = { 0, 0 };
            tilemap->fillWithValue(zeroTile, 0, 0, bounds.yUnits, bounds.xUnits);
        }

    }

    int Builder::makeRegion(const TileBrush& floorBrush,
                            const TileBrush& wallBrush,
                            const vector<NewRegionInstruction>& instructions)
    {
        //  Send a request for a new room within our entire map bounds
        //  the builder currently is pretty simple - it requests a room and then
        //  paints it.
        //
        int regionIndex = -1;
        Region* region = nullptr;
        bool finalize = false;
        for (auto& cmd : instructions)
        {
            Box segBox = cmd.box;

            switch (cmd.policy)
            {
            case NewRegionInstruction::kRandomize:
                break;
            case NewRegionInstruction::kFixed:
                break;
            default:
                finalize = true;
                break;
            }

            if (_segments.size() >= _segments.capacity())
            {
                finalize = true;
            }

            if (!finalize)
            {
                if (!region)
                {
                    _regions.emplace_back();
                    region = &_regions.back();
                    regionIndex = (int)_regions.size()-1;
                }
                _segments.emplace_back(segBox);
                auto* segment = &_segments.back();

                region->segments.push_back(segment);

                paintBoxOntoMap(floorBrush, wallBrush, segment->box);
            }
            else
            {
                break;
            }
        }

        return regionIndex;
    }

    int Builder::connectRegions(const TileBrush& floorBrush,
                                const TileBrush& wallBrush,
                                int startRegionHandle, int endRegionHandle,
                                const vector<MapPoint>& connectPoints)
    {
        if (_connections.size() >= _connections.capacity())
            return -1;
        if (startRegionHandle >= _regions.size())
            return -1;
        if (endRegionHandle >= _regions.size())
            return -1;
        if (startRegionHandle == endRegionHandle)
            return -1;

        //  Paint connection onto map
        //  From the closest segment to the end region in the start region to
        //  the closest segment in the start region from the closest end room
        //  segment - these points mark our start and end points for the
        //  region's connection.
        Region& startRegion = _regions[startRegionHandle];
        Region& endRegion = _regions[endRegionHandle];
        Segment* startSegment = startRegion.closestSegmentToPoint(endRegion.bounds.center());
        if (!startSegment)
            return -1;      // only if an empty start region
        Segment* endSegment = endRegion.closestSegmentToPoint(startSegment->box.center());
        if (!endSegment)
            return -1;      // only if an empty end retion
        startSegment = startRegion.closestSegmentToPoint(endSegment->box.center());

        //  paint all connections
        MapPoint startPoint = startSegment->box.center();
        MapPoint endPoint = endSegment->box.center();
        const MapPoint* connectStartPoint = &startPoint;

        for (auto& connectPoint : connectPoints)
        {
            const MapPoint* connectEndPoint = &connectPoint;
            paintConnectionOntoMap(floorBrush, wallBrush, *connectStartPoint, *connectEndPoint);
            connectStartPoint = connectEndPoint;
        }

        paintConnectionOntoMap(floorBrush, wallBrush, *connectStartPoint, endPoint);

        _connections.emplace_back();
        Connection& connection = _connections.back();
        connection.regionA = startRegionHandle;
        connection.regionB = endRegionHandle;

        return (int)_connections.size()-1;
    }

    void Builder::paintConnectionOntoMap(const TileBrush& floorBrush,
                                         const TileBrush& wallBrush,
                                         const MapPoint& p0,
                                         const MapPoint& p1)
    {
        //  paint a hallway line using the desired plot method -
        //  by default, the method is orthogonal (half-rectangle) from p0 to p1
        const int32_t kHallSize = 3;
        const Box kHallBox = {
            Box::Point(-kHallSize/2,-kHallSize/2),
            Box::Point((kHallSize+1)/2,(kHallSize+1)/2)
        };
        MapPoint delta = p1 - p0;
        if (delta.x)
            delta.x /= abs(delta.x);
        if (delta.y)
            delta.y /= abs(delta.y);
        MapPoint ptCurrent = p0;
        int xy = rand() % 2;

        while (ptCurrent != p1)
        {
            paintBoxOntoMap(floorBrush, wallBrush, kHallBox + ptCurrent);
            if (xy == 0)            // x dominant
            {
                if (ptCurrent.x == p1.x)
                {
                    xy = 1;
                }
                else
                {
                    ptCurrent.x += delta.x;
                }
            }
            else                    // y dominant
            {
                if (ptCurrent.y == p1.y)
                {
                    xy = 0;
                }
                else
                {
                    ptCurrent.y += delta.y;
                }
            }
            paintBoxOntoMap(floorBrush, wallBrush, kHallBox + ptCurrent);
        }

    }

    //  the segment is guaranteed to lie entirely within the map's bounds
    //  the paint action is a multi step process:
    //      - step 1, paint a rect of floor tiles
    //      - step 2, paint a rect of wall tiles
    //
    void Builder::paintBoxOntoMap(const TileBrush& floorBrush,
                                  const TileBrush& wallBrush, const Box& box)
    {
        //  paint floor
        cinek_tile floorTileId = _tileTemplates.tileHandleFromDescriptor(
                                        floorBrush.tileCategoryId,
                                        floorBrush.tileClassId,
                                        kTileRole_Floor);

        Tile tile;
        tile.layer[0] = floorTileId;
        tile.layer[1] = 0;

        cinekine::ovengine::Tilemap* tileMap = _map.tilemapAtZ(0);
        tileMap->fillWithValue(tile, box.p0.y, box.p0.x,
                                 box.height(),
                                 box.width());

        //  paint walls into this segment
        //  this is a two step process - painting wall tiles, and a second
        //      pass for wall corners to "fill in the gaps" created from the
        //      first pass.
        //
        uint32_t yPos;
        for (yPos = box.p0.y; yPos < box.p1.y; ++yPos)
        {
            for (uint32_t xPos = box.p0.x; xPos < box.p1.x; ++xPos)
            {
                paintTileWalls(*tileMap, yPos, xPos, wallBrush);
            }
        }
        for (yPos = box.p0.y; yPos < box.p1.y; ++yPos)
        {
            for (uint32_t xPos = box.p0.x; xPos < box.p1.x; ++xPos)
            {
                paintTileWallCorners(*tileMap, yPos, xPos, wallBrush);
            }
        }
    }



    void Builder::paintTileWalls(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                                const TileBrush& brush)
    {
        const cinek_tile_info& thisFloorTemplate =
            _tileTemplates.tileInfo(tileMap.at(tileY, tileX).layer[0]);

        //  calculate wall masks, which are used to determine what wall tiles
        //  to display.
        uint16_t wallMask = kTileDirection_N | kTileDirection_E |
            kTileDirection_W | kTileDirection_S;

        if (tileY > 0 &&
            tileFloorsClassIdEqual(tileMap.at(tileY-1, tileX), thisFloorTemplate.classIndex))
        {
            wallMask &= ~(kTileDirection_N);
        }
        if (tileX > 0 &&
            tileFloorsClassIdEqual(tileMap.at(tileY, tileX-1), thisFloorTemplate.classIndex))
        {
            wallMask &= ~(kTileDirection_W);
        }
        if (tileY < tileMap.rowCount()-1 &&
            tileFloorsClassIdEqual(tileMap.at(tileY+1, tileX), thisFloorTemplate.classIndex))
        {
            wallMask &= ~(kTileDirection_S);
        }
        if (tileX < tileMap.columnCount()-1 &&
            tileFloorsClassIdEqual(tileMap.at(tileY, tileX+1), thisFloorTemplate.classIndex))
        {
            wallMask &= ~(kTileDirection_E);
        }

        uint16_t wallRoleFlags = 0;

        if (wallMask)
        {
            if (wallMask & kTileDirection_W)
            {
                if (wallMask & kTileDirection_N)
                    wallRoleFlags |= kTileDirection_NW;
                else if (wallMask & kTileDirection_S)
                    wallRoleFlags |= kTileDirection_SW;
                else
                    wallRoleFlags |= kTileDirection_W;
            }
            if (!wallRoleFlags && (wallMask & kTileDirection_N))
            {
                //  we've already evaluated for West, so only need to eval East
                if (wallMask & kTileDirection_E)
                    wallRoleFlags |= kTileDirection_NE;
                else
                    wallRoleFlags |= kTileDirection_N;
            }
            if (!wallRoleFlags && (wallMask & kTileDirection_E))
            {
                //  we've already evaluated North, so only care about South
                if (wallMask & kTileDirection_S)
                    wallRoleFlags |= kTileDirection_SE;
                else
                    wallRoleFlags |= kTileDirection_E;
            }
            if (!wallRoleFlags && (wallMask & kTileDirection_S))
            {
                //  we've already evaluated East and West, so...
                wallRoleFlags |= kTileDirection_S;
            }

            wallRoleFlags |= kTileRole_Wall;
        }

        cinek_tile wallTileHandle =
            _tileTemplates.tileHandleFromDescriptor(brush.tileCategoryId,
                                                brush.tileClassId,
                                                wallRoleFlags);

        Tile& thisTile = tileMap.at(tileY, tileX);
        thisTile.layer[1] = wallTileHandle;
    }

    bool Builder::tileFloorsClassIdEqual(const Tile& tile, uint8_t thisClassId) const
    {
        const cinek_tile_info& floorTemplate = _tileTemplates.tileInfo(tile.layer[0]);
        return floorTemplate.classIndex == thisClassId;
    }

    void Builder::paintTileWallCorners(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                            const TileBrush& brush)
    {
        uint16_t cornerMask = kTileDirection_N | kTileDirection_W |
                    kTileDirection_S | kTileDirection_E;

        Tile& thisTile = tileMap.at(tileY, tileX);
        if (thisTile.layer[1])
        {
            // this tile already has a wall - no need to run tests
            return;
        }

        //printf("at (%u,%u) => ", tileX, tileY);

        uint16_t wallRoleFlags = 0;

        if (tileY > 0)
        {
            const Tile& north = tileMap.at(tileY-1, tileX);
            //printf("north:[%u,%u], ", north.floor, north.wall);
            if (!tileWallsEqual(north, kTileDirection_W, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_E, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_NW, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_NE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_N);
            }
        }
        if (tileX > 0)
        {
            const Tile& west = tileMap.at(tileY, tileX-1);
            //printf("west:[%u,%u], ", west.floor, west.wall);
            if (!tileWallsEqual(west, kTileDirection_N, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_S, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_NW, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_SW, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_W);
            }
        }
        if (tileY < tileMap.rowCount()-1)
        {
            const Tile& south = tileMap.at(tileY+1, tileX);
            //printf("south:[%u,%u], ", south.floor, south.wall);
            if (!tileWallsEqual(south, kTileDirection_W, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_E, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_SW, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_SE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_S);
            }
        }
        if (tileX < tileMap.columnCount()-1)
        {
            const Tile& east = tileMap.at(tileY, tileX+1);
            //printf("east:[%u,%u], ", east.floor, east.wall);
            if (!tileWallsEqual(east, kTileDirection_S, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_N, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_NE, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_SE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_E);
            }
        }

        if (cornerMask & kTileDirection_W)
        {
            if (cornerMask & kTileDirection_N)
                wallRoleFlags |= kTileDirection_NW;
            else  if (cornerMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SW;
        }
        if (!wallRoleFlags && (cornerMask & kTileDirection_N))
        {
            if (cornerMask & kTileDirection_E)
                wallRoleFlags |= kTileDirection_NE;
        }
        if (!wallRoleFlags && (cornerMask & kTileDirection_E))
        {
            if (cornerMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SE;
        }
        //printf("cornerFlags, wallRoleFlags (%04x,%04x)\n",
        //    cornerMask, wallRoleFlags);
        wallRoleFlags |= (kTileRole_Wall+kTileRole_Corner);

        cinek_tile wallTileHandle =
            _tileTemplates.tileHandleFromDescriptor(brush.tileCategoryId,
                                                brush.tileClassId,
                                                wallRoleFlags);

        thisTile.layer[1] = wallTileHandle;
    }

    bool Builder::tileWallsEqual(const Tile& tile, uint16_t roleFlags, uint8_t classId) const
    {
        const cinek_tile_info& wallTemplate = _tileTemplates.tileInfo(tile.layer[1]);
        return wallTemplate.classIndex == classId &&
              (wallTemplate.flags & roleFlags)==roleFlags;
    }

} /* namespace ovengine */ } /* namespace cinekine */
