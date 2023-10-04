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

namespace gfx {

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
        int x1, y1, x2, y2;
        Rectangle(int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0):
            x1(x1), y1(y1), x2(x2), y2(y2)
        {
            if (this->x1 > this->x2) Tools::replace(this->x1, this->x2);
            if (this->y1 > this->y2) Tools::replace(this->y1, this->y2);
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
    };

    class GraphicsWindow: public EventHandler {
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

        void setupDrawing(int &x1, int &y1, int &x2, int &y2, Color color) const {
            XSetForeground(display, gc, color);
            if (x1 > x2) Tools::replace(x1, x2);
            if (y1 > y2) Tools::replace(y1, y2);
            Rectangle intersection(x1, y1, x2, y2);
            intersection.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            x1 = intersection.x1;
            y1 = intersection.y1;
            x2 = intersection.x2;
            y2 = intersection.y2;
        }

        void setupWriting(int& x, int& y, string& text, Color color) {
            XSetForeground(display, gc, color);            
            while (true) {
                int width, height;
                getTextSize(text, width, height);
                const int textYFixer4 = 4; // ??4
                int x1 = x;
                int y1 = y + textYFixer4;
                int x2 = x + width;
                int y2 = y - height + textYFixer4;
                if (x1 > x2) Tools::replace(x1, x2);
                if (y1 > y2) Tools::replace(y1, y2);
                
                if (y1 < viewport.y1 || y2 > viewport.y2) text = "";
                if (text.empty()) return;
                if (x1 < viewport.x1) {
                    // Extract the first character.
                    getTextSize(text.substr(0, 1), width, height); 
                    x += width;
                    text = text.substr(1); // This removes the first character.
                    continue;
                }
                if (x2 > viewport.x2) {
                    // Extract the last character.
                    getTextSize(text.substr(text.length() - 1), width, height); 
                    text = text.substr(0, text.length() - 1); // Remove the last character.
                    continue;
                }
                break;
            }
        }

    public:

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
            setupDrawing(x1, y1, x2, y2, color);
            XDrawRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        void fillRectangle(int x1, int y1, int x2, int y2, Color color) const {            
            setupDrawing(x1, y1, x2, y2, color);
            XFillRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        // void drawLine(int x1, int y1, int x2, int y2, Color color) const {            
        //     setupDrawing(x1, y1, x2, y2, color);
        //     XDrawLine(display, window, gc, x1, y1, x2, y2);
        // }

        void drawVerticalLine(int x1, int y1, int y2, Color color) const {            
            setupDrawing(x1, y1, x1, y2, color);
            XDrawLine(display, window, gc, x1, y1, x1, y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2, Color color) const {            
            setupDrawing(x1, y1, x2, y1, color);
            XDrawLine(display, window, gc, x1, y1, x2, y1);
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
            string txt = text;
            setupWriting(x, y, txt, color);
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
            LOG("Window event loop starts");
            
            while (true) {

                if (!onLoopHandlers.empty() && XPending(display) <= 0) {
                    Tools::sleep(ms);
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
                        LOG("Windows expose: ", width, ":", height);
                        if (!onResizeHandlers.empty())
                            for (const onResizeHandler& onResize: onResizeHandlers)
                                onResize(eventContext, width, height);
                        break;

                    case KeyPress:
                        // Handle key press event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        if (!onKeyPressHandlers.empty()) 
                            for (const onKeyPressHandler& onKeyPress: onKeyPressHandlers)
                                onKeyPress(eventContext, key);
                        break;

                    case KeyRelease:
                        // Handle key release event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        if (!onKeyReleaseHandlers.empty()) 
                            for (const onKeyReleaseHandler& onKeyRelease: onKeyReleaseHandlers)
                                onKeyRelease(eventContext, key);
                        break;

                    case ButtonPress:
                        // Handle mouse button press event
                        if (!onTouchHandlers.empty()) 
                            for (const onTouchHandler& onTouch: onTouchHandlers)
                                onTouch(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case ButtonRelease:
                        // Handle mouse button release event
                        if (!onReleaseHandlers.empty()) 
                            for (const onReleaseHandler& onRelease: onReleaseHandlers)
                                onRelease(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case MotionNotify:
                        // Handle mouse motion event
                        if (!onMoveHandlers.empty()) 
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

    Display* GraphicsWindow::display = NULL;
    const char* GraphicsWindow::defaultWindowFont = Theme::windowFont;



    class Area: public EventHandler {
    public:

        static const Align defaultAreaTextAlign = Theme::textAlign;
        static const Color defaultAreaBorderColor = Theme::borderColor;
        static const Color defaultAreaTextColor = Theme::textColor;
        static const Border defaultAreaBorder = NONE;
        static const Color defaultAreaBackgroundColor = GraphicsWindow::defaultWindowColor;
        static const int defaultScrollMargin = Theme::scrollMargin;

        typedef void (*onDrawHandler)(void*);
        
        onDrawHandler onDraw = NULL;

    protected:
        GraphicsWindow* gwin = NULL;

        const int left, top;
        int width, height;
        const string text;
        const Align textAlign;
        Color borderColor = defaultAreaBorderColor;
        Color textColor = defaultAreaTextColor;
        Border border;
        Color backgroundColor;
        int scrollMargin;

        vector<Area*> areas;
        Area* parent = NULL;
        int scrollX = 0, scrollY = 0, scrollXMax = 0, scrollYMax = 0;

        void forceScrollInRange() {
            if (scrollX < 0) scrollX = 0;
            if (scrollY < 0) scrollY = 0;
            if (scrollX > scrollXMax) scrollX = scrollXMax;
            if (scrollY > scrollYMax) scrollY = scrollYMax;
        }

        void reduceViewport(Rectangle& viewport) {
            if (parent) {
                int parentTop = parent->getTop();
                int parentLeft = parent->getLeft();
                int parentRight = parent->getRight(parentLeft);
                int parentBottom = parent->getBottom(parentTop);
                if (viewport.intersect(parentLeft, parentTop, parentRight, parentBottom)) {
                    parent->reduceViewport(viewport);
                }
            }
        }

    public:

        Area(GraphicsWindow* gwin, int left, int top, int width, int height, 
            const string text = "", const Align textAlign = defaultAreaTextAlign,
            const Border border = defaultAreaBorder,
            const Color backgroundColor = defaultAreaBackgroundColor,
            const int scrollMargin = defaultScrollMargin
        ):
            gwin(gwin),
            left(left), top(top), width(width), height(height), 
            text(text), textAlign(textAlign), border(border), 
            backgroundColor(backgroundColor), scrollMargin(scrollMargin)
        {}

        GraphicsWindow* getGraphicsWindow() const {
            return gwin;
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
            int xMax = (area->left + area->width) - width + scrollMargin;
            int yMax = (area->top + area->height) - height + scrollMargin;
            if (xMax > scrollXMax) scrollXMax = xMax;
            if (yMax > scrollYMax) scrollYMax = yMax;
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
            if (contains(x, y)) {
                if (!onTouchHandlers.empty()) 
                    for (const onTouchHandler& onTouch: onTouchHandlers)
                        onTouch(this, button, x, y);
                for (Area* area: areas) {
                    if (contains(area)) area->propagateTouch(button, x, y);
                }
            }
        }

        void propagateRelease(unsigned int button, int x, int y) {
            if (!onReleaseHandlers.empty()) 
                for (const onReleaseHandler& onRelease: onReleaseHandlers)
                    onRelease(this, button, x, y);
            for (Area* area: areas) {
                if (contains(area)) area->propagateRelease(button, x, y);
            }
        }

        void propagateMove(int x, int y) {
            if (!onMoveHandlers.empty()) 
                for (const onMoveHandler& onMove: onMoveHandlers)
                    onMove(this, x, y);
            for (Area* area: areas) {
                if (contains(area)) area->propagateMove(x, y);
            }
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
                    gwin->drawRectangle(left, top, right, bottom, getBorderColor());
                    break;

                case BUTTON_RELEASED:
                    // button borders get the background color
                    borderColor = getBackgroundColor();
                    borderColorLight = ColorMixer::light(borderColor);
                    borderColorDark = ColorMixer::dark(borderColor);
                    gwin->drawHorizontalLine(left, top, right, borderColorLight);
                    gwin->drawHorizontalLine(left, bottom, right, borderColorDark);
                    gwin->drawVerticalLine(left, top, bottom, borderColorLight);
                    gwin->drawVerticalLine(right, top, bottom, borderColorDark);
                    break;

                case BUTTON_PUSHED:
                    // button borders get the background color
                    borderColor = getBackgroundColor();
                    borderColorLight = ColorMixer::light(borderColor);
                    borderColorDark = ColorMixer::dark(borderColor);
                    gwin->drawHorizontalLine(left, top, right, borderColorDark);
                    gwin->drawHorizontalLine(left, bottom, right, borderColorLight);
                    gwin->drawVerticalLine(left, top, bottom, borderColorDark);
                    gwin->drawVerticalLine(right, top, bottom, borderColorLight);
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
            gwin->setViewport(viewport);

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
            gwin->setViewport(viewport);
            
            const string text = getText();
            LOG("fillRectangle (in draw): ", left, " ", top, " ", right, " ", bottom);
            gwin->fillRectangle(left, top, right, bottom, getBackgroundColor());

            drawBorder(left, top, right, bottom);
            
            if(!text.empty()) {
                int textWidth, textHeight;
                gwin->getTextSize(text, textWidth, textHeight);
                int textLeft, textTop;
                Align textAlign = getTextAlign();
                switch (textAlign) {

                    case CENTER:
                        textLeft = left + ((width - textWidth) / 2);
                        textTop = top + ((height - textHeight) / 2) + 16; // ??16
                        break;
                    
                    default:
                        throw runtime_error("Invalid text align");
                        break;
                }
                gwin->writeText(textLeft, textTop, text, getTextColor());
            }

            for (Area* area: areas) {
                if (contains(area)) area->draw();
            }

            if (onDraw) onDraw(this);
        }
    };

    class GUI: public Area {
    protected:
        // GraphicsWindow* gwin = NULL;

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

    public:
        GUI(int width, int height,
            Color color = GraphicsWindow::defaultWindowColor
        ):
            Area(NULL, 0, 0, width, height, "", defaultAreaTextAlign, defaultAreaBorder) 
        {
            gwin = new GraphicsWindow();
            gwin->openWindow(width, height, color);
            gwin->eventContext = this;
            gwin->onResizeHandlers.push_back(resize);
            gwin->onTouchHandlers.push_back(touch);
            gwin->onReleaseHandlers.push_back(release);
            gwin->onMoveHandlers.push_back(move);
        }

        ~GUI() {
            gwin->closeWindow();
            delete gwin;
        }

        void loop(unsigned long ms = GraphicsWindow::defaultLoopMs) const {
            gwin->eventLoop(ms);
        }
    };

    class Button: public Area {
    protected:

        bool pushed = false;
        
        static void touch(void* context, unsigned int button, int x, int y) {
            Button* that = (Button*)context;
            LOG("Button touch: ", button, " (", that->getText().c_str() , ") ", x, ":", y);
            that->setBorder(BUTTON_PUSHED);
            that->drawBorder();
            that->pushed = true;
        }
        
        static void release(void* context, unsigned int button, int x, int y) {
            Button* that = (Button*)context;
            if (!that->pushed) return;
            LOG("Button release: ", button, " (", that->getText().c_str() , ") ", " ", x, ":", y);
            that->setBorder(BUTTON_RELEASED);
            that->drawBorder();
            that->pushed = false;
        }

    public:
        Button(GraphicsWindow* gwin, int left, int top, int width, int height, 
            const string text, const Align textAlign = Area::defaultAreaTextAlign
        ): Area(gwin, left, top, width, height, text, textAlign, BUTTON_RELEASED) 
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
        }
    };

    class Drag: public Area {
    public:

        static const Border defaultScrollBorder = BUTTON_PUSHED;
        static const Color defaultScrollBackgroundColor = Theme::scrollBackgroundColor;

    protected:

        bool drag = false;
        int dragStartedX, dragStartedY, dragScrollStartedX, dragScrollStartedY;

        static void touch(void* context, unsigned int button, int x, int y) {
            Drag* that = (Drag*)context;
            LOG("Drag touch: ", button, " ", x, ":", y);
            that->drag = true;
            that->dragStartedX = x;
            that->dragStartedY = y;
            that->dragScrollStartedX = that->getScrollX();
            that->dragScrollStartedY = that->getScrollY();
        }
        
        static void release(void* context, unsigned int button, int x, int y) {
            Drag* that = (Drag*)context;
            LOG("Drag release: ", button, " ", x, ":", y);
            that->drag = false;
        }
        
        static void move(void* context, int x, int y) {
            Drag* that = (Drag*)context;
            if (that->drag) {
                that->setScrollXY(
                    that->dragScrollStartedX + (that->dragStartedX - x), 
                    that->dragScrollStartedY + (that->dragStartedY - y)
                );
                LOG("Drag scrollTo: ", that->scrollX, ":", that->scrollY);
                that->draw();
            }
        }

    public:
        Drag(GraphicsWindow* gwin, int left, int top, int width, int height,
            const Border border = defaultScrollBorder,
            const Color backgroundColor = defaultScrollBackgroundColor
        ): Area(gwin, left, top, width, height, "", CENTER, border, backgroundColor)
        {
            onTouchHandlers.push_back(touch);
            onReleaseHandlers.push_back(release);
            onMoveHandlers.push_back(move);
        }
    };

}
