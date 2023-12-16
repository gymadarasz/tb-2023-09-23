#pragma once

#include "Theme.hpp"

namespace madlib::graph {
    
    class Margin {
    public:

        int top, 
            left, 
            right,
            bottom; 

        Margin(
            int top = Theme::defaultTimeRangeAreMarginTop, 
            int left = Theme::defaultTimeRangeAreMarginLeft, 
            int right = Theme::defaultTimeRangeAreMarginRight,
            int bottom = Theme::defaultTimeRangeAreMarginBottom
        ):
            top(top), 
            left(left), 
            right(right), 
            bottom(bottom)
        {}
    };

}