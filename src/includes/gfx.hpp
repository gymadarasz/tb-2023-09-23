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
        BUTTON,
        PUSHED,
    };

    struct Theme {
        static const unsigned long eventLoopMs = 100;
        static const Color windowColor = gray;
        static const char* windowFont;
        static const Align textAlign = CENTER;
        static const Color borderColor = gray;
        static const Color textColor = black;
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
        typedef void (*onDragHandler)(void*, int, int);
        typedef void (*onLoopHandler)(void*);
        
        onResizeHandler onResize = NULL;
        onKeyPressHandler onKeyPress = NULL;
        onKeyReleaseHandler onKeyRelease = NULL;
        onTouchHandler onTouch = NULL;
        onReleaseHandler onRelease = NULL;
        onDragHandler onDrag = NULL;
        onLoopHandler onLoop = NULL;

    };
    
    class GraphicsWindow: public EventHandler {
    public:

        static const unsigned long defaultLoopMs = Theme::eventLoopMs;
        static const Color defaultWindowColor = Theme::windowColor;
        static const char* defaultWindowFont;

    private:

        void setupDrawing(int &x1, int &y1, int &x2, int &y2, Color color) {
            XSetForeground(display, gc, color);
            if (x1 > x2) Tools::replace(x1, x2);
            if (y1 > y2) Tools::replace(y1, y2);
        }

    protected:

        static Display *display;
        Window window;
        GC gc;
        XFontStruct *fontInfo = NULL;

    public:

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

        void closeWindow() {            
            if (fontInfo) XFreeFont(display, fontInfo);
            XFreeGC(display, gc);
            XDestroyWindow(display, window);
            XCloseDisplay(display);
        }

        void getWindowSize(int &width, int &height) {            
            XWindowAttributes attr;
            XGetWindowAttributes(display, window, &attr);
            width = attr.width;
            height = attr.height;
        }

        void clearWindow(Color color) {            
            XSetForeground(display, gc, color);
            int width, height;
            getWindowSize(width, height);
            XFillRectangle(display, window, gc, 0, 0, (unsigned)width, (unsigned)height);
        }

        void drawRectangle(int x1, int y1, int x2, int y2, Color color) {            
            setupDrawing(x1, y1, x2, y2, color);
            XDrawRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        void fillRectangle(int x1, int y1, int x2, int y2, Color color) {            
            setupDrawing(x1, y1, x2, y2, color);
            XFillRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        void drawLine(int x1, int y1, int x2, int y2, Color color) {            
            setupDrawing(x1, y1, x2, y2, color);
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void drawVerticalLine(int x1, int y1, int y2, Color color) {            
            setupDrawing(x1, y1, x1, y2, color);
            XDrawLine(display, window, gc, x1, y1, x1, y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2, Color color) {            
            setupDrawing(x1, y1, x1, y1, color);
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
            XSetForeground(display, gc, color);
            XDrawString(display, window, gc, x, y, text.c_str(), (int)text.length());
        }

        void getTextSize(const string text, int &width, int &height) {            
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

        void eventLoop(unsigned long ms = defaultLoopMs) {
            LOG("Window event loop starts");
            

            while (true) {

                if (onLoop && XPending(display) <= 0) {
                    Tools::sleep(ms);
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
                        if (onResize) onResize(eventContext, width, height);
                        break;

                    case KeyPress:
                        // Handle key press event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        if (onKeyPress) onKeyPress(eventContext, key);
                        break;

                    case KeyRelease:
                        // Handle key release event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        if (onKeyRelease) onKeyRelease(eventContext, key);
                        break;

                    case ButtonPress:
                        // Handle mouse button press event
                        if (onTouch) onTouch(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case ButtonRelease:
                        // Handle mouse button release event
                        if (onRelease) onRelease(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case MotionNotify:
                        // Handle mouse motion event
                        if (onDrag) onDrag(eventContext, event.xbutton.x, event.xbutton.y);
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

    protected:
        GraphicsWindow* gwin = NULL;

        const int left, top;
        int width, height;
        const string text;
        const Align textAlign;
        const Color backgroundColor = GraphicsWindow::defaultWindowColor;
        const Color borderColor = defaultAreaBorderColor;
        const Color textColor = defaultAreaTextColor;
        Border border = defaultAreaBorder;

        vector<Area> areas;

    public:

        Area(GraphicsWindow* gwin, int left, int top, int width, int height, 
            const string text = "", const Align textAlign = defaultAreaTextAlign,
            const Border border = defaultAreaBorder
        ):
            gwin(gwin),
            left(left), top(top), width(width), height(height), 
            text(text), textAlign(textAlign), border(border) {}

        GraphicsWindow* getGraphicsWindow() {
            return gwin;
        }

        void child(const Area& area) {
            areas.push_back(area);
        }

        int getLeft() const {
            return left;
        }

        int getTop() const {
            return top;
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

        Border getBorder() {
            return border;
        }

        void setBorder(Border border) {
            this->border = border;
        }

        string getText() {
            return text;
        }

        Align getTextAlign() {
            return textAlign;
        }

        Color getBackgroundColor() {
            return backgroundColor;
        }

        Color getBorderColor() {
            return borderColor;
        }

        Color getTextColor() {
            return textColor;
        }

        bool contains(int x, int y) const {
            int left = getLeft();
            int top = getTop();
            return
                x >= left && x <= left + getWidth() &&
                y >= top && y <= top + getHeight();
        }

        void draw() {
            int top = getTop();
            int left = getLeft();
            int width = getWidth();
            int height = getHeight();
            const string text = getText();
            int right = left + width;
            int bottom = top + height;
            gwin->fillRectangle(left, top, right, bottom, getBackgroundColor());
            // TODO: draw borders: PUSHED, RELEASED, NONE
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
                case BUTTON:
                    borderColor = getBorderColor();
                    borderColorLight = ColorMixer::light(borderColor);
                    borderColorDark = ColorMixer::dark(borderColor);
                    gwin->drawHorizontalLine(left, top, right, borderColorLight);
                    gwin->drawHorizontalLine(left, bottom, right, borderColorDark);
                    gwin->drawVerticalLine(left, top, bottom, borderColorLight);
                    gwin->drawVerticalLine(right, top, bottom, borderColorDark);
                    break;
                case PUSHED:
                    borderColor = getBorderColor();
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
            
            int textWidth, textHeight;
            gwin->getTextSize(text, textWidth, textHeight);
            int textLeft, textTop;
            Align textAlign = getTextAlign();
            switch (textAlign)
            {
                case CENTER:
                    textLeft = left + ((width - textWidth) / 2);
                    textTop = top + ((height - textHeight) / 2) + 16; // ??16
                    break;
                
                default:
                    throw runtime_error("Invalid text align");
                    break;
            }
            gwin->writeText(textLeft, textTop, text, getTextColor());

            for (Area& area: areas) {
                area.draw();
            }

        }
    };

    class Button: public Area {
    protected:
        
        static void touch(void* context, unsigned int button, int x, int y) {
            Button* that = (Button*)context;
            LOG("Button touch: ", button, " ", x, ":", y);
            that->setBorder(PUSHED);
            that->draw();
        }
        
        static void release(void* context, unsigned int button, int x, int y) {
            Button* that = (Button*)context;
            LOG("Button release: ", button, " ", x, ":", y);
            that->setBorder(BUTTON);
            that->draw();
        }

    public:
        Button(GraphicsWindow* gwin, int left, int top, int width, int height, 
            const string text, const Align textAlign = Area::defaultAreaTextAlign
        ):
            Area(gwin, left, top, width, height, text, textAlign, BUTTON)
        {
            onTouch = touch;
            onRelease = release;
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
            LOG("Touch UI: ", button, " ", x, ":", y);
            GUI* that = (GUI*)context;
            for (Area& area: that->areas) {
                if (area.onTouch && area.contains(x, y)) {
                    area.onTouch(&area, button, x - area.getLeft(), y - area.getTop());
                }
            }
        }

        static void release(void* context, unsigned int button, int x, int y) {
            LOG("Release UI: ", button, " ", x, ":", y);
            GUI* that = (GUI*)context;
            for (Area& area: that->areas) {
                if (area.onRelease && area.contains(x, y)) {
                    area.onRelease(&area, button, x - area.getLeft(), y - area.getTop());
                }
            }
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
            gwin->onResize = resize;
            gwin->onTouch = touch;
            gwin->onRelease = release;
        }

        ~GUI() {
            gwin->closeWindow();
            delete gwin;
        }

        void loop(unsigned long ms = GraphicsWindow::defaultLoopMs) {
            gwin->eventLoop(ms);
        }
    };

}
