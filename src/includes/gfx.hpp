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
    const Color gray = 0x888888;
    const Color darkGray = 0x444444;
    const Color red = 0x880000;
    const Color green = 0x008800;
    const Color blue = 0x000088;
    const Color lightRed = 0xFF0000;
    const Color lightGreen = 0x00FF00;
    const Color lightBlue = 0x0000FF;
    const Color cyan = 0x008888;
    const Color purple = 0x880088;
    const Color orange = 0x888800;
    const Color lightCyan = 0x00FFFF;
    const Color lightPurple = 0xFF00FF;
    const Color yellow = 0xFFFF00;

    class EventHandler {
    public:

        void* eventContext = NULL;

        typedef void (*onResizeHandler)(void*, int, int);
        typedef void (*onKeyPressHandler)(void*, unsigned long);
        typedef void (*onKeyReleaseHandler)(void*, unsigned long);
        typedef void (*onTouchHandler)(void*, unsigned int, int, int);
        typedef void (*onReleaseHandler)(void*, unsigned int, int, int);
        typedef void (*onDragHandler)(void*, int, int);
        typedef void (*onOpenHandler)(void*);
        typedef void (*onLoopHandler)(void*);
        
        onResizeHandler onResize = NULL;
        onKeyPressHandler onKeyPress = NULL;
        onKeyReleaseHandler onKeyRelease = NULL;
        onTouchHandler onTouch = NULL;
        onReleaseHandler onRelease = NULL;
        onDragHandler onDrag = NULL;
        onOpenHandler onSetup = NULL;
        onLoopHandler onLoop = NULL;

    };
    
    class GraphicsWindow: public EventHandler {
    public:

        static const unsigned long defaultLoopMs = 100;
        static const Color defaultWindowColor = gray;
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
            if (onSetup && XPending(display) <= 0) {
                Tools::sleep(ms);
                onSetup(eventContext);
            }

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
                        LOG("Key pressed: ", text);
                        if (onKeyPress) onKeyPress(eventContext, key);
                        break;

                    case KeyRelease:
                        // Handle key release event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        LOG("Key released: ", text);
                        if (onKeyRelease) onKeyRelease(eventContext, key);
                        break;

                    case ButtonPress:
                        // Handle mouse button press event
                        if (onTouch) onTouch(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        LOG("Mouse button pressed: ", event.xbutton.button, " at (", event.xbutton.x, ", ", event.xbutton.y, ")");
                        break;

                    case ButtonRelease:
                        // Handle mouse button release event
                        LOG("Mouse button released: ", event.xbutton.button, " at (", event.xbutton.x, ", ", event.xbutton.y, ")");
                        if (onRelease) onRelease(eventContext, event.xbutton.button, event.xbutton.x, event.xbutton.y);
                        break;

                    case MotionNotify:
                        // Handle mouse motion event
                        LOG("Mouse moved to (", event.xmotion.x, ", ", event.xmotion.y, ")");
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
    const char* GraphicsWindow::defaultWindowFont = "10x20";


    enum Align {
        CENTER,
    };

    class Area: public EventHandler {
    public:

        static const Align defaultAreaTextAlign = CENTER;
        static const Align defaultAreaBorderColor = CENTER;
        static const Color defaultAreaTextColor = black;

    protected:
        const int left, top, width, height;
        const string text;
        const Align textAlign;
        const Color backgroundColor = GraphicsWindow::defaultWindowColor;
        const Color borderColor = defaultAreaBorderColor;
        const Color textColor = defaultAreaTextColor;
    public:

        Area(int left, int top, int width, int height, const string text = "", const Align textAlign = defaultAreaTextAlign):
            left(left), top(top), width(width), height(height), text(text), textAlign(textAlign) {}

        int getLeft() {
            return left;
        }

        int getTop() {
            return top;
        }

        int getWidth() {
            return width;
        }

        int getHeight() {
            return height;
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

        void draw(GraphicsWindow* gwin) {
            int top = getTop();
            int left = getLeft();
            int width = getWidth();
            int height = getHeight();
            int right = left + width;
            int bottom = top + height;
            gwin->fillRectangle(left, top, right, bottom, getBackgroundColor());
            // TODO: draw borders: PUSHED, RELEASED, NONE
            gwin->drawRectangle(left, top, right, bottom, getBorderColor());
            int textWidth, textHeight;
            const string text = getText();
            gwin->getTextSize(text, textWidth, textHeight);
            int textLeft, textTop;
            Align textAlign = getTextAlign();
            switch (textAlign)
            {
                case CENTER:
                    textLeft = left + ((width - textWidth) / 2);
                    textTop = top + ((height - textHeight) / 2) + 14; // ??14
                    break;
                
                default:
                    throw runtime_error("Invalid text align");
                    break;
            }
            gwin->writeText(textLeft, textTop, text, getTextColor());
        }
    };

    class GUI: public GraphicsWindow {
    protected:
        vector<Area> areas;

        static void drawUI(void* context) {
            LOG("Draw UI");
            GUI* that = (GUI*)context;
            for (Area& area: that->areas) {
                area.draw(that);
            }
        }

    public:
        GUI(int width, int height, Color color = defaultWindowColor) {
            openWindow(width, height, color);
            eventContext = this;
            onSetup = drawUI;
        }

        ~GUI() {
            closeWindow();
        }

        void button(int left, int top, int width, int height, const string text, const Align align = Area::defaultAreaTextAlign) {
            Area button(left, top, width, height, text, align);
            areas.push_back(button);
        }
    };

}
