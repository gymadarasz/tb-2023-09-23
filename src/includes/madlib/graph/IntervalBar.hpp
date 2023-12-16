#pragma once

namespace madlib::graph {

    class IntervalBar: public ScrollBar {
    public:

        static void handlerMinTouch(void* context, unsigned int, int, int) {
            Button* handlerMin = (Button*)context;
            IntervalBar* that = (IntervalBar*)handlerMin->getParent()->getParent();
            that->dragStarted = false;
            that->dragMinStarted = true;
        }

        static void handlerMaxTouch(void* context, unsigned int, int, int) {
            Button* handlerMax = (Button*)context;
            IntervalBar* that = (IntervalBar*)handlerMax->getParent()->getParent();
            that->dragStarted = false;
            that->dragMaxStarted = true;
        }

        static void handlerRelease(void* context, unsigned int, int, int) {
            IntervalBar* that = (IntervalBar*)context;
            // that->dragStarted = false;
            that->dragMinStarted = false;
            that->dragMaxStarted = false;
        }

        static void handlerMove(void* context, int x, int y) {
            IntervalBar* that = (IntervalBar*)context;
            if (!that->dragMinStarted && !that->dragMaxStarted) return;

            if (x < 0) x = 0;
            if (x > that->width) x = that->width;
            if (y < 0) y = 0;
            if (y > that->height) y = that->height;

            if (that->dragMinStarted) {
                that->position = (that->direction == HORIZONTAL ? x : y) - that->thickness / 6;
                if (that->position < 0) that->position = 0;
                if (that->position > that->maxPosition) that->position = that->maxPosition;

                if (that->direction == HORIZONTAL) {
                    that->handler->setLeft(that->position + 1);
                    int delta = that->dragStartAt - x;
                    that->dragStartAt = x;
                    that->size += delta;
                    if (that->size < that->thickness) that->size = that->thickness;
                    if (that->size > that->width) that->size = that->width;
                    that->handler->setWidth(that->size);
                    that->handlerMax->setLeft(that->handler->getWidth() - that->thickness / 3);
                } else {
                    that->handler->setTop(that->position + 1);
                    int delta = that->dragStartAt - y;
                    that->dragStartAt = y;
                    that->size += delta;
                    if (that->size < that->thickness) that->size = that->thickness;
                    if (that->size > that->height) that->size = that->height;
                    that->handler->setHeight(that->size);
                    that->handlerMax->setTop(that->handler->getHeight() - that->thickness / 3);
                }
            }

            if (that->dragMaxStarted) {
                if (that->direction == HORIZONTAL) {
                    int delta = x - that->dragStartAt;
                    that->dragStartAt = x;
                    that->size += delta;
                    if (that->size < that->thickness) that->size = that->thickness;
                    if (that->size > that->width) that->size = that->width;
                    that->handler->setWidth(that->size);
                    that->handlerMax->setLeft(that->handler->getWidth() - that->thickness / 3);
                } else {
                    int delta = y - that->dragStartAt;
                    that->dragStartAt = y;
                    that->size += delta;
                    if (that->size < that->thickness) that->size = that->thickness;
                    if (that->size > that->height) that->size = that->height;
                    that->handler->setHeight(that->size);
                    that->handlerMax->setTop(that->handler->getHeight() - that->thickness / 3);
                }
            }

            that->maxPosition = that->calcMaxPosition();
            if (that->position < 0) that->position = 0;
            if (that->position > that->maxPosition) that->position = that->maxPosition;

            that->value = that->calcValue();
            // that->valueSize = ... // TODO calcValueSize
            // logger.writeln(that->value, ", ", that->valueSize);
            that->draw();
        }

    protected:
        bool dragMinStarted = false;
        bool dragMaxStarted = false;

        Button* handlerMin = nullptr;
        Button* handlerMax = nullptr;
    public:
        IntervalBar(
            GFX& gfx, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, 
            double value = 0, double valueSize = .3,
            int thickness = Theme::defaultBarThickness
        ): ScrollBar(
            gfx, left, top, length, direction,
            minValue, maxValue, value, valueSize, thickness
        ) 
        {
            handlerMin = direction == HORIZONTAL ?
                new Button(gfx, 0, 0, thickness / 3, thickness):
                new Button(gfx, 0, 0, thickness, thickness / 3);
            handlerMax = direction == HORIZONTAL ?
                new Button(gfx, size - thickness / 3, 0, thickness / 3, thickness):
                new Button(gfx, 0, size - thickness / 3, thickness, thickness / 3);
            handler->child(*handlerMin);
            handler->child(*handlerMax);
            handlerMin->addTouchHandler(handlerMinTouch);
            handlerMax->addTouchHandler(handlerMaxTouch);
            addReleaseHandler(handlerRelease);
            addMoveHandler(handlerMove);
        }

        virtual ~IntervalBar() {
            delete handlerMin;
            delete handlerMax;
        }
    };

}