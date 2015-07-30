//
//  Diagnostics.cpp
//  Overview
//
//  Created by Samir Sinha on 7/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Diagnostics.hpp"

#include <cinek/debug.hpp>

namespace cinek { namespace ovproto {


Diagnostics::Diagnostics() :
    _currentTimeMs(0)
{
    _intervalTimeMs[kInterval_Second] = 1000;
    _intervalTimeMs[kInterval_QuarterMinute] = 15*1000;
    _intervalTimeMs[kInterval_Minute] = 60*1000;
    
    memset(_intervalStartTimeMs, 0, sizeof(_intervalStartTimeMs));
    memset(_intervalCount, 0, sizeof(_intervalCount));
    memset(_FPS, 0, sizeof(_FPS));
    memset(_totals, 0, sizeof(_totals));
}

uint32_t Diagnostics::valueGaugeAmount(ValueGauge gauge) const
{
    CK_ASSERT_RETURN_VALUE(gauge < kValueCount, 0);
    return _totals[gauge];
}

ckm::scalar Diagnostics::rateGaugeAmount(RateGauge gauge, Interval interval) const
{
    CK_ASSERT_RETURN_VALUE(gauge < kRateGaugeCount && interval < kIntervalCount, 0);
    return _FPS[gauge][interval];
}

void Diagnostics::incrementRateGauge(RateGauge gauge)
{
    CK_ASSERT_RETURN(gauge < kRateGaugeCount);
    auto& rateGauge = _intervalCount[gauge];
    for (auto i = 0; i < kIntervalCount; ++i)
    {
        ++rateGauge[i];
    }
}

void Diagnostics::setValueGauge(ValueGauge gauge, uint32_t value)
{
    CK_ASSERT_RETURN(gauge < kValueCount);
    ++_totals[gauge];
}

void Diagnostics::updateTime(uint32_t timeMs)
{
    _currentTimeMs = timeMs;
    for (auto i = 0; i < kIntervalCount; ++i)
    {
        if (_currentTimeMs - _intervalStartTimeMs[i] > _intervalTimeMs[i])
        {
            ckm::scalar div = _intervalTimeMs[i] / (ckm::scalar)1000;
            for (auto j = 0; j < kRateGaugeCount; ++j)
            {
                _FPS[j][i] = _intervalCount[j][i] / div;
                _intervalCount[j][i] = 0;
            }
            _intervalStartTimeMs[i] = _currentTimeMs;
        }
    }
}


} /* namespace ovproto */ } /* namespace cinek */