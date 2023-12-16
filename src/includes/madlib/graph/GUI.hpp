#pragma once

#include "Area.hpp"

namespace madlib::graph {

    class GUI: public Area {
    protected:

        static void resizeHandler(void* context, int width, int height) {
            GUI* that = (GUI*)(context);
            that->width = width;
            that-> height = height;
            that->draw();
        }

        static void touchHandler(void* context, unsigned int button, int x, int y) {
            Area* that = (Area*)(context);
            that->propagateTouch(button, x, y);
        }

        static void releaseHandler(void* context, unsigned int button, int x, int y) {
            Area* that = (Area*)(context);
            that->propagateRelease(button, x, y);
        }

        static void moveHandler(void* context, int x, int y) {
            Area* that = (Area*)(context);
            that->propagateMove(x, y);
        }

        void init(
            int width, int height, 
            const char* title = Theme::defaultWindowTitle, 
            Color color = Theme::defaultWindowColor
        ) {
            gfx.openWindow(width, height, title, color);
            gfx.setEventContext(this);
            gfx.addResizeHandler(resizeHandler);
            gfx.addTouchHandler(touchHandler);
            gfx.addReleaseHandler(releaseHandler);
            gfx.addMoveHandler(moveHandler);
        }

    public:
        GUI(
            GFX& gfx, 
            int width, int height,
            const char* title = Theme::defaultWindowTitle, 
            Color color = Theme::defaultWindowColor,
            void* eventContext = nullptr
        ):
            Area(
                gfx, 0, 0, width, height, false, false, "",
                Theme::defaultAreaTextAlign,
                Theme::defaultAreaBorder,
                Theme::defaultAreaBackgroundColor,
                Theme::defaultAreaMargin,
                Theme::defaultAreaTextMargin,
                Theme::defaultAreaBorderColor,
                Theme::defaultAreaTextColor,
                eventContext
            )
        {
            init(width, height, title, color);
        }

        void close() {
            gfx.closeWindow();
        }

        void loop(unsigned long ms = Theme::defaultGFXEventLoopMs) const {
            gfx.eventLoop(ms);
        }

        void setTitle(const char* title) {
            gfx.setWindowTitle(title);
        }
    };

}