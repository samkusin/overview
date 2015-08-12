//
//  Diagnostics.hpp
//  Overview
//
//  Created by Samir Sinha on 7/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Client_Diagnostics_hpp
#define Overview_App_Client_Diagnostics_hpp

#include "GameTypes.hpp"

namespace cinek { namespace ovproto {

class Diagnostics
{
public:
    enum Interval
    {
        kInterval_Second,
        kInterval_QuarterMinute,
        kInterval_Minute,
        kIntervalCount
    };
    
    enum RateGauge
    {
        kFrameRate_Main,
        kFrameRate_Update,
        kRateGaugeCount
    };
    
    enum ValueGauge
    {
        kValue_UIItems,
        kValueCount
    };
    
    Diagnostics();
    
    void updateTime(uint32_t renderTimeMs, uint32_t simTimeMs);
    void incrementRateGauge(RateGauge gauge);
    void setValueGauge(ValueGauge gauge, uint32_t value);
    
    uint32_t valueGaugeAmount(ValueGauge) const;
    ckm::scalar rateGaugeAmount(RateGauge, Interval) const;
    uint32_t currentTimeInMs() const { return _currentTimeMs; }
    uint32_t simTimeInMs() const { return _currentSimTimeMs; }

private:
    uint32_t _currentTimeMs;
    uint32_t _currentSimTimeMs;
    
    //  gauges averaged over time
    uint32_t _intervalStartTimeMs[kIntervalCount];
    uint32_t _intervalTimeMs[kIntervalCount];
    uint32_t _intervalCount[kRateGaugeCount][kIntervalCount];
    ckm::scalar _FPS[kRateGaugeCount][kIntervalCount];
    
    //  real-time gauges
    uint32_t _totals[kValueCount];
};

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Client_Diagnostics_hpp) */
