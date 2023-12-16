#pragma once

#include "Area.hpp"

namespace madlib::graph {

    class Label: public Area {
    public:
        Label(GFX& gfx, int left, int top, int width, int height,
            const string &text, 
            const Align textAlign = Theme::defaultLabelTextAlign,
            const Border border = Theme::defaultLabelBorder,
            void* eventContext = nullptr
        ): Area(
            gfx, left, top, width, height, false, false, text, textAlign, border, 
            Theme::defaultAreaBackgroundColor,
            Theme::defaultAreaMargin,
            Theme::defaultAreaTextMargin,
            Theme::defaultAreaBorderColor,
            Theme::defaultAreaTextColor,
            eventContext
        )
        {}
    };

}