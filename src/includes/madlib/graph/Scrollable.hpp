#pragma once

#include "defs.hpp"

namespace madlib::graph {

    class Scrollable {
    protected:

        int scrollX = 0, scrollY = 0,
            scrollXMin = 0, scrollYMin = 0,
            scrollXMax = 0, scrollYMax = 0;


        int width, height;
        bool scrollFixedX;
        bool scrollFixedY;

    public:

        Scrollable(
            int width, int height, 
            bool scrollFixed = true
        ):
            width(width), height(height), 
            scrollFixedX(scrollFixed),
            scrollFixedY(scrollFixed)
        {}

        virtual ~Scrollable() {}

        void setScrollFixedX(bool scrollFixedX) {
            this->scrollFixedX = scrollFixedX;
        }

        bool isScrollFixedX() const {
            return scrollFixedX;
        }

        void setScrollFixedY(bool scrollFixedY) {
            this->scrollFixedY = scrollFixedY;
        }

        bool isScrollFixedY() const {
            return scrollFixedY;
        }

        void setScrollFixed(bool scrollFixed) {
            scrollFixedX = scrollFixed;
            scrollFixedY = scrollFixed;
        }

        int getScrollX() const {
            return scrollX;
        }

        int getScrollY() const {
            return scrollY;
        }

        int getScrollXMin() const {
            return scrollXMin;
        }

        int getScrollXMax() const {
            return scrollXMax;
        }

        int getScrollYMin() const {
            return scrollYMin;
        }

        int getScrollYMax() const {
            return scrollYMax;
        }

        void resetScrollXYMax() {
            scrollXMax = 0;
            scrollYMax = 0;
        }

        void setWidth(int width) {
            this->width = width;
        }

        int getWidth() const {
            return width;
        }

        void setHeight(int height) {
            this->height = height;
        }

        int getHeight() const {
            return height;
        }

        void forceScrollInRange() {
            if (scrollX < scrollXMin) 
                scrollX = scrollXMin;
            if (scrollY < scrollYMin) 
                scrollY = scrollYMin;
            if (scrollX > scrollXMax) 
                scrollX = scrollXMax;
            if (scrollY > scrollYMax) 
                scrollY = scrollYMax;
        }

        void setScrollXYMin(int x, int y, bool forceInRange = true) {
            int xMin = x;
            int yMin = y;
            if (xMin < scrollXMin) 
                scrollXMin = xMin;
            if (yMin < scrollYMin) 
                scrollYMin = yMin;
            if (forceInRange) forceScrollInRange();
        }
        
        void setScrollXYMax(int x, int y, bool forceInRange = true) {
            int xMax = x - width;
            int yMax = y - height;
            if (xMax > scrollXMax) 
                scrollXMax = xMax;
            if (yMax > scrollYMax) 
                scrollYMax = yMax;
            if (forceInRange) forceScrollInRange();
        }

        void setScrollXY12MinMax(int x1, int y1, int x2, int y2, bool forceInRange = true) {
            setScrollXYMin(x1, y1, false);
            setScrollXYMax(x2, y2, false);
            if (forceInRange) forceScrollInRange();
        }
        
        void setScrollXYMinMax(int x, int y, bool forceInRange = true) {
            setScrollXYMin(x, y, false);
            setScrollXYMax(x, y, false);
            if (forceInRange) forceScrollInRange();
        }
        
        void setScrollXY(int x, int y) {
            if (!scrollFixedX) scrollX = x;
            if (!scrollFixedY) scrollY = y;
            forceScrollInRange();
        }

        void moveScrollXY(int x, int y) {
            if (!scrollFixedX) scrollX += x;
            if (!scrollFixedY) scrollY += y;
            forceScrollInRange();
        }

        Point<int> getScrollXY() const {
            return Point<int>(scrollX, scrollY);
        }
    };

}