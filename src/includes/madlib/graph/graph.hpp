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

    template<typename T>
    class Point {
    protected:

        T x, y;

    public:

        Point(T x = 0, T y = 0): x(x), y(y) {}

        void setX(T x) {
            this->x = x; 
        }

        T getX() const {
            return x;
        }

        void setY(T y) {
            this->y = y;
        }

        T getY() const {
            return y;
        }

        void setXY(T x, T y) {
            setX(x);
            setY(y);
        }

    };

    class RealPoint: public Point<double> {
    public:
        using Point<double>::Point;
    };

    class ProjectedPoint: public Point<int> {
    public:
        using Point<int>::Point;
    };

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

        static bool isValidColor(Color color, bool throws = true) {
            switch (color) {
                case black:
                case white:
                case gray:
                case darkGray:
                case red:
                case green:
                case blue:
                case lightRed:
                case lightGreen:
                case lightBlue:
                case cyan:
                case purple:
                case orange:
                case lightCyan:
                case lightPurple:
                case yellow:
                    return true;
                default:
                    if (throws) 
                        throw ERROR("Invalid color");
                    return false;
            } 
        }

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
        static const unsigned long defaultGFXEventLoopMs = 100;
        static const Color defaultWindowColor = gray;
        static const char* defaultWindowTitle;
        static const char* defaultWindowFont;

        static const Align defaultAreaTextAlign = CENTER;
        static const Border defaultAreaBorder = NONE;
        static const Color defaultAreaBackgroundColor = gray;
        static const int defaultAreaMargin = 10;
        static const int defaultAreaTextMargin = 10;
        static const Color defaultAreaBorderColor = gray;
        static const Color defaultAreaTextColor = black;

        static const Color frameBackgroundColor = darkGray;
        static const int frameMargin = 10;
        static const int textPadding = 10;
        static const Border defaultFrameBorder = NONE;
        static const Color defaultFrameBackgroundColor = gray;

        static const Align defaultButtonTextAlign = CENTER;
        static const Border defaultButtonBorder = BUTTON_RELEASED;

        static const Border defaultAccordionBorder = NONE;
        static const Color defaultAccordionBackgroundColor = darkGray;
        static const Align defaultAccordionTogglerTextAlign = CENTER;

        static const Align labelTextAlign = LEFT;
        static const int barThickness = 20;
        static const Border chartBorder = BUTTON_PUSHED;
        static const Color chartBackgroundColor = black;
        static const Color defaultCandleColorUp = green;
        static const Color defaultCandleColorDown = red;
        static const Color defaultChartLabelColor = white;
        static const bool defaultChartLabelHasBackground = true;
        static const Color defaultChartLabelBackgroundColor = black;
        static const Color defaultChartLabelBorderColor =  gray;
        static const int defaultChartLabelPadding = 3;
        static const Color defaultChartDotScaleContext;
        static const Color defaultChartLineScaleContext;
        static const Color defaultChartBoxScaleContext;
        static const Color defaultChartFilledScaleContext;
    };
    const char* Theme::defaultWindowTitle = "graph";
    const char* Theme::defaultWindowFont = "10x20";
    const Color Theme::defaultChartDotScaleContext = darkGray;
    const Color Theme::defaultChartLineScaleContext = darkGray;
    const Color Theme::defaultChartBoxScaleContext = darkGray;
    const Color Theme::defaultChartFilledScaleContext = darkGray;

    class EventHandler {
    public:

        typedef void (*onResizeHandler)(void*, int, int);
        typedef void (*onKeyPressHandler)(void*, unsigned long);
        typedef void (*onKeyReleaseHandler)(void*, unsigned long);
        typedef void (*onTouchHandler)(void*, unsigned int, int, int);
        typedef void (*onReleaseHandler)(void*, unsigned int, int, int);
        typedef void (*onMoveHandler)(void*, int, int);
        typedef void (*onLoopHandler)(void*);
        typedef void (*onDrawHandler)(void*);
        // typedef void (*onScrollHandler)(const void*, unsigned int);
        // typedef void (*onZoomHandler)(const void*, unsigned int);

    protected:

        void* eventContext = NULL;
        
        vector<onResizeHandler> onResizeHandlers;
        vector<onKeyPressHandler> onKeyPressHandlers;
        vector<onKeyReleaseHandler> onKeyReleaseHandlers;
        vector<onTouchHandler> onTouchHandlers;
        vector<onReleaseHandler> onReleaseHandlers;
        vector<onMoveHandler> onMoveHandlers;
        vector<onLoopHandler> onLoopHandlers;
        vector<onDrawHandler> onDrawHandlers;
        // vector<onScrollHandler> onScrollHandlers;
        // vector<onZoomHandler> onZoomHandlers;

    public:

        EventHandler(void* eventContext = NULL): eventContext(eventContext) {}

        void setEventContext(void* eventContext) {
            this->eventContext = eventContext;
        }

        void* getEventContext() const {
            return eventContext;
        }

        vector<onResizeHandler> getResizeHandlers() {
            return onResizeHandlers;
        }

        void addResizeHandler(onResizeHandler handler) {
            onResizeHandlers.push_back(handler);
        }

        vector<onKeyPressHandler> getKeyPressHandlers() {
            return onKeyPressHandlers;
        }

        void addKeyPressHandler(onKeyPressHandler handler) {
            onKeyPressHandlers.push_back(handler);
        }

        vector<onKeyReleaseHandler> getKeyReleaseHandlers() {
            return onKeyReleaseHandlers;
        }

        void addKeyReleaseHandler(onKeyReleaseHandler handler) {
            onKeyReleaseHandlers.push_back(handler);
        }

        vector<onTouchHandler> getTouchHandlers() {
            return onTouchHandlers;
        }

        void addTouchHandler(onTouchHandler handler) {
            onTouchHandlers.push_back(handler);
        }

        vector<onReleaseHandler> getReleaseHandlers() {
            return onReleaseHandlers;
        }

        void addReleaseHandler(onReleaseHandler handler) {
            onReleaseHandlers.push_back(handler);
        }

        vector<onMoveHandler> getMoveHandlers() {
            return onMoveHandlers;
        }

        void addMoveHandler(onMoveHandler handler) {
            onMoveHandlers.push_back(handler);
        }

        vector<onLoopHandler> getLoopHandlers() {
            return onLoopHandlers;
        }

        void addLoopHandler(onLoopHandler handler) {
            onLoopHandlers.push_back(handler);
        }

        vector<onDrawHandler> getDrawHandlers() {
            return onDrawHandlers;
        }

        void addDrawHandler(onDrawHandler handler) {
            onDrawHandlers.push_back(handler);
        }

        // vector<onScrollHandler> getScrollHandlers() {
        //     return onScrollHandlers;
        // }

        // void addScrollHandler(onScrollHandler handler) {
        //     onScrollHandlers.push_back(handler);
        // }

        // vector<onZoomHandler> getZoomHandlers() {
        //     return onZoomHandlers;
        // }

        // void addZoomHandler(onZoomHandler handler) {
        //     onZoomHandlers.push_back(handler);
        // }

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

        void openWindow(
            int width, int height, 
            const char* title = Theme::defaultWindowTitle, 
            Color color = Theme::defaultWindowColor, 
            const char* font = Theme::defaultWindowFont
        ) {
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
                const int textYFixer4 = 4; // ??4
                int x1 = x;
                int y1 = y + textYFixer4;
                int x2 = x + width;
                int y2 = y - height + textYFixer4;
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

        void eventLoop(unsigned long ms = Theme::defaultGFXEventLoopMs) const {

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
                        // Handle mouse button press / click / touch / scroll / zoom event
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


    class Zoom {
    public:

        const int defaultCenterX = 0, defaultCenterY = 0; // TODO
        const double defaultRatioX = 1.0, defaultRatioY = 1.0;
    protected:
        ProjectedPoint center = ProjectedPoint(defaultCenterX, defaultCenterY);
        RealPoint ratio = RealPoint(defaultRatioX, defaultRatioY);

    public:

        Zoom(): center(defaultCenterX, defaultCenterY), ratio(defaultRatioX, defaultRatioY) {}
        Zoom(int centerX, int centerY): center(centerX, centerY), ratio(defaultRatioX, defaultRatioY) {}
        explicit Zoom(const ProjectedPoint& center): center(center), ratio(defaultRatioX, defaultRatioY) {}
        Zoom(double ratioX, double ratioY): center(defaultCenterX, defaultCenterY), ratio(ratioX, ratioY) {}
        explicit Zoom(const RealPoint& ratio): center(defaultCenterX, defaultCenterY), ratio(ratio) {}
        Zoom(int centerX, int centerY, double ratioX, double ratioY): center(centerX, centerY), ratio(ratioX, ratioY) {}
        Zoom(const ProjectedPoint& center, const RealPoint& ratio): center(center), ratio(ratio) {}
        Zoom(double ratioX, double ratioY, int centerX, int centerY): center(centerX, centerY), ratio(ratioX, ratioY) {}
        Zoom(const RealPoint& ratio, const ProjectedPoint& center): center(center), ratio(ratio) {}
        
        Zoom& operator=(const Zoom& other) {
            if (this != &other) { // Check for self-assignment
                this->center = other.center;
                this->ratio = other.ratio;
            }
            return *this;
        }

        ProjectedPoint getCenter() const {
            return center;
        }

        void setCenter(const ProjectedPoint& center) {
            this->center = center;
        }

        void setCenter(int x = 0, int y = 0) {
            setCenter(ProjectedPoint(x, y));
        }

        RealPoint getRatio() const {
            return ratio;
        }

        void setRatio(const RealPoint& ratio) {
            this->ratio = ratio;
        }

        void setRatio(double x = 1.0, double y = 1.0) {
            setRatio(RealPoint(x, y));
        }

        void setRatioX(double ratioX) {
            this->ratio.setX(ratioX);
        }

        void setRatioY(double ratioY) {
            this->ratio.setY(ratioY);
        }

        int applyX(int origoX, int pointX) const {
            int origoXAddCenterX = origoX + center.getX();
            int pointXSubCenterX = pointX - origoXAddCenterX;
            double pointXSubCenterXMulRatioX = pointXSubCenterX * ratio.getX();
            return origoXAddCenterX + (int)pointXSubCenterXMulRatioX;
        }

        int applyY(int origoY, int pointY) const {
            int origoYAddCenterY = origoY + center.getY();
            int pointYSubCenterY = pointY - origoYAddCenterY;
            double pointYSubCenterYMulRatioY = pointYSubCenterY * ratio.getY();
            return origoYAddCenterY + (int)pointYSubCenterYMulRatioY;
        }

        ProjectedPoint apply(int origoX, int origoY, int pointX, int pointY) const {
            ProjectedPoint result(
                applyX(origoX, pointX),
                applyY(origoY, pointY)
            );

            return result;
        }
        
        ProjectedPoint apply(const ProjectedPoint& origo, const ProjectedPoint& point) const {
            return apply(origo.getX(), origo.getY(), point.getX(), point.getY());
        }
    };

    class Zoomable {
    protected:
    
        Zoom& zoom;

    public:

        Zoomable(Zoom& zoom): zoom(zoom) {}

        void setZoom(const Zoom& zoom) {
            this->zoom = zoom;
        }

        Zoom& getZoom() {
            return zoom;
        }
    };

    class Scrollable {
    protected:

        bool fixed = false;

        int width, height;

        int scrollX = 0, scrollY = 0,
            scrollXMin = 0, scrollYMin = 0,
            scrollXMax = 0, scrollYMax = 0;

    public:

        Scrollable(int width, int height): width(width), height(height) {}

        void setFixed(bool fixed) {
            this->fixed = fixed;
        }

        bool isFixed() const {
            return fixed;
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
            if (xMin < scrollXMin) scrollXMin = xMin;
            if (yMin < scrollYMin) scrollYMin = yMin;
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
            if (fixed) return;
            scrollX = x;
            scrollY = y;
            forceScrollInRange();
        }

        void moveScrollXY(int x, int y) {
            if (fixed) return;
            scrollX += x;
            scrollY += y;
            forceScrollInRange();
        }

        Point<int> getScrollXY() const {
            return Point<int>(scrollX, scrollY);
        }
    };

    class Painter: public Scrollable, public Zoomable, public EventHandler {
    public:

        typedef struct { int width = 0, height = 0; } TextSize;

        Painter(int width, int height, Zoom& zoom): 
            Scrollable(width, height),
            Zoomable(zoom)
        {}

        virtual void brush(Color) const { throw ERR_UNIMP; }
        virtual void point(int, int) { throw ERR_UNIMP; }
        virtual void rect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void fRect(int, int, int, int) { throw ERR_UNIMP; }
        virtual void line(int, int, int, int) { throw ERR_UNIMP; }
        virtual void hLine(int, int, int) { throw ERR_UNIMP; }
        virtual void vLine(int, int, int) { throw ERR_UNIMP; }
        virtual void font(const char*) const { throw ERR_UNIMP; }
        virtual void write(int, int, const string&) { throw ERR_UNIMP; }
        virtual TextSize getTextSize(const string&) const { throw ERR_UNIMP; }
        // virtual int getWidth() const { throw ERR_UNIMP; } // = 0; // { return 0; };
        // virtual int getHeight() const { throw ERR_UNIMP; } // = 0; // { return 0; };

        virtual void draw() = 0;
    };


    class Area: public Painter {
    protected:
        GFX& gfx;

        int left, top;
        const string text;
        const Align textAlign;
        Border border;
        Color backgroundColor;
        int areaMargin;
        int textPadding;
        Color borderColor;
        Color textColor;

        vector<Area*> areas;
        Area* parent = NULL;

        bool calcScrollOnly = false;

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

        Area(
            GFX& gfx, Zoom& zoom, 
            int left, int top, 
            int width, int height,
            const string &text = "", 
            const Align textAlign = Theme::defaultAreaTextAlign,
            const Border border = Theme::defaultAreaBorder,
            const Color backgroundColor = Theme::defaultAreaBackgroundColor,
            const int areaMargin = Theme::defaultAreaMargin,
            const int textPadding = Theme::defaultAreaTextMargin,
            const Color borderColor = Theme::defaultAreaBorderColor,
            const Color textColor = Theme::defaultAreaTextColor
        ): 
            Painter(width, height, zoom),
            gfx(gfx),
            left(left), 
            top(top),
            text(text), 
            textAlign(textAlign), 
            border(border), 
            backgroundColor(backgroundColor), 
            areaMargin(areaMargin),
            textPadding(textPadding),
            borderColor(borderColor),
            textColor(textColor)
        {
            setScrollXY12MinMax(0, 0, width, height);
        }

        virtual ~Area() {
            // TODO: manage area pointers: vector_destroy<Area>(areas);
        }

        bool isCalcScrollOnly() const {
            return calcScrollOnly;
        }

        void setCalcScrollOnly(bool calcScrollOnly) {
            this->calcScrollOnly = calcScrollOnly;
        }

        const Rectangle& getViewport(Rectangle& viewport) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            viewport.set(l, t, r, b);
            return viewport;
        }

        void brush(Color color) const override {
            gfx.setColor(color);
        }

        void point(int x, int y) override {
            setScrollXYMinMax(x, y);
            if (calcScrollOnly) return;
            prepare(x, y);
            gfx.drawPoint(x, y);
        }

        void rect(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            if (calcScrollOnly) return;
            prepare(x1, y1, x2, y2);
            gfx.drawRectangle(x1, y1, x2, y2);
        }

        void fRect(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            if (calcScrollOnly) return;
            prepare(x1, y1, x2, y2);
            gfx.fillRectangle(x1, y1, x2, y2);
        }

        void line(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            if (calcScrollOnly) return;
            prepare(x1, y1, x2, y2);
            gfx.drawLine(x1, y1, x2, y2);
        }

        void hLine(int x1, int y1, int x2) override {
            int y2 = y1;
            setScrollXY12MinMax(x1, y1, x2, y2);
            if (calcScrollOnly) return;
            prepare(x1, y1, x2, y2);
            gfx.drawHorizontalLine(x1, y1, x2);
        }

        void vLine(int x1, int y1, int y2) override {
            int x2 = x1;
            setScrollXY12MinMax(x1, y1, x2, y2);
            if (calcScrollOnly) return;
            prepare(x1, y1, x2, y2);
            gfx.drawVerticalLine(x1, y1, y2);
        }

        void font(const char* font) const override {
            gfx.setFont(font);
        }

        void write(int x, int y, const string &text) override {
            setScrollXYMinMax(x, y);
            if (calcScrollOnly) return;
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
            setScrollXYMinMax(
                area.left + area.width + areaMargin,
                area.top + area.height + areaMargin
            );
            if (calcScrollOnly) return area;
            area.setParent(this);
            areas.push_back(&area);
            return area;
        }

        int getTop(bool withParent = true) const {
            if (!withParent) return top;
            Area* parent = getParent();
            return top + (parent ? parent->getTop() - parent->scrollY : 0);
        }

        void setTop(int top) {
            this->top = top;
        }

        int getLeft(bool withParent = true) const {
            if (!withParent) return left;
            Area* parent = getParent();
            return left + (parent ? parent->getLeft() - parent->scrollX : 0);
        }

        void setLeft(int left) {
            this->left = left;
        }

        int getRight(bool withParent = true) const {
            return getLeft(withParent) + width;
        }

        int getRight(int left) const {
            return left + width;
        }

        int getRight(int left, int width) const {
            return left + width;
        }

        int getBottom(bool withParent = true) const {
            return getTop(withParent) + height;
        }

        int getBottom(int top) const {
            return top + height;
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

        bool contains(const Area* area) const {
            int l = area->getLeft();
            int t = area->getTop();
            int r = area->getRight(l);
            int b = area->getBottom(t);
            return contains(l, t, r, b);
        }

        void propagateTouch(unsigned int button, int x, int y) {
            if (!contains(x, y)) return;
            for (const onTouchHandler& onTouch: onTouchHandlers)
                onTouch(this, button, x - this->getLeft(true), y - this->getTop(true));
            for (Area* area: areas)
                if (contains(area)) area->propagateTouch(button, x, y);
        }

        void propagateRelease(unsigned int button, int x, int y) {
            for (const onReleaseHandler& onRelease: onReleaseHandlers)
                onRelease(this, button, x - this->getLeft(true), y - this->getTop(true));
            for (Area* area: areas)
                if (contains(area)) area->propagateRelease(button, x, y);
        }

        void propagateMove(int x, int y) {
            for (const onMoveHandler& onMove: onMoveHandlers)
                onMove(this, x - this->getLeft(true), y - this->getTop(true));
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

        virtual void draw() override {
            int t = getTop();
            int l = getLeft();
            int w = width;
            int h = height;
            int r = getRight(l, w);
            int b = getBottom(t, h);

            Rectangle viewport(l, t, r, b);
            reduceViewport(viewport);
            gfx.setViewport(viewport);
            
            gfx.setColor(getBackgroundColor());
            gfx.fillRectangle(l, t, r, b);

            drawBorder(l, t, r, b);

            if(!text.empty()) {
                int txtWidth, txtHeight;
                gfx.getTextSize(text, txtWidth, txtHeight);
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
                gfx.setColor(textColor);
                gfx.writeText(txtLeft, txtTop, text);
            }

            for (Area* area: areas)
                if (contains(area)) area->draw();

            for (const onDrawHandler& onDraw: onDrawHandlers) {
                onDraw(this);
            }

        }
    };

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
            GFX& gfx, Zoom& zoom, 
            int width, int height, 
            const char* title = Theme::defaultWindowTitle, 
            Color color = Theme::defaultWindowColor
        ):
            Area(gfx, zoom, 0, 0, width, height, "")
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

    class Frame: public Area {
    public:

        static const Border defaultFrameBorder = BUTTON_PUSHED; // TODO: to theme
        static const Color defaultFrameBackgroundColor = Theme::frameBackgroundColor;

    protected:

        bool drag = false;
        int dragStartedX = 0, dragStartedY = 0, dragScrollStartedX = 0, dragScrollStartedY = 0;

        static void touchHandler(void* context, unsigned int, int x, int y) {
            Frame* that = (Frame*)(context);

            // drag & scroll only if no child in the event focus
            for (Area* area: that->areas)
                if (area->contains(x, y)) return; // cppcheck-suppress useStlAlgorithm

            that->drag = true;
            that->dragStartedX = x;
            that->dragStartedY = y;
            that->dragScrollStartedX = that->scrollX;
            that->dragScrollStartedY = that->scrollY;
        }
        
        static void releaseHandler(void* context, unsigned int, int, int) {
            Frame* that = (Frame*)(context);
            that->drag = false;
        }
        
        static void moveHandler(void* context, int x, int y) {
            Frame* that = (Frame*)(context);
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

        Frame(
            GFX& gfx, Zoom& zoom, 
            int left, int top, 
            int width, int height,
            Border border = Theme::defaultFrameBorder,
            Color backgroundColor = Theme::defaultFrameBackgroundColor
        ):
            Area(
                gfx, zoom, 
                left, top, 
                width, height, 
                "", CENTER, 
                border, backgroundColor
            )
        {
            addTouchHandler(touchHandler);
            addReleaseHandler(releaseHandler);
            addMoveHandler(moveHandler);
        }
    };


    class Button: public Area {
    protected:
        
        static void touchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)(context);
            
            if (that->sticky) {
                that->pushed ? that->release() : that->push();                
                return;
            }
            
            if (!that->pushed) that->push();
        }
        
        static void releaseHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)(context);

            if (that->sticky) return;
                
            if (that->pushed) that->release();
        }

        bool pushed = false;
        bool sticky = false;

    public:

        Button(
            GFX& gfx, Zoom& zoom, 
            int left, int top, 
            int width, int height, 
            const string &text = "", 
            const Align textAlign = Theme::defaultButtonTextAlign,
            const Border buttonBorder = Theme::defaultButtonBorder
        ): Area(
            gfx, zoom, left, top, width, height, text, 
            textAlign, buttonBorder
        ) {
            addTouchHandler(touchHandler);
            addReleaseHandler(releaseHandler);
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
        Label(GFX& gfx, Zoom& zoom, int left, int top, int width, int height,
            const string &text, const Align textAlign = defaultLabelTextAlign
        ): Area(gfx, zoom, left, top, width, height, text, textAlign, NONE) // TODO: border = NONE to theme
        {}
    };


    enum Direction { HORIZONTAL, VERTICAL };

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
            // logger.writeln(that->value);
            that->draw();
        }

    protected:

        bool dragStarted = false;
        // Point<int> dragStartAt = Point<int>(-1, -1);
        int dragStartAt = -1;

        Button* handler = NULL;

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
            GFX& gfx, Zoom& zoom, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, double value = 0,
            int thickness = Theme::barThickness
        ): Area(
            gfx, zoom, left, top, 
            direction == HORIZONTAL ? length : thickness,
            direction == VERTICAL ? length : thickness, 
            "", CENTER, BUTTON_PUSHED
        ),  
            direction(direction), thickness(thickness),
            minValue(minValue), maxValue(maxValue), value(value),
            size(calcSize()),
            maxPosition(calcMaxPosition()), 
            position(calcPosition())
        {
            handler = direction == HORIZONTAL ?
                new Button(gfx, zoom, position, 1, thickness - 2, thickness - 2):
                new Button(gfx, zoom, 1, position, thickness - 2, thickness - 2);
            this->child(*handler);
            addTouchHandler(handlerTouch);
            addReleaseHandler(handlerRelease);
            addMoveHandler(handlerMove);
        }

        ~SlideBar() {
            delete handler;
        }

        double getValue() {
            return value;
        }
    };

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
            GFX& gfx, Zoom& zoom, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, 
            double value = 0, double valueSize = .3,
            int thickness = Theme::barThickness
        ): SlideBar(
            gfx, zoom, left, top, length, direction, 
            minValue, maxValue, value, thickness
        ), valueSize(valueSize)
        {
            this->size = calcSize();
            this->maxPosition = calcMaxPosition();
            handler->setWidth(direction == HORIZONTAL ? size : thickness - 2);
            handler->setHeight(direction == VERTICAL ? size : thickness - 2);
        }

        double getValueSize() {
            return valueSize;
        }
    };

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
                logger.writeln("drag min ", x, ":", y, " - from:", that->dragStartAt);

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
                logger.writeln("drag max ", x, ":", y, " - from:", that->dragStartAt);

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

        Button* handlerMin = NULL;
        Button* handlerMax = NULL;
    public:
        IntervalBar(
            GFX& gfx, Zoom& zoom, int left, int top, int length, bool direction,
            double minValue = 0, double maxValue = 1, 
            double value = 0, double valueSize = .3,
            int thickness = Theme::barThickness
        ): ScrollBar(
            gfx, zoom, left, top, length, direction,
            minValue, maxValue, value, valueSize, thickness
        ) 
        {
            handlerMin = direction == HORIZONTAL ?
                new Button(gfx, zoom, 0, 0, thickness / 3, thickness):
                new Button(gfx, zoom, 0, 0, thickness, thickness / 3);
            handlerMax = direction == HORIZONTAL ?
                new Button(gfx, zoom, size - thickness / 3, 0, thickness / 3, thickness):
                new Button(gfx, zoom, 0, size - thickness / 3, thickness, thickness / 3);
            handler->child(*handlerMin);
            handler->child(*handlerMax);
            handlerMin->addTouchHandler(handlerMinTouch);
            handlerMax->addTouchHandler(handlerMaxTouch);
            addReleaseHandler(handlerRelease);
            addMoveHandler(handlerMove);
        }

        ~IntervalBar() {
            delete handlerMin;
            delete handlerMax;
        }
    };


    class Accordion: public Area {
    public:

        class Container;

        class Toggler: public Button {
        protected:
            
            static void toggleHandler(void* context, unsigned int, int, int) {
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
            Toggler(
                Container& container, GFX& gfx, Zoom& zoom, 
                size_t containerIndex, 
                int left, int top, int width, int height, 
                const string &text, 
                const Align textAlign = Theme::defaultAccordionTogglerTextAlign
            ): 
                Button(gfx, zoom, left, top, width, height, text, textAlign),
                container(container),
                containerIndex(containerIndex)
            {
                sticky = container.getAccordion().isSticky();
                addTouchHandler(toggleHandler);
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
            int frameHeight;
            bool opened = false;
        public:
            Container(
                Accordion& accordion, Zoom& zoom,
                const string& title, const Align textAlign, 
                int frameHeight
            ): 
                accordion(accordion), frameHeight(frameHeight) 
            {
                GFX& gfx = accordion.getGFX();
                const int width = accordion.width;
                size_t accordionContainerAt = accordion.getContainers().size();
                const int togglerTop = accordion.height + innerBorderSize;

                toggler = new Toggler(*this, gfx, zoom, accordionContainerAt,
                    innerBorderSize, togglerTop, 
                    width - innerBorderSize*2, 
                    togglerHeight - innerBorderSize*2, 
                    title, textAlign);

                frame = new Frame(gfx, zoom, 
                    innerBorderSize, togglerTop + togglerHeight - innerBorderSize*2,
                    width, 0, NONE, accordion.getBackgroundColor());
            
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
        bool sticky;
        bool single;
        bool one;

    public:
        Accordion(GFX& gfx, Zoom& zoom, int left, int top, int width,
            bool sticky = false, bool single = false, bool one = true, // TODO
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor
        ): 
            Area(
                gfx, zoom, left, top, width, 0, "", CENTER,
                border, backgroundColor
            ),
            sticky(sticky),
            single(single),
            one(one)
        {}

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

        Container& createContainer(Zoom& zoom, const string& title, int frameHeight) {
            return *vector_create(containers, *this, zoom, title, textAlign, frameHeight);
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

        GFX gfx = GFX(this);
        Zoom zoom;
        GUI gui = GUI(gfx, zoom, 1600, 900, "Application");

    public:

        virtual ~Application() {}

        Application* run() {
            try {
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
        Frame mainFrame = Frame(
            gfx, zoom, 0, 0, 
            gui.getWidth(), gui.getHeight(), 
            NONE, Theme::defaultWindowColor
        );
    public:
        virtual void init() override {
            gui.child(mainFrame);
        }
    };

}
