#pragma once

#include "../ERROR.hpp"

#include "Theme.hpp"
#include "EventHandler.hpp"
#include "Zoomable.hpp"
#include "Scrollable.hpp"

namespace madlib::graph {

    // TODO: add Resizable class
    class Painter: public Scrollable, public Zoomable, public EventHandler {
    protected:

        static void zoomHandler(void* context, unsigned int button, int x, int y) {
            Painter* that = (Painter*)context;

            // change zoom ratio

            double ratioX;
            double ratioY;
            switch (button) {
                case Theme::zoomInScrollButton: // TODO: disable X and/or Y direction of zoom
                    ratioX = that->getZoomRatio().x * Theme::zoomInRatio;
                    ratioY = that->getZoomRatio().y * Theme::zoomInRatio;
                    if (ratioX > Theme::zoomRatioMax) ratioX = Theme::zoomRatioMax;
                    if (ratioY > Theme::zoomRatioMax) ratioY = Theme::zoomRatioMax;
                    break;
                case Theme::zoomOutScrollButton:
                    ratioX = that->getZoomRatio().x * Theme::zoomOutRatio;
                    ratioY = that->getZoomRatio().y * Theme::zoomOutRatio;
                    if (ratioX < Theme::zoomRatioMin) ratioX = Theme::zoomRatioMin;
                    if (ratioY < Theme::zoomRatioMin) ratioY = Theme::zoomRatioMin;
                    break;
                default:
                    return; // no scroll zoom
            }
            that->setZoomRatio(ratioX, ratioY);
            that->setZoomCenter(x, y);
            // that->draw(); // redraw
        }

    public:

        Painter(
            int width, int height, 
            bool scrollFixed = true,
            bool zoomFixed = true,
            void* eventContext = nullptr
        ): 
            Scrollable(width, height, scrollFixed),
            Zoomable(zoomFixed),
            EventHandler(eventContext)
        {}

        virtual ~Painter() {}

        virtual void brush(Color) const { throw ERR_UNIMP; }
        virtual void point(int, int) { throw ERR_UNIMP; }
        virtual void rect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void fRect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void line(int, int, int, int) { throw ERR_UNIMP; }
        virtual void hLine(int, int, int) { throw ERR_UNIMP; }
        virtual void vLine(int, int, int) { throw ERR_UNIMP; }
        virtual void write(int, int, const string&) { throw ERR_UNIMP; }
        virtual void font(const char*) const { throw ERR_UNIMP; }
        virtual TextSize getTextSize(const string&) const { throw ERR_UNIMP; }
        // virtual int getWidth() const { throw ERR_UNIMP; } // = 0; // { return 0; };
        // virtual int getHeight() const { throw ERR_UNIMP; } // = 0; // { return 0; };

        virtual void draw() = 0;
    };

}