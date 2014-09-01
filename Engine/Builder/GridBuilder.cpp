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

#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/Block.hpp"

#include <algorithm>
#include <glm/glm.hpp>

namespace cinekine { namespace ovengine {

    inline void plotBlockTileOnMap(TileGrid& mapGrid,
                                   TileSlot tileSlot, int tileX, int tileY,
                                   const Block::Grid& blockGrid, int blockX, int blockY,
                                   int blockCntX, int blockCntY)
    {
        for (int iy = 0; iy < blockCntY; ++iy)
        {
            for (int ix = 0; ix < blockCntX; ++ix)
            {
                TileId srcTileId = compressTileToId(tileSlot,
                                                    blockGrid.at(blockY+iy, blockX+ix));
                mapGrid.at(tileY+iy, tileX+ix) = srcTileId;
            }
        }
    }


    GridBuilder::GridBuilder(TileGrid& gridMap, int tileCoordRatio) :
        _grid(&gridMap),
        _tileCoordRatio(tileCoordRatio)
    {
    }

    glm::ivec2 GridBuilder::dimensions() const
    {
        return glm::ivec2(_grid->columnCount() * _tileCoordRatio,
                          _grid->rowCount() * _tileCoordRatio);
    }

    void GridBuilder::clear()
    {
        _grid->fillWithValue(0x0000, 0, 0, _grid->rowCount(), _grid->columnCount());
    }

    void GridBuilder::fillBox(const Block& block,
                              TileSlot tileCollectionSlot,
                              const glm::ivec2& mapPoint,  const glm::ivec2& mapDims)
    {
        auto tileX = mapPoint.x/_tileCoordRatio;
        auto tileY = mapPoint.y/_tileCoordRatio;
        auto unitsX = mapDims.x/(_tileCoordRatio * block.granularity());
        auto unitsY = mapDims.y/(_tileCoordRatio * block.granularity());
        if (!unitsX || !unitsY)
            return;

        //  choose the grid with the greatest area
        auto& blockGrid = block.findLargestGridByDimensions(std::abs(unitsX),
                                                            std::abs(unitsY));
        if (!blockGrid)
            return;

        auto granularity = block.granularity();
        if (!granularity)
            return;
        auto xDim = unitsX * granularity;
        auto yDim = unitsY * granularity;
        auto tileXEnd = tileX + xDim;
        auto tileYEnd = tileY + yDim;

        //  normalize coords so that X, Y is "upper left", and XEnd, YEnd is
        //  "lower right"
        if (tileXEnd < tileX)
            std::swap(tileX, tileXEnd);
        if (tileYEnd < tileY)
            std::swap(tileY, tileYEnd);

        //  clip our rectangle to the target grid bounds
        int xCol = 0, yRow = 0;
        if (tileX < 0)
            xCol = -tileX;
        if (tileXEnd >= _grid->columnCount())
            xDim -= (tileXEnd - _grid->columnCount());
        if (tileY < 0)
            yRow = -tileY;
        if (tileYEnd >= _grid->rowCount())
            yDim -= (tileYEnd - _grid->rowCount());

        if (block.paintStyle() == kBuilderPaintStyle_Tiled)
        {
            //  paint our rect
            for (auto yRow = 0; yRow < yDim; ++yRow)
            {
                for (auto xCol = 0; xCol < xDim; ++xCol)
                {
                    plotBlockTileOnMap(*_grid,
                                       tileCollectionSlot,
                                       tileX + xCol, tileY + yRow,
                                       blockGrid,
                                       xCol % blockGrid.columnCount(), yRow % blockGrid.rowCount(),
                                       1, 1);
                }
            }
        }
        else if (block.paintStyle() == kBuilderPaintStyle_Stretch)
        {

        }

    }

    void GridBuilder::drawLine(const Block& block, BlockSideType blockSide,
                               TileSlot tileSlot,
                               DrawDirection drawDirection,
                               const glm::ivec2& mapPoint, int mapLineOffset)
    {
        auto mapLineUnits = mapLineOffset/(_tileCoordRatio * block.granularity());

        // Select source Block::Grid using direction and mapLineOffset
        // Obtain tile from source Block::Grid
        //  Tiled Case:
        //   mapLineOffset % Block::Grid::columnCount()
        //  Stretch Case:
        //   srcGridTileOffset % Block::Grid::columnCount()
        //
        glm::ivec2 destTilePos = mapPoint;
        destTilePos /= _tileCoordRatio;

        glm::ivec2 destTilePosEnd;
        glm::ivec2 tilePosIncr;

        switch (drawDirection)
        {
        case kDrawDirection_Horizontal:
            destTilePosEnd.x = mapLineUnits * block.granularity();
            tilePosIncr.x = 1;
            break;
        case kDrawDirection_Vertical:
            destTilePosEnd.y = mapLineUnits * block.granularity();
            tilePosIncr.y = 1;
            break;
        }

        destTilePosEnd += destTilePos;

        //  flip our start and end points so that the algorithm only needs to
        //  handle a single "left to right, top to bottom" case
        if (mapLineUnits < 0)
        {
            std::swap(destTilePos, destTilePosEnd);
            // necessary since our destEnd always points to the tile ahead of
            // the last plotted tile
            destTilePos += tilePosIncr;
            destTilePosEnd += tilePosIncr;
        }

        //  determine start position within source block from which to copy
        //  tiles to the target, based on the blockSide and border.
        auto destTileDims = destTilePosEnd - destTilePos;
        auto& srcGrid = block.findLargestGridByDimensions(destTileDims.x/block.granularity(),
                                                          destTileDims.y/block.granularity());
        glm::ivec2 srcTilePos, srcTileDim;
        int blockBorder = block.border() ? block.border() : 1;

        switch (drawDirection)
        {
        case kDrawDirection_Horizontal:
            if (blockSide == kBlockSide_Bottom)
                srcTilePos.y = srcGrid.rowCount() - blockBorder;
            else
                srcTilePos.y = 0;
            srcTileDim.x = 1;
            srcTileDim.y = blockBorder;
            break;
        case kDrawDirection_Vertical:
            if (blockSide == kBlockSide_Right)
                srcTilePos.x = srcGrid.columnCount() - blockBorder;
            else
                srcTilePos.x = 0;
            srcTileDim.x = blockBorder;
            srcTileDim.y = 1;
            break;
        }

        //  draw from mapPoint start to end
        //

        //  >>> used for stretch mode only
        enum {
            kSrcTileStartUnit,
            kSrcTileStretchUnit,
            kSrcTileEndUnit,
            kSrcTileCompleteUnit
        }
        srcTileUnitStage = kSrcTileStartUnit;
        glm::ivec2 destTilePosStageEnd = tilePosIncr * block.granularity();
        glm::ivec2 srcTilePosStageEnd = tilePosIncr * block.granularity();
        destTilePosStageEnd += destTilePos;
        //  <<<

        glm::ivec2 srcTilePosOffset;

        while (destTilePos.x < destTilePosEnd.x ||
               destTilePos.y < destTilePosEnd.y)
        {
            //  least efficient way to clip, but our logic for calculating
            //  srcGrid coordinates is rather complex - clipping both dest and
            //  source to avoid edge cases seems unnecessary
            if (destTilePos.x >= 0 && destTilePos.x < _grid->columnCount() &&
                destTilePos.y >= 0 && destTilePos.y < _grid->rowCount())
            {
                //  plot tiles up to the border edge
                plotBlockTileOnMap(*_grid, tileSlot,
                                   destTilePos.x, destTilePos.y,
                                   srcGrid,
                                   (srcTilePos.x + srcTilePosOffset.x) % srcGrid.columnCount(),
                                   (srcTilePos.y + srcTilePosOffset.y) % srcGrid.rowCount(),
                                   srcTileDim.x, srcTileDim.y);
            }

            srcTilePosOffset += tilePosIncr;
            destTilePos += tilePosIncr;

            if (block.paintStyle() == kBuilderPaintStyle_Stretch)
            {
                while (destTilePos == destTilePosStageEnd)
                {
                    srcTilePos.x = srcTilePosStageEnd.x % srcGrid.columnCount();
                    srcTilePos.y = srcTilePosStageEnd.y % srcGrid.rowCount();
                    srcTilePosOffset.x = 0;
                    srcTilePosOffset.y = 0;
                    if (srcTileUnitStage == kSrcTileStartUnit)
                    {
                        srcTileUnitStage = kSrcTileStretchUnit;

                        destTilePosStageEnd = destTilePosEnd - (tilePosIncr * block.granularity());
                        srcTilePosStageEnd.x = std::max(tilePosIncr.x * block.granularity(),
                                                        (int)srcGrid.columnCount() - tilePosIncr.x * block.granularity());
                        srcTilePosStageEnd.y = std::max(tilePosIncr.x * block.granularity(),
                                                        (int)srcGrid.rowCount() - tilePosIncr.y * block.granularity());
                    }
                    else if (srcTileUnitStage == kSrcTileStretchUnit)
                    {
                        srcTileUnitStage = kSrcTileEndUnit;
                        destTilePosStageEnd = destTilePosEnd;
                        srcTilePosStageEnd.x = srcGrid.columnCount();
                        srcTilePosStageEnd.y = srcGrid.rowCount();
                    }
                    else if (srcTileUnitStage == kSrcTileEndUnit)
                    {
                        srcTileUnitStage = kSrcTileCompleteUnit;
                        break;
                    }
                }
            }
        }

    }


} /* namespace ovengine */ } /* namespace cinekine */
