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
    
    class Graphics {
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

        typedef void (*onResizeHandler)(int, int);
        typedef void (*onKeyPressHandler)(unsigned long);
        typedef void (*onKeyReleaseHandler)(unsigned long);
        typedef void (*onTouchHandler)(unsigned int, int, int);
        typedef void (*onReleaseHandler)(unsigned int, int, int);
        typedef void (*onDragHandler)(int, int);
        
        onResizeHandler onResize = NULL;
        onKeyPressHandler onKeyPress = NULL;
        onKeyReleaseHandler onKeyRelease = NULL;
        onTouchHandler onTouch = NULL;
        onReleaseHandler onRelease = NULL;
        onDragHandler onDrag = NULL;

        void openWindow(int width, int height, Color color) {
            // Initialize the X display
            if (display) throw runtime_error("Display already open.");
            display = XOpenDisplay(NULL);
            if (!display)
                throw runtime_error("Unable to open display.");

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
            XSetForeground(display, gc, color);
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void drawVerticalLine(int x1, int y1, int y2, Color color) {            
            XSetForeground(display, gc, color);
            XDrawLine(display, window, gc, x1, y1, x1, y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2, Color color) {            
            XSetForeground(display, gc, color);
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

        void eventLoop() {            
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
                    if (onResize) onResize(width, height);
                    break;

                case KeyPress:
                    // Handle key press event
                    XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                    LOG("Key pressed: ", text);
                    if (onKeyPress) onKeyPress(key);
                    break;

                case KeyRelease:
                    // Handle key release event
                    XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                    LOG("Key released: ", text);
                    if (onKeyRelease) onKeyRelease(key);
                    break;

                case ButtonPress:
                    // Handle mouse button press event
                    if (onTouch) onTouch(event.xbutton.button, event.xbutton.x, event.xbutton.y);
                    LOG("Mouse button pressed: ", event.xbutton.button, " at (", event.xbutton.x, ", ", event.xbutton.y, ")");
                    break;

                case ButtonRelease:
                    // Handle mouse button release event
                    LOG("Mouse button released: ", event.xbutton.button, " at (", event.xbutton.x, ", ", event.xbutton.y, ")");
                    if (onRelease) onRelease(event.xbutton.button, event.xbutton.x, event.xbutton.y);
                    break;

                case MotionNotify:
                    // Handle mouse motion event
                    LOG("Mouse moved to (", event.xmotion.x, ", ", event.xmotion.y, ")");
                    if (onDrag) onDrag(event.xbutton.x, event.xbutton.y);
                    break;

                default:
                    throw runtime_error("Unhandled event type: " + to_string(event.type));
                    break;
            }
        }
    };

    Display* Graphics::display = NULL;

}
