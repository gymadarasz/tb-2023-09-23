#pragma once

#include "SlideBar.hpp"

namespace madlib::graph {

    class ScrollBar: public SlideBar {
    protected:
        double valueSize;

        int calcSize() {
            int size = (
                (int)(valueSize * (direction == HORIZONTAL ? width : height)) /
                (int)(maxValue - minValue)
             ) - 1;
            if (size < thickness) size = thickness;
            return size;
        }


        int calcMaxPosition() {
            return (direction == HORIZONTAL ? width : height) - size - 2;
        }

    public:
        ScrollBar(
            GFX& gfx, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, 
            double value = 0, double valueSize = .3,
            int thickness = Theme::defaultBarThickness,
            void* eventContext = nullptr
        ): SlideBar(
            gfx, left, top, length, direction, 
            minValue, maxValue, value, thickness, 
            eventContext
        ), valueSize(valueSize)
        {
            this->size = calcSize();
            this->maxPosition = calcMaxPosition();
            handler->setWidth(direction == HORIZONTAL ? size : thickness - 2);
            handler->setHeight(direction == VERTICAL ? size : thickness - 2);
        }

        virtual ~ScrollBar() {}

        double getValueSize() {
            return valueSize;
        }
    };

}