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

#include "Engine/Builder/Builder.hpp"

#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Builder/BlockLibrary.hpp"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

namespace cinekine { namespace ovengine {

    static void plotBlockTileOnGridTiled(TileGrid& targetGrid,
                                         const TileGrid& blockGrid,
                                         TileSlot tileSlot,
                                         int xOrigin, int yOrigin,
                                         int xOffset, int yOffset);

    static void plotBlockTileOnGridStretch(TileGrid& targetGrid,
                                          const TileGrid& blockGrid,
                                          TileSlot tileSlot,
                                          int xOrigin, int yOrigin,
                                          int xOffset, int yOffset);


    GridBuilder::GridBuilder(TileGrid& gridMap,
                             const TileLibrary& tileLibrary,
                             const BlockLibrary& blockLibrary,
                             const Allocator& allocator) :
        _allocator(allocator),
        _grid(gridMap),
        _tileTemplates(tileLibrary),
        _blockTemplates(blockLibrary),
        _blockCollection(&_blockTemplates.nullCollection()),
        _tileCollectionSlot(0)
    {
    }
    
    glm::ivec2 GridBuilder::dimensions() const
    {
        return glm::ivec2(_grid.columnCount(), _grid.rowCount());
    }
    
    void GridBuilder::clear()
    {
        _grid.fillWithValue(0x0000, 0, 0, _grid.rowCount(), _grid.columnCount());
    }
    
    void GridBuilder::setBlockCollection(BlockCollectionId id)
    {
        _blockCollection = &_blockTemplates.collectionAtSlot(id);
        _tileCollectionSlot =
            _tileTemplates.slotByCollectionName(_blockCollection->tileCollectionName());
    }

    void GridBuilder::paintRect(const string& blockName,
                                int x0, int y0, int x1, int y1)
    {
        //  select the block
        auto& block = (*_blockCollection)[blockName];

        //  align our rectangle to the block's granularity.  if alignment
        //  results in a rectangle dimension smaller than our granularity, then
        //  abort the paint
        auto granularity = block.granularity();
        if (!granularity)
            return;
        auto xDim = x1 - x0;
        if (xDim < 0)
        {
            std::swap(x0, x1);
            xDim *= -1;
        }
        auto yDim = y1 - y0;
        if (yDim < 0)
        {
            std::swap(y0,y1);
            yDim *= -1;
        }
        if (yDim < granularity || xDim < granularity)
            return;

        auto xBlockUnits = xDim/granularity;
        auto yBlockUnits = yDim/granularity;
        xDim = xBlockUnits * granularity;
        yDim = yBlockUnits * granularity;
        x1 = x0 + xDim;
        y1 = y0 + yDim;

        //  select our source grids (from the block) based on our target
        //  dimensions
        if (xBlockUnits > Block::kClass_Count)
            xBlockUnits = Block::kClass_Count;
        if (yBlockUnits > Block::kClass_Count)
            yBlockUnits = Block::kClass_Count;

        while (xBlockUnits && !block.hasGrid(xBlockUnits-1))
            --xBlockUnits;
        while (yBlockUnits && !block.hasGrid(yBlockUnits-1))
            --yBlockUnits;
        if (!xBlockUnits || !yBlockUnits)
            return;

        //  select the dominant grid (vertical or horizontal bias)
        auto& xBlockGrid = block.grid(xBlockUnits-1);
        auto& yBlockGrid = block.grid(yBlockUnits-1);
        auto blockGrid = &xBlockGrid;
        if (yBlockUnits > xBlockUnits)
            blockGrid = &yBlockGrid;

        PlotFn plotFn = plotFunctionFromStyle(block.paintStyle());
        if (!plotFn)
            return;

        //  clip our rectangle to the target grid bounds
        int xCol = 0, yRow = 0;
        if (x0 < 0)
            xCol = -x0;
        if (x1 >= _grid.columnCount())
            xDim -= (x1 - _grid.columnCount());
        if (y0 < 0)
            yRow = -y0;
        if (y1 >= _grid.rowCount())
            yDim -= (y1 - _grid.rowCount());

        //  paint our rect
        for (auto yRow = 0; yRow < yDim; ++yRow)
        {
            for (auto xCol = 0; xCol < xDim; ++xCol)
            {
                (*plotFn)(_grid, *blockGrid, _tileCollectionSlot, x0, y0, xCol, yRow);
            }
        }

    }

    GridBuilder::PlotFn GridBuilder::plotFunctionFromStyle(BuilderPaintStyle style) const
    {
        switch (style)
        {
        case kBuilderPaintStyle_Tiled:
            return plotBlockTileOnGridTiled;
        case kBuilderPaintStyle_Stretch:
            return plotBlockTileOnGridStretch;
        default:
            break;
        }
        return nullptr;
    }

    void plotBlockTileOnGridTiled(TileGrid& targetGrid,
                                  const TileGrid& blockGrid,
                                  TileSlot tileSlot,
                                  int xOrigin, int yOrigin,
                                  int xOffset, int yOffset)
    {
        auto xBlockTile = xOffset % blockGrid.columnCount();
        auto yBlockTile = yOffset % blockGrid.rowCount();
        TileId srcTileId = compressTileToId(tileSlot,
                                            blockGrid.at(yBlockTile, xBlockTile));

        targetGrid.at(yOrigin + yOffset, xOrigin + xOffset) = srcTileId;
    }

    void plotBlockTileOnGridStretch(TileGrid& targetGrid,
                                    const TileGrid& blockGrid,
                                    TileSlot tileSlot,
                                    int xOrigin, int yOrigin,
                                    int xOffset, int yOffset)
    {

    }

} /* namespace ovengine */ } /* namespace cinekine */
