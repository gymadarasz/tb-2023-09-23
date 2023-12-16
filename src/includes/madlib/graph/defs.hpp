#pragma once

#include "geo.hpp"

namespace madlib::graph {

    struct Viewport: public Rectangle<int> {
        using Rectangle<int>::Rectangle;
    };

    struct Coord: public Point<double> {
        using Point<double>::Point;
    };

    struct Pixel: public Point<int> {
        using Point<int>::Point;
    };
    
    enum Align {
        CENTER,
        LEFT,
        RIGHT,
    };

    enum Border {
        NONE,
        SIMPLE,
        RELEASED,
        PUSHED,
    };

    typedef struct { int width = 0, height = 0; } TextSize;

    enum Direction { HORIZONTAL, VERTICAL };

}