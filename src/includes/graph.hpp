#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <stdexcept>
#include <vector>
#include "Log.hpp"
#include "Tools.hpp"

using namespace std;

namespace graph {

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
                    throw runtime_error("Invalid color");                    
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
                    throw runtime_error("Invalid color");                    
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
        static const char* windowFont;
        static const Align textAlign = CENTER;
        static const Color borderColor = gray;
        static const Color textColor = black;
        static const Color backgroundColor = gray;
        static const Color scrollBackgroundColor = darkGray;
        static const int scrollMargin = 10;
        static const int textPadding = 10;
        static const Align labelTextAlign = LEFT;
    };
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
        static const char* defaultWindowFont;

    protected:

        static Display *display;
        Window window;
        GC gc;
        XFontStruct *fontInfo = NULL;

        Rectangle viewport;

    public:

        bool close = false;

        void setViewport(Rectangle viewport) {
            this->viewport = viewport;
        }

        void openWindow(int width, int height, Color color = defaultWindowColor, const char* font = defaultWindowFont) {
            // Initialize the X display
            if (!display) display = XOpenDisplay(NULL);
            if (!display) throw runtime_error("Unable to open display.");

            // int screen = DefaultScreen(display);

            // Create the window
            window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, (unsigned)width, (unsigned)height, 1, color, color);
            XMapWindow(display, window);

            // Create a graphics context
            gc = XCreateGC(display, window, 0, NULL);

            // Select inputs
            XSelectInput(display, window, 
                ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
                ButtonReleaseMask | PointerMotionMask);

            setFont(font);

            eventContext = this;
        }

        void closeWindow() const {            
            if (fontInfo) XFreeFont(display, fontInfo);
            XFreeGC(display, gc);
            XDestroyWindow(display, window);
            XCloseDisplay(display);
        }

        void getWindowSize(int &width, int &height) const {            
            XWindowAttributes attr;
            XGetWindowAttributes(display, window, &attr);
            width = attr.width;
            height = attr.height;
        }

        void clearWindow(Color color) const {            
            XSetForeground(display, gc, color);
            int width, height;
            getWindowSize(width, height);
            XFillRectangle(display, window, gc, 0, 0, (unsigned)width, (unsigned)height);
        }

        void drawRectangle(int x1, int y1, int x2, int y2, Color color) const {
            Rectangle rect(x1, y1, x2, y2);
            XSetForeground(display, gc, color);
            if (rect.insideOf(viewport)) {
                XDrawRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
                return;
            }
            if (rect.containsPartially(x1, y1, x2, y1)) drawHorizontalLine(x1, y1, x2, color);
            if (rect.containsPartially(x1, y2, x2, y2)) drawHorizontalLine(x1, y2, x2, color);
            if (rect.containsPartially(x2, y1, x2, y2)) drawVerticalLine(x2, y1, y2, color);
            if (rect.containsPartially(x1, y1, x1, y2)) drawVerticalLine(x1, y1, y2, color);
        }

        void fillRectangle(int x1, int y1, int x2, int y2, Color color) const {
            Rectangle rect(x1, y1, x2, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);

            XSetForeground(display, gc, color);
            XFillRectangle(display, window, gc, rect.x1, rect.y1, (unsigned)(rect.x2 - rect.x1), (unsigned)(rect.y2 - rect.y1));
        }

        void drawLine(int x1, int y1, int x2, int y2, Color color) const {
            Rectangle rect(x1, y1, x2, y2);
            
            if (x1 == x2) {
                drawVerticalLine(x1, y1, y2, color);
                return;
            }
            
            if (y1 == y2) {
                drawHorizontalLine(x1, y1, x2, color);
                return;
            }

            // Fix the line first, it's always goes left to right
            if (x1 > x2) {
                swap(x1, x2);
                swap(y1, y2);
            }

            if (!viewport.containsCompletely(rect)) {
                // Clip the line to fit within the viewport
                    
                if (x1 < viewport.x1) {
                    if (x2 < viewport.x1) return; // Stop if out of scope...

                    // Clip the left overflow at viewport.x1 and assign the new values to x1 and y1
                    y1 = y2 - ((y2 - y1) * (x2 - viewport.x1)) / (x2 - x1);
                    x1 = viewport.x1;
                }

                if (y1 < viewport.y1) {
                    if (y2 < viewport.y1) return; // Stop if out of scope...

                    // Clip the top overflow at viewport.y1 and assign the new values to x1 and y1
                    x1 = x1 + ((x2 - x1) * (viewport.y1 - y1)) / (y2 - y1);
                    y1 = viewport.y1;
                }

                if (x2 > viewport.x2) {
                    if (x1 > viewport.x2) return; // Stop if out of scope...

                    // Clip the right overflow at viewport.x2 and assign the new values to x2 and y2
                    y2 = y2 - ((y2 - y1) * (x2 - viewport.x2)) / (x2 - x1);
                    x2 = viewport.x2;
                }

                if (y2 > viewport.y2) {
                    if (y1 > viewport.y2) return; // Stop if out of scope...

                    // Clip the bottom overflow at viewport.y2 and assign the new values to x2 and y2
                    x2 = x1 + ((y2 - y1) * (viewport.y2 - y1)) / (x2 - x1);
                    y2 = viewport.y2;
                }

                if (y1 > y2) { // rise

                    if (y2 < viewport.y1) {
                        if (y1 < viewport.y1) return; // Stop if out of scope...

                        // Clip the top overflow at viewport.y1 and assign the new values to x2 and y2
                        x2 = x1 + ((x2 - x1) * (y1 - viewport.y1)) / (y1 - y2);
                        y2 = viewport.y1;
                    }

                    if (y1 > viewport.y2) {
                        if (y2 > viewport.y2) return; // Stop if out of scope...

                        // Clip the bottom overflow at viewport.y2 and assign the new values to x1 and y1
                        x1 = x2 - ((x2 - x1) * (viewport.y2 - y2)) / (y1 - y2);
                        y1 = viewport.y2;
                    }

                }
            }

            // Set the foreground color and draw the clipped line
            XSetForeground(display, gc, color);
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void drawVerticalLine(int x1, int y1, int y2, Color color) const {
            Rectangle rect(x1, y1, x1, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);

            XSetForeground(display, gc, color);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x1, rect.y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2, Color color) const {
            Rectangle rect(x1, y1, x2, y1);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);

            XSetForeground(display, gc, color);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x2, rect.y1);
        }

        void setFont(const char* font) {            
            fontInfo = XLoadQueryFont(display, font);
            if (!fontInfo)
            {
                LOG("Unable to load font: %s\n", font);
                throw runtime_error("Unable to load font.\n");
            }
            XSetFont(display, gc, fontInfo->fid);
        }
        
        void writeText(int x, int y, const string text, Color color) {
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

            XSetForeground(display, gc, color);
            XDrawString(display, window, gc, x, y, txt.c_str(), (int)txt.length());
        }

        void getTextSize(const string text, int &width, int &height) const {            
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

                int width, height;
                KeySym key;
                char text[32];
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
                        throw runtime_error("Unhandled event type: " + to_string(event.type));
                        break;
                }
            }
        }
    };

    Display* GFX::display = NULL;
    const char* GFX::defaultWindowFont = Theme::windowFont;



    class Area: public EventHandler {
    public:

        static const Align defaultAreaTextAlign = Theme::textAlign;
        static const Color defaultAreaBorderColor = Theme::borderColor;
        static const Color defaultAreaTextColor = Theme::textColor;
        static const Border defaultAreaBorder = NONE; // TODO: to theme
        static const Color defaultAreaBackgroundColor = GFX::defaultWindowColor;
        static const int defaultScrollMargin = Theme::scrollMargin;
        static const int defaultTextMargin = Theme::textPadding;

        typedef void (*onDrawHandler)(void*);
        
        vector<onDrawHandler> onDrawHandlers;

    protected:
        GFX* gfx = NULL;

        const int left, top;
        int width, height;
        const string text;
        const Align textAlign;
        Color borderColor = defaultAreaBorderColor;
        Color textColor = defaultAreaTextColor;
        Border border;
        Color backgroundColor;
        int scrollMargin;
        int textPadding;

        vector<Area*> areas;
        Area* parent = NULL;
        int scrollX = 0, scrollY = 0, scrollXMax = 0, scrollYMax = 0;

        void forceScrollInRange() {
            if (scrollX < 0) scrollX = 0;
            if (scrollY < 0) scrollY = 0;
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

        void prepare(int &x1, int &y1, int &x2, int &y2) const {
            int leftAndScroll = getLeft() - scrollX;
            int topAndScroll = getTop() - scrollY;
            x1 += leftAndScroll;
            y1 += topAndScroll;
            x2 += leftAndScroll;
            y2 += topAndScroll;

            Rectangle viewport;
            getViewport(viewport);
            gfx->setViewport(viewport);
        }

    public:

        Area(GFX* gfx, int left, int top, int width, int height, 
            const string text = "", const Align textAlign = defaultAreaTextAlign,
            const Border border = defaultAreaBorder,
            const Color backgroundColor = defaultAreaBackgroundColor,
            const int scrollMargin = defaultScrollMargin,
            const int textPadding = defaultTextMargin
        ):
            gfx(gfx),
            left(left), top(top), width(width), height(height), 
            text(text), textAlign(textAlign), border(border), 
            backgroundColor(backgroundColor), scrollMargin(scrollMargin),
            textPadding(textPadding)
        {
            setScrollXYMax(width, height);
        }

        const Rectangle& getViewport(Rectangle& viewport) const {
            int top = getTop();
            int left = getLeft();
            int right = getRight(left);
            int bottom = getBottom(top);
            viewport.set(left, top, right, bottom);
            return viewport;
        }

        void rect(int x1, int y1, int x2, int y2, Color color) const {
            prepare(x1, y1, x2, y2);
            gfx->drawRectangle(x1, y1, x2, y2, color);
        }

        void fillRect(int x1, int y1, int x2, int y2, Color color) const {
            prepare(x1, y1, x2, y2);
            gfx->fillRectangle(x1, y1, x2, y2, color);
        }

        void line(int x1, int y1, int x2, int y2, Color color) const {
            prepare(x1, y1, x2, y2);
            gfx->drawLine(x1, y1, x2, y2, color);
        }

        void hLine(int x1, int y1, int x2, Color color) const {
            int y2;
            prepare(x1, y1, x2, y2);
            gfx->drawHorizontalLine(x1, y1, x2, color);
        }

        void vLine(int x1, int y1, int y2, Color color) const {
            int x2;
            prepare(x1, y1, x2, y2);
            gfx->drawVerticalLine(x1, y1, y2, color);
        }

        GFX* getGFX() const {
            return gfx;
        }

        void setParent(Area* parent) {
            this->parent = parent;
        }

        Area* getParent() const {
            return parent;
        } 

        void child(Area* area) {
            area->setParent(this);
            areas.push_back(area);
            setScrollXYMax(
                area->left + area->width + scrollMargin,
                area->top + area->height + scrollMargin
            );
        }

        void setScrollXYMax(int x, int y) {
            int xMax = x - width;
            int yMax = y - height;
            if (xMax > scrollXMax) scrollXMax = xMax;
            if (yMax > scrollYMax) scrollYMax = yMax;
            forceScrollInRange();
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

        int getTop() const {
            Area* parent = getParent();
            return top + (parent ? parent->getTop() - parent->getScrollY() : 0);
        }

        int getLeft() const {
            Area* parent = getParent();
            return left + (parent ? parent->getLeft() - parent->getScrollX() : 0);
        }        

        int getWidth() const {
            return width;
        }

        void setWidth(int width) {
            this->width = width;
        }

        int getHeight() const {
            return height;
        }

        void setHeight(int height) {
            this->height = height;
        }

        int getRight() const {
            return getLeft() + getWidth();
        }

        int getRight(int left) const {
            return left + getWidth();
        }

        int getRight(int left, int width) const {
            return left + width;
        }

        int getBottom() const {
            return getTop() + getHeight();
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
            int top = getTop();
            int left = getLeft();
            int right = getRight(left);
            int bottom = getBottom(top);
            return
                x >= left && x <= right &&
                y >= top && y <= bottom;
        }

        bool contains(int x1, int y1, int x2, int y2) const {
            int top = getTop();
            int left = getLeft();
            int right = getRight(left);
            int bottom = getBottom(top);
            return !(left >= x2 || x1 >= right || top >= y2 || y1 >= bottom);
        }

        bool contains(Area* area) const {
            int left = area->getLeft();
            int top = area->getTop();
            int right = area->getRight(left);
            int bottom = area->getBottom(top);
            return contains(left, top, right, bottom);
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
            Border border = getBorder();
            Color borderColor;
            Color borderColorLight;
            Color borderColorDark;
            switch (border) {

                case NONE:
                    break;

                case SIMPLE:
                    gfx->drawRectangle(left, top, right, bottom, getBorderColor());
                    break;

                case BUTTON_RELEASED:
                    // button borders get the background color
                    borderColor = getBackgroundColor();
                    borderColorLight = ColorMixer::light(borderColor);
                    borderColorDark = ColorMixer::dark(borderColor);
                    gfx->drawHorizontalLine(left, top, right, borderColorLight);
                    gfx->drawVerticalLine(left, top, bottom, borderColorLight);
                    gfx->drawHorizontalLine(left, bottom, right, borderColorDark);
                    gfx->drawVerticalLine(right, top, bottom, borderColorDark);
                    break;

                case BUTTON_PUSHED:
                    // button borders get the background color
                    borderColor = getBackgroundColor();
                    borderColorLight = ColorMixer::light(borderColor);
                    borderColorDark = ColorMixer::dark(borderColor);
                    gfx->drawHorizontalLine(left, bottom, right, borderColorLight);
                    gfx->drawVerticalLine(right, top, bottom, borderColorLight);
                    gfx->drawHorizontalLine(left, top, right, borderColorDark);
                    gfx->drawVerticalLine(left, top, bottom, borderColorDark);
                    break;

                default:
                    throw runtime_error("Invalid border");
                    break;
            }
        }

        void drawBorder() {
            int top = getTop();
            int left = getLeft();
            int right = getRight(left);
            int bottom = getBottom(top);

            Rectangle viewport(left, top, right, bottom);
            reduceViewport(viewport);
            gfx->setViewport(viewport);

            drawBorder(left, top, right, bottom);
        }

        void draw() {
            int top = getTop();
            int left = getLeft();
            int width = getWidth();
            int height = getHeight();
            int right = getRight(left, width);
            int bottom = getBottom(top, height);

            Rectangle viewport(left, top, right, bottom);
            reduceViewport(viewport);
            gfx->setViewport(viewport);
            
            gfx->fillRectangle(left, top, right, bottom, getBackgroundColor());

            drawBorder(left, top, right, bottom);

            const string text = getText();
            if(!text.empty()) {
                int textWidth, textHeight;
                gfx->getTextSize(text, textWidth, textHeight);
                int textLeft;
                Align textAlign = getTextAlign();
                switch (textAlign) {

                    case CENTER:
                        textLeft = left + ((width - textWidth) / 2);
                        break;

                    case LEFT:
                        textLeft = left + textPadding;
                        break;

                    case RIGHT:
                        textLeft = left + width - textPadding - textWidth;
                        break;
                    
                    default:
                        throw runtime_error("Invalid text align");
                        break;
                }
                int textTop = top + ((height - textHeight) / 2) + 16; // ??16
                gfx->writeText(textLeft, textTop, text, getTextColor());
            }

            for (Area* area: areas)
                if (contains(area)) area->draw();

            for (const onDrawHandler& onDraw: onDrawHandlers) onDraw(this);

        }
    };

    class GUI: public Area {
    protected:

        static void resize(void* context, int width, int height) {
            GUI* that = (GUI*)context;
            that->setWidth(width);
            that->setHeight(height);
            that->draw();
        }

        static void touch(void* context, unsigned int button, int x, int y) {
            Area* that = (Area*)context;
            that->propagateTouch(button, x, y);
        }

        static void release(void* context, unsigned int button, int x, int y) {
            Area* that = (Area*)context;
            that->propagateRelease(button, x, y);
        }

        static void move(void* context, int x, int y) {
            Area* that = (Area*)context;
            that->propagateMove(x, y);
        }

        void init(int width, int height, Color color = GFX::defaultWindowColor) {
            gfx->openWindow(width, height, color);
            gfx->eventContext = this;
            gfx->onResizeHandlers.push_back(resize);
            gfx->onTouchHandlers.push_back(touch);
            gfx->onReleaseHandlers.push_back(release);
            gfx->onMoveHandlers.push_back(move);
        }

    public:
        GUI(GFX* gfx, int width, int height, Color color = GFX::defaultWindowColor):
            Area(gfx, 0, 0, width, height, "", defaultAreaTextAlign, defaultAreaBorder) 
        {
            init(width, height, color);
        }

        ~GUI() {
            gfx->closeWindow();
        }

        void loop(unsigned long ms = GFX::defaultLoopMs) const {
            gfx->eventLoop(ms);
        }
    };

    class Frame: public Area {
    public:

        static const Border defaultScrollBorder = BUTTON_PUSHED; // TODO: to theme
        static const Color defaultScrollBackgroundColor = Theme::scrollBackgroundColor;

    protected:

        bool drag = false;
        int dragStartedX, dragStartedY, dragScrollStartedX, dragScrollStartedY;

        static void touch(void* context, unsigned int, int x, int y) {
            Frame* that = (Frame*)context;
            that->drag = true;
            that->dragStartedX = x;
            that->dragStartedY = y;
            that->dragScrollStartedX = that->getScrollX();
            that->dragScrollStartedY = that->getScrollY();
        }
        
        static void release(void* context, unsigned int, int, int) {
            Frame* that = (Frame*)context;
            that->drag = false;
        }
        
        static void move(void* context, int x, int y) {
            Frame* that = (Frame*)context;
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

        Frame(GFX* gfx, int left, int top, int width, int height,
            const Border border = defaultScrollBorder,
            const Color backgroundColor = defaultScrollBackgroundColor
        ): Area(gfx, left, top, width, height, "", CENTER, border, backgroundColor)
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
            onMoveHandlers.push_back(move);
        }
    };


    class Button: public Area {
    protected:

        bool pushed = false;
        
        static void touch(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;
            
            if (that->sticky) {
                that->pushed ? that->release() : that->push();                
                return;
            }
            
            if (!that->pushed) that->push();
        }
        
        static void release(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;

            if (that->sticky) return;
                
            if (that->pushed) that->release();
        }

    public:
        bool sticky = false;

        Button(GFX* gfx, int left, int top, int width, int height, 
            const string text, const Align textAlign = Area::defaultAreaTextAlign
        ): Area(gfx, left, top, width, height, text, textAlign, BUTTON_RELEASED) // TODO: border = BUTTON_RELEASED to theme
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
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
        Label(GFX* gfx, int left, int top, int width, int height, 
            const string text, const Align textAlign = defaultLabelTextAlign
        ): Area(gfx, left, top, width, height, text, textAlign, NONE) // TODO: border = NONE to theme
        {}
    };
    
}
