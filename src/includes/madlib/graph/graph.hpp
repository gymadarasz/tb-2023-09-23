#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <stdexcept>
#include <vector>

#include "../madlib.hpp"

using namespace std;
using namespace madlib;

namespace madlib::graph {

    typedef unsigned long Color;

    const Color black = 0x000000;
    const Color white = 0xFFFFFF;
    const Color gray = 0xAAAAAA;
    const Color darkGray = 0x666666;
    const Color red = 0xAA0000;
    const Color green = 0x00AA00;
    const Color blue = 0x0000AA;
    const Color lightRed = 0xFF0000;
    const Color lightGreen = 0x00FF00;
    const Color lightBlue = 0x0000FF;
    const Color cyan = 0x00AAAA;
    const Color purple = 0xAA00AA;
    const Color orange = 0xAAAA00;
    const Color lightCyan = 0x00FFFF;
    const Color lightPurple = 0xFF00FF;
    const Color yellow = 0xFFFF00;

    class ColorMixer {
    public:

        static Color light(Color color) {
            switch (color) {
                case black:
                    return white;
                case white:
                    return white;
                case gray:
                    return white;
                case darkGray:
                    return white;
                case red:
                    return lightRed;
                case green:
                    return lightGreen;
                case blue:
                    return lightBlue;
                case lightRed:
                    return white;
                case lightGreen:
                    return white;
                case lightBlue:
                    return white;
                case cyan:
                    return lightCyan;
                case purple:
                    return lightPurple;
                case orange:
                    return yellow;
                case lightCyan:
                    return white;
                case lightPurple:
                    return white;
                case yellow:
                    return white;
                default:
                    throw ERROR("Invalid color");                    
            }
        }

        static Color dark(Color color) {
            switch (color) {
                case black:
                    return black;
                case white:
                    return black;
                case gray:
                    return black;
                case darkGray:
                    return black;
                case red:
                    return black;
                case green:
                    return black;
                case blue:
                    return black;
                case lightRed:
                    return red;
                case lightGreen:
                    return green;
                case lightBlue:
                    return blue;
                case cyan:
                    return black;
                case purple:
                    return black;
                case orange:
                    return black;
                case lightCyan:
                    return cyan;
                case lightPurple:
                    return purple;
                case yellow:
                    return orange;
                default:
                    throw ERROR("Invalid color");                    
            }
        }
    };
    
    enum Align {
        CENTER,
        LEFT,
        RIGHT,
    };

    enum Border {
        NONE,
        SIMPLE,
        BUTTON_RELEASED,
        BUTTON_PUSHED,
    };

    struct Theme {
        static const unsigned long eventLoopMs = 100;
        static const Color windowColor = gray;
        static const char* windowTitle;
        static const char* windowFont;
        static const Align textAlign = CENTER;
        static const Color borderColor = gray;
        static const Color textColor = black;
        //static const Color backgroundColor = gray;
        static const Color frameBackgroundColor = darkGray;
        static const int frameMargin = 10;
        static const int textPadding = 10;
        static const Align labelTextAlign = LEFT;
    };
    const char* Theme::windowTitle = "graph";
    const char* Theme::windowFont = "10x20";

    class EventHandler {
    public:

        void* eventContext = NULL;

        typedef void (*onResizeHandler)(void*, int, int);
        typedef void (*onKeyPressHandler)(void*, unsigned long);
        typedef void (*onKeyReleaseHandler)(void*, unsigned long);
        typedef void (*onTouchHandler)(void*, unsigned int, int, int);
        typedef void (*onReleaseHandler)(void*, unsigned int, int, int);
        typedef void (*onMoveHandler)(void*, int, int);
        typedef void (*onLoopHandler)(void*);
        
        vector<onResizeHandler> onResizeHandlers;
        vector<onKeyPressHandler> onKeyPressHandlers;
        vector<onKeyReleaseHandler> onKeyReleaseHandlers;
        vector<onTouchHandler> onTouchHandlers;
        vector<onReleaseHandler> onReleaseHandlers;
        vector<onMoveHandler> onMoveHandlers;
        vector<onLoopHandler> onLoopHandlers;

        void setEventContext(void* eventContext) {
            this->eventContext = eventContext;
        }

        void* getEventContext() const {
            return eventContext;
        }
    };

    struct Rectangle {
    protected:

    public:

        int x1, y1, x2, y2;

        Rectangle(int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0):
            x1(x1), y1(y1), x2(x2), y2(y2)
        {
            fix();
        }

        void set(int x1, int y1, int x2, int y2) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
            fix();
        }

        void fix() {
            if (this->x1 > this->x2) swap(this->x1, this->x2);
            if (this->y1 > this->y2) swap(this->y1, this->y2);
        }

        bool intersect(int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0) {
            // Calculate the intersection of the current viewport with the given viewport
            if (this->x1 < x1) this->x1 = x1;
            if (this->y1 < y1) this->y1 = y1;
            if (this->x2 > x2) this->x2 = x2;
            if (this->y2 > y2) this->y2 = y2;

            // Check if the resulting viewport is still valid (non-empty)
            if (this->x1 > this->x2 || this->y1 > this->y2) {
                // The viewports do not intersect, so set them to an empty viewport
                this->x1 = this->y1 = this->x2 = this->y2 = 0;
                return false;
            }

            return true;
        }

        bool intersect(const Rectangle& other) {
            return intersect(other.x1, other.y1, other.x2, other.y2);
        }

        bool insideOf(int x1, int y1, int x2, int y2) const {
            return (this->x1 >= x1 && this->y1 >= y1 && this->x2 <= x2 && this->y2 <= y2);
        }

        bool insideOf(const Rectangle& other) const {
            return insideOf(other.x1, other.y1, other.x2, other.y2);
        }

        bool containsCompletely(int x1, int y1, int x2, int y2) const {
            // Check if 'other' rectangle is completely inside 'this' rectangle
            return (x1 >= this->x1 && y1 >= this->y1 && x2 <= this->x2 && y2 <= this->y2);
        }

        bool containsCompletely(const Rectangle& other) const {
            // Check if 'other' rectangle is completely inside 'this' rectangle
            return containsCompletely(other.x1, other.y1, other.x2, other.y2);
        }

        bool containsPartially(int x1, int y1, int x2, int y2) const {
            // Check if 'other' rectangle partially overlaps with 'this' rectangle
            return !(x2 < this->x1 || x1 > this->x2 || y2 < this->y1 || y1 > this->y2);
        }

        bool containsPartially(const Rectangle& other) const {
            // Check if 'other' rectangle partially overlaps with 'this' rectangle
            return containsPartially(other.x1, other.y1, other.x2, other.y2);
        }
    };

    class GFX: public EventHandler {
    public:

        static const unsigned long defaultLoopMs = Theme::eventLoopMs;
        static const Color defaultWindowColor = Theme::windowColor;
        static const char* defaultWindowTitle;
        static const char* defaultWindowFont;

    protected:

        static Display *display;
        Window window;
        GC gc;
        XFontStruct *fontInfo = NULL;

        Rectangle viewport;

        void* context = NULL;

    public:

        bool close = false;

        GFX(void* context = NULL): context(context) {}

        void* getContext() const {
            return context;
        }

        void setColor(Color color) const {
            XSetForeground(display, gc, color);
        }

        void setViewport(Rectangle viewport) {
            this->viewport = viewport;
        }

        void openWindow(int width, int height, const char* title = defaultWindowTitle, Color color = defaultWindowColor, const char* font = defaultWindowFont) {
            // Initialize the X display
            if (!display) display = XOpenDisplay(NULL);
            if (!display) throw ERROR("Unable to open display.");

            // int screen = DefaultScreen(display);

            // Create the window
            window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, (unsigned)width, (unsigned)height, 1, color, color);
            XMapWindow(display, window);

            // Set the window title
            XStoreName(display, window, title);

            // Create a graphics context
            gc = XCreateGC(display, window, 0, NULL);

            // Select inputs
            XSelectInput(display, window, 
                ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
                ButtonReleaseMask | PointerMotionMask);

            setFont(font);
            setColor(color);
            clearWindow();

            eventContext = this;
        }

        void closeWindow(bool closeDisplay = true) const {            
            if (fontInfo) XFreeFont(display, fontInfo);
            XFreeGC(display, gc);
            XDestroyWindow(display, window);
            if (closeDisplay) XCloseDisplay(display);
        }

        void getWindowSize(int &width, int &height) const {
            XWindowAttributes attr;
            XGetWindowAttributes(display, window, &attr);
            width = attr.width;
            height = attr.height;
        }

        void clearWindow() const {
            int width, height;
            getWindowSize(width, height);
            XFillRectangle(display, window, gc, 0, 0, (unsigned)width, (unsigned)height);
        }

        void setWindowTitle(const char* title) const {
            XTextProperty prop;
            XStringListToTextProperty(const_cast<char**>(&title), 1, &prop);
            XSetWMName(display, window, &prop);
            XFree(prop.value);
            // XFlush(display);  // Flush the changes to the server
        }

        void drawPoint(int x, int y) const {
            if (viewport.containsCompletely(x, y, x, y))
                XDrawPoint(display, window, gc, x, y);
        }

        void drawRectangle(int x1, int y1, int x2, int y2) const {
            Rectangle rect(x1, y1, x2, y2);
            if (rect.insideOf(viewport)) {
                XDrawRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
                return;
            }
            if (rect.containsPartially(x1, y1, x2, y1)) drawHorizontalLine(x1, y1, x2);
            if (rect.containsPartially(x1, y2, x2, y2)) drawHorizontalLine(x1, y2, x2);
            if (rect.containsPartially(x2, y1, x2, y2)) drawVerticalLine(x2, y1, y2);
            if (rect.containsPartially(x1, y1, x1, y2)) drawVerticalLine(x1, y1, y2);
        }

        void fillRectangle(int x1, int y1, int x2, int y2) const {
            Rectangle rect(x1, y1, x2, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XFillRectangle(display, window, gc, rect.x1, rect.y1, (unsigned)(rect.x2 - rect.x1), (unsigned)(rect.y2 - rect.y1));
        }

        void drawLine(int x1, int y1, int x2, int y2) const {
            Rectangle rect(x1, y1, x2, y2);
            
            if (x1 == x2) {
                drawVerticalLine(x1, y1, y2);
                return;
            }
            
            if (y1 == y2) {
                drawHorizontalLine(x1, y1, x2);
                return;
            }

            if (
                (x1 > viewport.x2 && x2 > viewport.x2) ||
                (x1 < viewport.x1 && x2 < viewport.x1) ||
                (y1 > viewport.y2 && y2 > viewport.y2) ||
                (y1 < viewport.y1 && y2 < viewport.y1)
            ) {
                return;
            }

            // TODO: this part will need only when calculating the edge
            // // Fix the line first, it's always goes left to right
            // if (x1 > x2) {
            //     swap(x1, x2);
            //     swap(y1, y2);
            //     rect = Rectangle(x1, y1, x2, y2);
            // }

            if (!viewport.containsCompletely(rect)) {
                // TODO: cut the line at the edge of viewport
                return;
            }

            // Draw the clipped line
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void drawVerticalLine(int x1, int y1, int y2) const {
            Rectangle rect(x1, y1, x1, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x1, rect.y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2) const {
            Rectangle rect(x1, y1, x2, y1);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x2, rect.y1);
        }

        void setFont(const char* font) {            
            fontInfo = XLoadQueryFont(display, font);
            if (!fontInfo)
            {
                LOG("Unable to load font: %s\n", font);
                throw ERROR("Unable to load font.\n");
            }
            XSetFont(display, gc, fontInfo->fid);
        }
        
        void writeText(int x, int y, const string& text) const {
            // Cut text to fit into the viewport first
            string txt = text;            
            while (!txt.empty()) {
                int width, height;
                getTextSize(txt, width, height);
                // const int textYFixer4 = 0; //4; // ??4
                int x1 = x;
                int y1 = y; // + textYFixer4;
                int x2 = x + width;
                int y2 = y - height; // + textYFixer4;
                if (x1 > x2) swap(x1, x2);
                if (y1 > y2) swap(y1, y2);
                
                if (y1 < viewport.y1 || y2 > viewport.y2) return;
                if (x1 < viewport.x1) {
                    // Extract the first character.
                    getTextSize(txt.substr(0, 1), width, height); 
                    x += width;
                    txt = txt.substr(1); // This removes the first character.
                    continue;
                }
                if (x2 > viewport.x2) {
                    // Extract the last character.
                    getTextSize(txt.substr(txt.length() - 1), width, height); 
                    txt = txt.substr(0, txt.length() - 1); // Remove the last character.
                    continue;
                }
                break;
            }
            
            XDrawString(display, window, gc, x, y, txt.c_str(), (int)txt.length());
        }

        void getTextSize(const string &text, int &width, int &height) const {            
            if (fontInfo) {
                XCharStruct overall;
                int direction, ascent, descent; // TODO: add these retrievable
                
                XTextExtents(fontInfo, text.c_str(), (int)text.length(), &direction, &ascent, &descent, &overall);
                
                // The width and height are calculated based on the text extents
                width = overall.width;
                height = ascent + descent;
                return;
            }
            // Handle the case where the font is not set
            width = 0;
            height = 0;
        }

        void eventLoop(unsigned long ms = defaultLoopMs) const {

            while (!close) {

                if (!onLoopHandlers.empty() && XPending(display) <= 0) {
                    sleep(ms);
                    for (const onLoopHandler& onLoop: onLoopHandlers) 
                        onLoop(eventContext);
                    continue;
                }

                XEvent event;
                XNextEvent(display, &event);

                // Flush the event queue to discard any pending events
                while (XPending(display) > 0) { 
                    XNextEvent(display, &event);
                    if (event.type != MotionNotify) break;
                }

                int width, height;
                KeySym key;
                char text[32]; // FlawFinder: ignore
                switch (event.type) {
                    case Expose:
                        // Handle expose event (e.g., redraw)
                        getWindowSize(width, height);
                        for (const onResizeHandler& onResize: onResizeHandlers)
                            onResize(eventContext, width, height);
                        break;

                    case KeyPress:
                        // Handle key press event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        for (const onKeyPressHandler& onKeyPress: onKeyPressHandlers)
                            onKeyPress(eventContext, key);
                        break;

                    case KeyRelease:
                        // Handle key release event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        for (const onKeyReleaseHandler& onKeyRelease: onKeyReleaseHandlers)
                            onKeyRelease(eventContext, key);
                        break;

                    case ButtonPress:
                        // Handle mouse button press event
                        for (const onTouchHandler& onTouch: onTouchHandlers)
                            onTouch(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case ButtonRelease:
                        // Handle mouse button release event
                        for (const onReleaseHandler& onRelease: onReleaseHandlers)
                            onRelease(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case MotionNotify:
                        // Handle mouse motion event
                        for (const onMoveHandler& onMove: onMoveHandlers)
                            onMove(eventContext, event.xbutton.x, event.xbutton.y);
                        break;

                    default:
                        throw ERROR("Unhandled event type: " + to_string(event.type));
                        break;
                }
            }
        }
    };

    Display* GFX::display = NULL;
    const char* GFX::defaultWindowTitle = Theme::windowTitle;
    const char* GFX::defaultWindowFont = Theme::windowFont;


    class Painter {
    public:

        struct TextSize { int width = 0, height = 0; };

        virtual void color(Color) const { throw ERR_UNIMP; }
        virtual void point(int, int) { throw ERR_UNIMP; }
        virtual void rect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void fillRect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void line(int, int, int, int) { throw ERR_UNIMP; }
        virtual void hLine(int, int, int) { throw ERR_UNIMP; }
        virtual void vLine(int, int, int) { throw ERR_UNIMP; }
        virtual void font(const char*) const { throw ERR_UNIMP; }
        virtual void write(int, int, const string&) { throw ERR_UNIMP; }
        virtual TextSize getTextSize(const string&) const { throw ERR_UNIMP; }
        virtual int getWidth() const { throw ERR_UNIMP; } // = 0; // { return 0; };
        virtual int getHeight() const { throw ERR_UNIMP; } // = 0; // { return 0; };
    };


    class Area: public EventHandler, public Painter {
    public:

        static const Align defaultAreaTextAlign = Theme::textAlign;
        static const Color defaultAreaBorderColor = Theme::borderColor;
        static const Color defaultAreaTextColor = Theme::textColor;
        static const Border defaultAreaBorder = NONE; // TODO: to theme
        static const Color defaultAreaBackgroundColor = GFX::defaultWindowColor;
        static const int defaultFrameMargin = Theme::frameMargin;
        static const int defaultTextMargin = Theme::textPadding;

        typedef void (*onDrawHandler)(void*);
        
        vector<onDrawHandler> onDrawHandlers;

    protected:
        GFX& gfx;

        int left, top;
        int width, height;
        const string text;
        const Align textAlign;
        Color borderColor = defaultAreaBorderColor;
        Color textColor = defaultAreaTextColor;
        Border border;
        Color backgroundColor;
        int frameMargin;
        int textPadding;

        vector<Area*> areas;
        Area* parent = NULL;
        int scrollX = 0, scrollY = 0,
            scrollXMin = 0, scrollYMin = 0,
            scrollXMax = 0, scrollYMax = 0;

        void forceScrollInRange() {
            if (scrollX < scrollXMin) scrollX = scrollXMin;
            if (scrollY < scrollYMin) scrollY = scrollYMin;
            if (scrollX > scrollXMax) scrollX = scrollXMax;
            if (scrollY > scrollYMax) scrollY = scrollYMax;
        }

        void reduceViewport(Rectangle& viewport) const {
            if (!parent) return;
            int parentTop = parent->getTop() + 1;
            int parentLeft = parent->getLeft() + 1;
            int parentRight = parent->getRight(parentLeft) - 2;
            int parentBottom = parent->getBottom(parentTop) - 2;
            if (viewport.intersect(parentLeft, parentTop, parentRight, parentBottom))
                parent->reduceViewport(viewport);
        }

        void prepareSetViewport() const {
            Rectangle viewport;
            getViewport(viewport);
            reduceViewport(viewport);
            gfx.setViewport(viewport);
        }

        void prepare(int &x, int &y) const {
            int leftAndScroll = getLeft() - scrollX;
            int topAndScroll = getTop() - scrollY;
            x += leftAndScroll;
            y += topAndScroll;
            prepareSetViewport();
        }

        void prepare(int &x1, int &y1, int &x2, int &y2) const {
            int leftAndScroll = getLeft() - scrollX;
            int topAndScroll = getTop() - scrollY;
            x1 += leftAndScroll;
            y1 += topAndScroll;
            x2 += leftAndScroll;
            y2 += topAndScroll;
            prepareSetViewport();
        }

    public:

        Area(GFX& gfx, int left, int top, int width, int height, 
            const string &text = "", const Align textAlign = defaultAreaTextAlign,
            const Border border = defaultAreaBorder,
            const Color backgroundColor = defaultAreaBackgroundColor,
            const int frameMargin = defaultFrameMargin,
            const int textPadding = defaultTextMargin
        ):
            gfx(gfx),
            left(left), top(top), width(width), height(height), 
            text(text), textAlign(textAlign), border(border), 
            backgroundColor(backgroundColor), frameMargin(frameMargin),
            textPadding(textPadding)
        {
            setScrollXYMinMax(0, 0, width, height);
        }

        virtual ~Area() {
            // TODO: manage area pointers: vector_destroy<Area>(areas);
        }

        const Rectangle& getViewport(Rectangle& viewport) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            viewport.set(l, t, r, b);
            return viewport;
        }

        void color(Color color) const override {
            gfx.setColor(color);
        }

        void point(int x, int y) override {
            setScrollXYMinMax(x, y);
            prepare(x, y);
            gfx.drawPoint(x, y);
        }

        void rect(int x1, int y1, int x2, int y2) override {
            setScrollXYMinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawRectangle(x1, y1, x2, y2);
        }

        void fillRect(int x1, int y1, int x2, int y2) override {
            setScrollXYMinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.fillRectangle(x1, y1, x2, y2);
        }

        void line(int x1, int y1, int x2, int y2) override {
            setScrollXYMinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawLine(x1, y1, x2, y2);
        }

        void hLine(int x1, int y1, int x2) override {
            int y2 = y1;
            setScrollXYMinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawHorizontalLine(x1, y1, x2);
        }

        void vLine(int x1, int y1, int y2) override {
            int x2 = x1;
            setScrollXYMinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawVerticalLine(x1, y1, y2);
        }

        void font(const char* font) const override {
            gfx.setFont(font);
        }

        void write(int x, int y, const string &text) override {
            setScrollXYMinMax(x, y);
            prepare(x, y);
            gfx.writeText(x, y, text);
        }

        virtual TextSize getTextSize(const string& text) const override {
            TextSize textSize;
            gfx.getTextSize(text, textSize.width, textSize.height);
            return textSize;
        }

        GFX& getGFX() const {
            return gfx;
        }

        void setParent(Area* parent) {
            this->parent = parent;
        }

        Area* getParent() const {
            return parent;
        }

        Area* getParentOrSelf() {
            return parent ? parent : this;
        }

        Area* getRoot() {
            return parent ? parent->getRoot() : this;
        }

        Area& child(Area& area) {
            area.setParent(this);
            areas.push_back(&area);
            setScrollXYMinMax(
                area.left + area.width + frameMargin,
                area.top + area.height + frameMargin
            );
            return area;
        }

        void setScrollXYMin(int x, int y, bool forceInRange = true) {
            int xMin = x;
            int yMin = y;
            if (xMin < scrollXMin) scrollXMin = xMin;
            if (yMin < scrollYMin) scrollYMin = yMin;
            if (forceInRange) forceScrollInRange();
        }

        void setScrollXYMax(int x, int y, bool forceInRange = true) {
            int xMax = x - width;
            int yMax = y - height;
            if (xMax > scrollXMax) scrollXMax = xMax;
            if (yMax > scrollYMax) scrollYMax = yMax;
            if (forceInRange) forceScrollInRange();
        }

        void setScrollXYMinMax(int x, int y, bool forceInRange = true) {
            setScrollXYMin(x, y, false);
            setScrollXYMax(x, y, false);
            if (forceInRange) forceScrollInRange();
        }

        void setScrollXYMinMax(int x1, int y1, int x2, int y2, bool forceInRange = true) {
            setScrollXYMin(x1, y1, false);
            setScrollXYMax(x2, y2, false);
            if (forceInRange) forceScrollInRange();
        }

        void setScrollXY(int x, int y) {
            scrollX = x;
            scrollY = y;
            forceScrollInRange();
        }

        void moveScrollXY(int x, int y) {
            scrollX += x;
            scrollY += y;
            forceScrollInRange();
        }

        int getScrollX() const {
            return scrollX;
        }

        int getScrollY() const {
            return scrollY;
        }

        int getTop(bool withParent = true) const {
            if (!withParent) return top;
            Area* p = getParent();
            return top + (p ? p->getTop() - p->getScrollY() : 0);
        }

        void setTop(int top) {
            this->top = top;
        }

        int getLeft(bool withParent = true) const {
            if (!withParent) return left;
            Area* p = getParent();
            return left + (p ? p->getLeft() - p->getScrollX() : 0);
        }

        void setLeft(int left) {
            this->left = left;
        }    

        int getWidth() const override {
            return width;
        }

        void setWidth(int width) {
            this->width = width;
        }

        int getHeight() const override {
            return height;
        }

        void setHeight(int height) {
            this->height = height;
        }

        int getRight(bool withParent = true) const {
            return getLeft(withParent) + getWidth();
        }

        int getRight(int left) const {
            return left + getWidth();
        }

        int getRight(int left, int width) const {
            return left + width;
        }

        int getBottom(bool withParent = true) const {
            return getTop(withParent) + getHeight();
        }

        int getBottom(int top) const {
            return top + getHeight();
        }

        int getBottom(int top, int height) const {
            return top + height;
        }

        Border getBorder() const {
            return border;
        }

        void setBorder(Border border) {
            this->border = border;
        }

        string getText() const {
            return text;
        }

        Align getTextAlign() const {
            return textAlign;
        }

        Color getBackgroundColor() const {
            return backgroundColor;
        }

        void setBackgroundColor(Color backgroundColor) {
            this->backgroundColor = backgroundColor;
        }

        Color getBorderColor() const {
            return borderColor;
        }

        void setBorderColor(Color borderColor) {
            this->borderColor = borderColor;
        }

        Color getTextColor() const {
            return textColor;
        }

        void setTextColor(Color textColor) {
            this->textColor = textColor;
        }

        bool contains(int x, int y) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            return
                x >= l && x <= r &&
                y >= t && y <= b;
        }

        bool contains(int x1, int y1, int x2, int y2) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            return !(l >= x2 || x1 >= r || t >= y2 || y1 >= b);
        }

        bool contains(Area* area) const {
            int l = area->getLeft();
            int t = area->getTop();
            int r = area->getRight(l);
            int b = area->getBottom(t);
            return contains(l, t, r, b);
        }

        void propagateTouch(unsigned int button, int x, int y) {
            if (!contains(x, y)) return;
            for (const onTouchHandler& onTouch: onTouchHandlers)
                onTouch(this, button, x, y);
            for (Area* area: areas)
                if (contains(area)) area->propagateTouch(button, x, y);
        }

        void propagateRelease(unsigned int button, int x, int y) {
            for (const onReleaseHandler& onRelease: onReleaseHandlers)
                onRelease(this, button, x, y);
            for (Area* area: areas)
                if (contains(area)) area->propagateRelease(button, x, y);
        }

        void propagateMove(int x, int y) {
            for (const onMoveHandler& onMove: onMoveHandlers)
                onMove(this, x, y);
            for (Area* area: areas)
                if (contains(area)) area->propagateMove(x, y);
        }

        void drawBorder(int left, int top, int right, int bottom) const {
            Border b = getBorder();
            Color bColor;
            Color bColorLight;
            Color bColorDark;
            switch (b) {

                case NONE:
                    break;

                case SIMPLE:
                    gfx.setColor(getBorderColor());
                    gfx.drawRectangle(left, top, right, bottom);
                    break;

                case BUTTON_RELEASED:
                    // button borders get the background color
                    bColor = getBackgroundColor();
                    bColorLight = ColorMixer::light(bColor);
                    bColorDark = ColorMixer::dark(bColor);
                    gfx.setColor(bColorLight);
                    gfx.drawHorizontalLine(left, top, right);
                    gfx.drawVerticalLine(left, top, bottom);
                    gfx.setColor(bColorDark);
                    gfx.drawHorizontalLine(left, bottom, right);
                    gfx.drawVerticalLine(right, top, bottom);
                    break;

                case BUTTON_PUSHED:
                    // button borders get the background color
                    bColor = getBackgroundColor();
                    bColorLight = ColorMixer::light(bColor);
                    bColorDark = ColorMixer::dark(bColor);
                    gfx.setColor(bColorLight);
                    gfx.drawHorizontalLine(left, bottom, right);
                    gfx.drawVerticalLine(right, top, bottom);
                    gfx.setColor(bColorDark);
                    gfx.drawHorizontalLine(left, top, right);
                    gfx.drawVerticalLine(left, top, bottom);
                    break;

                default:
                    throw ERROR("Invalid border");
                    break;
            }
        }

        void drawBorder() {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);

            Rectangle viewport(l, t, r, b);
            reduceViewport(viewport);
            gfx.setViewport(viewport);

            drawBorder(l, t, r, b);
        }

        void draw() {
            int t = getTop();
            int l = getLeft();
            int w = getWidth();
            int h = getHeight();
            int r = getRight(l, w);
            int b = getBottom(t, h);

            Rectangle viewport(l, t, r, b);
            reduceViewport(viewport);
            gfx.setViewport(viewport);
            
            gfx.setColor(getBackgroundColor());
            gfx.fillRectangle(l, t, r, b);

            drawBorder(l, t, r, b);

            const string txt = getText();
            if(!txt.empty()) {
                int txtWidth, txtHeight;
                gfx.getTextSize(txt, txtWidth, txtHeight);
                int txtLeft;
                Align txtAlign = getTextAlign();
                switch (txtAlign) {

                    case CENTER:
                        txtLeft = l + ((w - txtWidth) / 2);
                        break;

                    case LEFT:
                        txtLeft = l + textPadding;
                        break;

                    case RIGHT:
                        txtLeft = l + w - textPadding - txtWidth;
                        break;
                    
                    default:
                        throw ERROR("Invalid text align");
                        break;
                }
                int txtTop = t + ((h - txtHeight) / 2) + 16; // ??16
                gfx.setColor(getTextColor());
                gfx.writeText(txtLeft, txtTop, txt);
            }

            for (Area* area: areas)
                if (contains(area)) area->draw();

            for (const onDrawHandler& onDraw: onDrawHandlers) 
                onDraw(this);

        }
    };

    class GUI: public Area {
    protected:

        static void resize(void* context, int width, int height) {
            GUI* that = static_cast<GUI*>(context);
            that->setWidth(width);
            that->setHeight(height);
            that->draw();
        }

        static void touch(void* context, unsigned int button, int x, int y) {
            Area* that = static_cast<Area*>(context);
            that->propagateTouch(button, x, y);
        }

        static void release(void* context, unsigned int button, int x, int y) {
            Area* that = static_cast<Area*>(context);
            that->propagateRelease(button, x, y);
        }

        static void move(void* context, int x, int y) {
            Area* that = static_cast<Area*>(context);
            that->propagateMove(x, y);
        }

        void init(int width, int height, const char* title = GFX::defaultWindowTitle, Color color = GFX::defaultWindowColor) {
            gfx.openWindow(width, height, title, color);
            gfx.eventContext = this;
            gfx.onResizeHandlers.push_back(resize);
            gfx.onTouchHandlers.push_back(touch);
            gfx.onReleaseHandlers.push_back(release);
            gfx.onMoveHandlers.push_back(move);
        }

    public:
        GUI(GFX& gfx, int width, int height, const char* title = GFX::defaultWindowTitle, Color color = GFX::defaultWindowColor):
            Area(gfx, 0, 0, width, height, "", defaultAreaTextAlign, defaultAreaBorder) 
        {
            init(width, height, title, color);
        }

        void close() {
            gfx.closeWindow();
        }

        void loop(unsigned long ms = GFX::defaultLoopMs) const {
            gfx.eventLoop(ms);
        }

        void setTitle(const char* title) {
            gfx.setWindowTitle(title);
        }
    };

    class Frame: public Area {
    public:

        static const Border defaultFrameBorder = BUTTON_PUSHED; // TODO: to theme
        static const Color defaultFrameBackgroundColor = Theme::frameBackgroundColor;

    protected:

        bool drag = false;
        int dragStartedX = 0, dragStartedY = 0, dragScrollStartedX = 0, dragScrollStartedY = 0;

        static void touch(void* context, unsigned int, int x, int y) {
            Frame* that = static_cast<Frame*>(context);

            // drag & scroll only if no child in the event focus
            for (Area* area: that->areas)
                if (area->contains(x, y)) return; // cppcheck-suppress useStlAlgorithm

            that->drag = true;
            that->dragStartedX = x;
            that->dragStartedY = y;
            that->dragScrollStartedX = that->getScrollX();
            that->dragScrollStartedY = that->getScrollY();
        }
        
        static void release(void* context, unsigned int, int, int) {
            Frame* that = static_cast<Frame*>(context);
            that->drag = false;
        }
        
        static void move(void* context, int x, int y) {
            Frame* that = static_cast<Frame*>(context);
            if (!that->fixed && that->drag) {
                that->setScrollXY(
                    that->dragScrollStartedX + (that->dragStartedX - x), 
                    that->dragScrollStartedY + (that->dragStartedY - y)
                );
                that->draw();
            }
        }

    public:

        bool fixed = false;

        Frame(GFX& gfx, int left, int top, int width, int height,
            const Border border = defaultFrameBorder,
            const Color backgroundColor = defaultFrameBackgroundColor
        ): Area(gfx, left, top, width, height, "", CENTER, border, backgroundColor)
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
            onMoveHandlers.push_back(move);
        }
    };


    class Button: public Area {
    protected:
        
        static void touch(void* context, unsigned int, int, int) {
            Button* that = static_cast<Button*>(context);
            
            if (that->sticky) {
                that->pushed ? that->release() : that->push();                
                return;
            }
            
            if (!that->pushed) that->push();
        }
        
        static void release(void* context, unsigned int, int, int) {
            Button* that = static_cast<Button*>(context);

            if (that->sticky) return;
                
            if (that->pushed) that->release();
        }

        bool pushed = false;
        bool sticky = false;

    public:

        Button(GFX& gfx, int left, int top, int width, int height, 
            const string &text, const Align textAlign = Area::defaultAreaTextAlign
        ): Area(gfx, left, top, width, height, text, textAlign, BUTTON_RELEASED) // TODO: border = BUTTON_RELEASED to theme
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
        }

        bool isPushed() const {
            return pushed;
        }

        void setPushed(bool pushed) {
            if (pushed) push();
            else release();
        }

        bool isSticky() const {
            return sticky;
        }

        void setSticky(bool sticky) {
            this->sticky = sticky;
        }

        void push() {
            setBorder(BUTTON_PUSHED);
            drawBorder();
            pushed = true;
        }

        void release() {
            setBorder(BUTTON_RELEASED);
            drawBorder();
            pushed = false;
        }
    };


    class Label: public Area {
    public:

        static const Align defaultLabelTextAlign = Theme::labelTextAlign;

    public:
        Label(GFX& gfx, int left, int top, int width, int height, 
            const string &text, const Align textAlign = defaultLabelTextAlign
        ): Area(gfx, left, top, width, height, text, textAlign, NONE) // TODO: border = NONE to theme
        {}
    };


    class Accordion: public Area {
    public:

        class Container;

        class Toggler: public Button {
        protected:
            
            static void toggle(void* context, unsigned int, int, int) {
                Toggler* that = (Toggler*)context;
                Accordion& accordion = that->container.getAccordion(); 
                
                if (accordion.isSingle()) accordion.closeAllExcept(that->containerIndex);

                if (that->sticky) {
                    accordion.toggleAt(that->containerIndex);
                    return;
                }
                
                Container* container = accordion.getContainers().at(that->containerIndex);
                if (container->isOpened()) {
                    if (!accordion.isOne()) accordion.closeAt(that->containerIndex);
                } else accordion.openAt(that->containerIndex);
            }

            Container& container;
            size_t containerIndex;
        public:
            Toggler(Container& container, GFX& gfx, size_t containerIndex, 
                int left, int top, int width, int height, 
                const string &text, const Align textAlign = Area::defaultAreaTextAlign
            ): 
                Button(gfx, left, top, width, height, text, textAlign),
                container(container),
                containerIndex(containerIndex)
            {
                sticky = container.getAccordion().isSticky();
                onTouchHandlers.push_back(toggle);
            }

            Container& getContainer() const {
                return container;
            }

            size_t getContainerIndex() const {
                return containerIndex;
            }
        };

        class Container {
        protected:
            const int togglerHeight = 30; // TODO
            const int innerBorderSize = 1;

            Toggler* toggler = NULL;
            Frame* frame = NULL;

            Accordion& accordion;
            int frameHeight = 100; // TODO
            bool opened = false;
        public:
            Container(
                Accordion& accordion,
                const string& title, const Align textAlign, 
                int frameHeight = 100 // TODO
            ): 
                accordion(accordion), frameHeight(frameHeight) 
            {
                GFX& gfx = accordion.getGFX();
                const int width = accordion.getWidth();
                size_t accordionContainerAt = accordion.getContainers().size();
                const int togglerTop = accordion.height + innerBorderSize;

                toggler = new Toggler(*this, gfx, accordionContainerAt,
                    innerBorderSize, togglerTop, 
                    width - innerBorderSize*2, 
                    togglerHeight - innerBorderSize*2, 
                    title, textAlign);

                frame = new Frame(gfx, 
                    innerBorderSize, togglerTop + togglerHeight - innerBorderSize*2,
                    width, 0, NONE);
            
                accordion.child(*toggler);
                accordion.child(*frame);
                accordion.height = togglerHeight * ((int)accordionContainerAt + 1);
            }

            ~Container() {
                if (toggler) delete toggler;
                toggler = NULL;
                if (frame) delete frame;
                frame = NULL;
            }

            bool isOpened() const {
                return opened;
            }

            void setOpened(bool opened) {
                if (opened) close();
                else open();
            }

            void open() { 
                if (opened) return;
                toggler->push();
                if (!toggler->isSticky()) toggler->release();
                opened = true;
            }

            void close() {
                if (!opened) return;
                toggler->release();
                opened = false;
            }

            Accordion& getAccordion() const {
                return accordion;
            }

            Toggler& getToggler() const {
                return *toggler;
            }

            Frame& getFrame() const {
                return *frame;
            }

            int getFrameHeight() const {
                return frameHeight;
            }
        };

    protected:
        vector<Container*> containers;
        bool sticky = false; // TODO
        bool single = false; // TODO
        bool one = false; // TODO
    public:
        Accordion(GFX& gfx, int left, int top, int width, 
            bool sticky = false, // TODO
            const Align textAlign = Area::defaultAreaTextAlign, // TODO
            const Border border = NONE, //Area::defaultAreaBorder, // TODO
            const Color backgroundColor = Area::defaultAreaBackgroundColor, // TODO
            const int frameMargin = Area::defaultFrameMargin, // TODO
            const int textPadding = Area::defaultTextMargin // TODO
        ): 
            Area(gfx, left, top, width, 0, "", 
                textAlign, border, backgroundColor, frameMargin, textPadding), sticky(sticky) {}

        ~Accordion() {
            vector_destroy<Container>(containers);
        }

        bool isSticky() const {
            return sticky;
        }

        void setSticky(bool sticky) {
            this->sticky = sticky;
            for (const Container* container: containers) {
                Toggler& toggler = container->getToggler();
                toggler.setSticky(sticky);
                if (sticky && container->isOpened()) toggler.push();
            }
        }

        bool isSingle() const {
            return single;
        }

        void setSingle(bool single) {
            this->single = single;

            // if single collapse the rest 
            // keep open only the first one 
            // that is already opened
            if (single) {
                bool closing = false;
                size_t containersSize = containers.size();
                for (size_t i = 0; i < containersSize; i++) {
                    if (isOpenedAt(i)) {
                        if (closing) closeAt(i);
                        else closing = true;
                    }
                }
            }
            getParentOrSelf()->draw();
        }

        bool isOne() const {
            return one;
        }

        void setOne(bool one) {
            if (one) setSingle(true);
            this->one = one;
        }

        void closeAllExcept(size_t exceptIndex) {
            size_t containersSize = containers.size();
            for (size_t i = 0; i < containersSize; i++) {
                if (i != exceptIndex) closeAt(i);
            }
        }

        void openAll() {
            const size_t containersSize = containers.size();
            for (size_t ii = 0; ii < containersSize; ii++) {
                openAt(ii);
            }
        }

        Container& addContainer(const string& title, int frameHeight) {
            Container* container = new Container(*this, title, textAlign, frameHeight);
            containers.push_back(container);
            return *container;
        }

        vector<Container*>& getContainers() {
            return containers;
        }

        void openAt(size_t containerIndex) {
            size_t containersSize = containers.size();
            Container* container = containers.at(containerIndex);
            if (container->isOpened()) return;
            container->open();
            const int frameHeight = container->getFrameHeight();
            container->getFrame().setHeight(frameHeight);
            for (size_t i = containerIndex + 1; i < containersSize; i++) {
                container = containers.at(i);
                Toggler& toggler = container->getToggler();
                toggler.setTop(toggler.getTop(false) + frameHeight);
                Frame& frame = container->getFrame();
                frame.setTop(frame.getTop(false) + frameHeight);
            }
            height += frameHeight;
            getParentOrSelf()->draw();
        }

        void closeAt(size_t containerIndex) {
            size_t containersSize = containers.size();
            Container* container = containers.at(containerIndex);
            if (!container->isOpened()) return;
            container->close();
            const int frameHeight = container->getFrameHeight();
            container->getFrame().setHeight(0);
            for (size_t i = containerIndex + 1; i < containersSize; i++) {
                container = containers.at(i);
                Toggler& toggler = container->getToggler();
                toggler.setTop(toggler.getTop(false) - frameHeight);
                Frame& frame = container->getFrame();
                frame.setTop(frame.getTop(false) - frameHeight);
            }
            height -= frameHeight;
            getParentOrSelf()->draw();
        }

        void toggleAt(size_t containerIndex) {
            isOpenedAt(containerIndex) ? closeAt(containerIndex) : openAt(containerIndex);
        }

        bool isOpenedAt(size_t containerIndex) {
            return containers.at(containerIndex)->isOpened();
        }

        bool isClosedAt(size_t containerIndex) {
            return !isOpenedAt(containerIndex);
        }
    };

    
    class Application {
    protected:
        
        static void close(void* context, unsigned int, int, int) {
            ((Area*)context)->getRoot()->getGFX().close = true;
        }

        GFX gfx = GFX(this);
        GUI gui = GUI(gfx, 1600, 900, "Application");

    public:

        virtual ~Application() {}

        Application* run() {
            try {
                // Application::init();
                init();
                gui.loop();
            } catch (exception& e) {
                logger.date().writeln("Exception: " + string(e.what()));
                throw ERROR("Application error, see more at log...");
            }
            return this;
        }

        virtual void init() {
            throw ERR_UNIMP;
        }
    };

    class FrameApplication: public Application {
    protected:
        Frame mainFrame = Frame(gfx, 0, 0, gui.getWidth(), gui.getHeight(), NONE, Theme::windowColor);
    public:
        virtual void init() override {
            gui.child(mainFrame);
        }
    };

}
