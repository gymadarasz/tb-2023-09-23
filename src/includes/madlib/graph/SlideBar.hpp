#pragma once

namespace madlib::graph {

    class SlideBar: public Area {
    public:

        static void handlerTouch(void* context, unsigned int, int x, int y) {
            SlideBar* that = (SlideBar*)context;
            that->dragStarted = true;
            that->dragStartAt = that->direction == HORIZONTAL ? x : y;
        }

        static void handlerRelease(void* context, unsigned int, int, int) {
            SlideBar* that = (SlideBar*)context;
            that->dragStarted = false;
        }

        static void handlerMove(void* context, int x, int y) {
            SlideBar* that = (SlideBar*)context;
            if (!that->dragStarted) return;

            that->position = (that->direction == HORIZONTAL ? x : y) - that->size / 2;
            if (that->position < 0) that->position = 0;
            if (that->position > that->maxPosition) that->position = that->maxPosition;

            that->direction == HORIZONTAL ? 
                that->handler->setLeft(that->position + 1):
                that->handler->setTop(that->position + 1);

            that->value = that->calcValue();
            that->draw();
        }

    protected:

        bool dragStarted = false;
        // Point<int> dragStartAt = Point<int>(-1, -1);
        int dragStartAt = -1;

        Button* handler = nullptr;

        bool direction;
        int thickness;
        double minValue;
        double maxValue;
        double value;
        int size;
        int maxPosition;
        int position;

        int calcSize() {
            return thickness;
        }

        int calcMaxPosition() {
            return (direction == HORIZONTAL ? width : height) - thickness;
        }

        int calcPosition() {
            return
                (int)((value - minValue) * maxPosition) / 
                (int)(maxValue - minValue);
        }

        double calcValue() {
            return 
                ((maxValue - minValue) * position) / 
                maxPosition;
        }

    public:
        SlideBar(
            GFX& gfx, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, double value = 0,
            int thickness = Theme::defaultBarThickness,
            void* eventContext = nullptr
        ): Area(
            gfx, left, top, 
            direction == HORIZONTAL ? length : thickness,
            direction == VERTICAL ? length : thickness, 
            false, false, "", CENTER, PUSHED, 
            Theme::defaultAreaBackgroundColor,
            Theme::defaultAreaMargin,
            Theme::defaultAreaTextMargin,
            Theme::defaultAreaBorderColor,
            Theme::defaultAreaTextColor,
            eventContext
        ),  
            direction(direction), thickness(thickness),
            minValue(minValue), maxValue(maxValue), value(value),
            size(calcSize()),
            maxPosition(calcMaxPosition()), 
            position(calcPosition())
        {
            handler = direction == HORIZONTAL ?
                new Button(gfx, position, 1, thickness - 2, thickness - 2):
                new Button(gfx, 1, position, thickness - 2, thickness - 2);
            this->child(*handler);
            addTouchHandler(handlerTouch);
            addReleaseHandler(handlerRelease);
            addMoveHandler(handlerMove);
        }

        virtual ~SlideBar() {
            delete handler;
        }

        double getValue() {
            return value;
        }
    };

}