#pragma once

#include "../../../../libs/clib/clib/time.hpp"

#include "../ERROR.hpp"
#include "../maps.hpp"

using namespace clib;

namespace madlib::trading {

    const map<const string, ms_t> periods = {
        {"1s", MS_PER_SEC},
        {"1m", MS_PER_MIN},
        {"5m", 5 * MS_PER_MIN},
        {"10m", 10 * MS_PER_MIN},
        {"30m", 30 * MS_PER_MIN},
        {"1h", MS_PER_HOUR},
        {"4h", 4 * MS_PER_HOUR},
        {"6h", 6 * MS_PER_HOUR},
        {"12h", 12 * MS_PER_HOUR},
        {"1d", MS_PER_DAY},
        {"3d", 3 * MS_PER_DAY},
        {"4d", 4 * MS_PER_DAY},
        {"1w", MS_PER_WEEK},
        {"2w", 2 * MS_PER_WEEK},
        {"4w", 4 * MS_PER_WEEK},
    };

    ms_t period_to_ms(const string &period) {
        if (period.empty())
            throw ERROR("Missing period");
        if (!map_has(periods, period))
            throw ERROR("Invalid period: " + period);
        return periods.at(period);
    }

    string ms_to_period(ms_t ms) {
        for (const auto& period: periods)
            if (period.second == ms) return period.first;
        throw ERROR("No period string match to the millisec: " + ms);
    }
}