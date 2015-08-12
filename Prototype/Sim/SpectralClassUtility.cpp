//
//  SpectralClassUtility.cpp
//  Overview
//
//  Created by Samir Sinha on 7/11/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "SpectralClassUtility.hpp"

#include "CKGfx/VertexTypes.hpp"

#include <cinek/filestreambuf.hpp>
#include <cinek/debug.hpp>
#include <cinek/vector.hpp>

#include <istream>
#include <cstring>

namespace cinek { namespace ovproto {

struct CSV
{
    CSV() : cursor(nullptr), colidx(0), lineidx(0) {}
    
    CSV& start(char* line)
    {
        cursor = line;
        colidx = 0;
        return *this;
    }
    
    CSV& token(const char** field)
    {
        if (!cursor)
            return *this;
    
        char ch;
        // skip whitespace
        while ((ch = *cursor) && ::isspace(ch))
            ++cursor;
                    
        // run though string until delim or null
        char* start = cursor;
        while ((ch = *cursor) && (ch != ','))
            ++cursor;
                    
        // trim whitespace
        char* end = cursor;
        while (start != end && ::isspace(end[-1]))
            --end;
        
        //  does this range count as a field?
        if (start != end || ch==',')
        {
            ++colidx;
            if (ch == ',')
                ++cursor;
        }
        end[0] = 0;
        *field = start;
        return *this;
    }
    
    CSV& end()
    {
        if (cursor)
        {
            if (colidx > 0)
                ++lineidx;
            cursor = nullptr;
        }
        return *this;
    }
    
    int lineIndex() const { return lineidx; }
    int columnCount() const { return colidx; }

private:
    char* cursor;
    int colidx;
    int lineidx;
};

namespace SpectralUtility
{

struct BCValues
{
    ckm::scalar logT;
    ckm::scalar bc[ovproto::SpectralClass::kSubclassCount];
    bool operator<(const BCValues& r) const {
        return logT > r.logT;
    }
    bool operator<(ckm::scalar r) const {
        return logT > r;
    }
};

struct ColorValue
{
    int temp;
    uint32_t abgr;
    bool operator<(const ColorValue& r) const {
        return temp > r.temp;
    }
    bool operator<(int r) const {
        return temp > r;
    }

};

struct Tables
{
    vector<BCValues> bcTable;
    vector<ColorValue> colorTable;
};

static unique_ptr<Tables> sTables;

bool loadTables()
{
    vector<BCValues> bcValues;
    bcValues.reserve(32);

    FileStreamBuf sb("k_to_bc.csv");
    if (sb)
    {
        CSV csv;
        
        std::istream istr(&sb);
        while (!istr.eof())
        {
            char line[256];
            const char* field[4];
            istr.getline(line, sizeof(line));
        
            csv.start(line)
               .token(&field[0])
               .token(&field[1])
               .token(&field[2])
               .token(&field[3])
               .end();
            
            if (csv.lineIndex() > 1)
            {
                bcValues.emplace_back();
                auto& row = bcValues.back();
                
                row.logT = strtof(field[0], nullptr);
                
                row.bc[SpectralClass::Undefined] = strtof(field[1], nullptr);
                row.bc[SpectralClass::V] = row.bc[SpectralClass::Undefined];
                row.bc[SpectralClass::IV] = row.bc[SpectralClass::V];
                row.bc[SpectralClass::III] = strtof(field[2], nullptr);
                row.bc[SpectralClass::II] = strtof(field[3], nullptr);
                row.bc[SpectralClass::Ib] = row.bc[SpectralClass::II];
                row.bc[SpectralClass::Iab] = row.bc[SpectralClass::Ib];
                row.bc[SpectralClass::Ia] = row.bc[SpectralClass::Iab];
                row.bc[SpectralClass::sd] = 0;
                row.bc[SpectralClass::D] = 0;
            }
        }
        
        //  sort values in descending order
        std::sort(bcValues.begin(), bcValues.end());
    }
    else
    {
        CK_LOG_ERROR("ovproto", "failed to load bolemetric corrections table.");
        return false;
    }
    
    //  min and max values used for interpolation at the extremes
    if (!bcValues.empty())
    {
        bcValues.insert(bcValues.begin(), bcValues.front());
        bcValues.push_back(bcValues.back());
        bcValues.front().logT = 7;
        bcValues.back().logT = 0;
    }
    
    //  load color table
    vector<ColorValue> colorValues;
    colorValues.reserve(64);
    {
        FileStreamBuf sb("k_to_rgb.csv");
        if (sb)
        {
            CSV csv;
            
            std::istream istr(&sb);
            while (!istr.eof())
            {
                char line[256];
                const char* field[4];
                istr.getline(line, sizeof(line));
            
                csv.start(line)
                   .token(&field[0])
                   .token(&field[1])
                   .token(&field[2])
                   .end();
                
                if (csv.lineIndex() > 1)
                {
                    colorValues.emplace_back();
                    auto& row = colorValues.back();
                    
                    row.temp = (int)strtoul(field[0], nullptr, 10);
                    const char* rgbField = field[2];
                    if (rgbField[0] == '#')
                        ++rgbField;
                    uint32_t rgb = (uint32_t)strtoul(rgbField, nullptr, 16);
                    row.abgr = (rgb >> 16) | (rgb & 0x00ff00) | ((rgb & 0xff) << 16);
                    row.abgr |= 0xff000000;
                }
            }
            
            //  sort values in descending order
            std::sort(colorValues.begin(), colorValues.end());
        }
        else
        {
            CK_LOG_ERROR("ovproto", "failed to load star color table.");
            return false;
        }
    }
    
    if (!colorValues.empty())
    {
        //  these are values that are on the extreme ends, and are included
        //  to simplify the associated getter methods
        //  they are not meant to be accurate or usable values.
        colorValues.insert(colorValues.begin(), colorValues.front());
        colorValues.push_back(colorValues.back());
        colorValues.front().temp = 10000000;
        colorValues.front().abgr = 0xffff0000;  // pure blue
        colorValues.back().temp = 1;
        colorValues.back().abgr = 0xff000011;   // low red
    }
    
    Allocator allocator;
    sTables = allocate_unique<Tables>(allocator);
    sTables->bcTable = std::move(bcValues);
    sTables->colorTable = std::move(colorValues);
    
    return true;
}

void unloadTables()
{
    sTables = nullptr;
}

//  returns kSolBolometricMag - 2.5*log10(solarL)
//
ckm::scalar bolometricMagFromLuminosity(ckm::scalar solarL)
{
    return kSolBolometricMag - 2.5 * std::log10(solarL);
}

//  Spectral class based on effective temperature (and stage when we have one.)
//
SpectralClass classFromTemp(int tempK)
{
    return SpectralClass();
}
    
//  lookup based on temperature and Spectral Luminosity Class (I,II,...,V, etc.)
//  using the Schmidt-Kaler Bolometric Correction table (see references)
//
ckm::scalar bolmetricCorrection(int tempK, SpectralClass mkClass)
{
    // lookup value from constants table and interpolate as needed.
    double logT = std::log10(tempK);
    auto& table = sTables->bcTable;
    auto it = std::lower_bound(table.begin(), table.end(), logT);
    if (it == table.end() && it == table.begin())
        return 0;
    
    auto& upper = *(it-1);
    auto& lower = *(it);
    
    SpectralClass::Luminosity lc = mkClass.luminosity();
    auto scalar = lower.bc[lc];
    
    //  linear interpolate
    if (upper.logT != lower.logT)
    {
        auto pct = (logT - lower.logT)/(upper.logT - lower.logT);
        scalar = scalar + (upper.bc[lc] - scalar) * pct;
    }
    return scalar;
}

//  lookup based on temperature and spectral class
//  using the table constructed by Mitchell Charity (see references)
uint32_t colorABGRFromTemp(int tempK)
{
    //  total black
    if (tempK <= 0)
        return 0xff000000;
    
    auto& table = sTables->colorTable;
    
    auto it = std::lower_bound(table.begin(), table.end(), tempK);
    if (it == table.begin())
        return table.front().abgr;
    else if (it == table.end())
        return table.back().abgr;
    
    auto& upper = *(it-1);
    auto& lower = *(it);
    
    auto abgr = lower.abgr;
    
    if (upper.temp != lower.temp)
    {
        gfx::Vector4 colorLow;
        colorLow.fromABGR(lower.abgr);
        
        gfx::Vector4 colorHi;
        colorHi.fromABGR(upper.abgr);
        
        auto pct = (tempK - lower.temp)/((ckm::scalar)(upper.temp - lower.temp));
     
        gfx::Vector4 color;
     
        color.comp[0] = (colorHi.comp[0] - colorLow.comp[0]) * pct;
        color.comp[1] = (colorHi.comp[1] - colorLow.comp[1]) * pct;
        color.comp[2] = (colorHi.comp[2] - colorLow.comp[2]) * pct;
        color.comp[3] = (colorHi.comp[3] - colorLow.comp[3]) * pct;
        color.comp[0] += colorLow.comp[0];
        color.comp[1] += colorLow.comp[1];
        color.comp[2] += colorLow.comp[2];
        color.comp[3] += colorLow.comp[3];
        
        abgr = color.toABGR();
    }
    
    return abgr;
}

ckm::scalar radiusAndLuminosityToEffTemp
(
    ckm::scalar radius,
    ckm::scalar luminosity
)
{
    //  ((R^2)/L) ^ (1/4) = 1/Teff
    //  http://skyserver.sdss.org/dr1/en/proj/advanced/hr/radius1.asp
    auto tempRatio = std::sqrt((radius*radius)/luminosity); // ratio is squared
    tempRatio = std::sqrt(tempRatio);
    tempRatio = 1/tempRatio;
    
    return tempRatio * kSolEffTemp;
}

}

} /* namespace ovproto */ } /* namespace cinek */