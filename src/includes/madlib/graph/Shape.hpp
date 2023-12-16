#pragma once

#include "geo.hpp"

namespace madlib::graph {

    class Shape {
    protected:

        const TimeRange timeRange;
        const MinMax<double> valueMinMax;

    public:

        Shape(
            const TimeRange timeRange,
            const MinMax<double> valueMinMax
        ):
            timeRange(timeRange),
            valueMinMax(valueMinMax)
        {}

        const TimeRange& getTimeRange() const {
            return timeRange;
        }

        const MinMax<double> getValueMinMax() const {
            return valueMinMax;
        }
    };

}