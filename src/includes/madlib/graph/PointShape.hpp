#pragma once

#include "Shape.hpp"

namespace madlib::graph {
    
    class PointShape: public Shape {
    public:

        PointShape(
            ms_t time, 
            double value
        ):
            Shape(
                TimeRange(time, time), 
                MinMax<double>(value, value)
            )
        {}

        ms_t time() const {
            return timeRange.begin;
        }

        double value() const {
            return valueMinMax.min;
        }
    };

}