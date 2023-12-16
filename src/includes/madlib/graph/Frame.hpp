#pragma once

#include "Area.hpp"

namespace madlib::graph {

    class Frame: public Area {
    public:

        // bool scrollFixed = false;

        Frame(
            GFX& gfx, 
            int left, int top, 
            int width, int height, 
            bool scrollFixed = false,
            bool zoomFixed = false,
            Border border = Theme::defaultFrameBorder,
            Color backgroundColor = Theme::defaultFrameBackgroundColor,
            void* eventContext = nullptr
        ):
            Area(
                gfx, 
                left, top, 
                width, height, 
                scrollFixed, zoomFixed,
                "", CENTER, 
                border, backgroundColor,
                Theme::defaultAreaMargin,
                Theme::defaultAreaTextMargin,
                Theme::defaultAreaBorderColor,
                Theme::defaultAreaTextColor,
                eventContext
            )
        {
            addTouchHandler(Area::touchHandler);
            addReleaseHandler(Area::releaseHandler);
            addMoveHandler(Area::moveHandler);
            addTouchHandler(Painter::zoomHandler);
        }

        virtual ~Frame() {}
    };

}