#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <cstdio> // Include for fprintf and stderr
#include <stdexcept>
#include <vector>
#include <iostream>


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

    class Area {
    protected:
        Area* parent = NULL;
        int top = 0, left = 0, width = 0, height = 0, scrollx = 0, scrolly = 0;
        vector<Area> children;
    public:
        void reset(int t, int l, int w, int h) {
            top = t;
            left = l;
            width = w;
            height = h;
            redraw();
        }

        void scroll(int x, int y) {
            scrollx = x;
            scrolly = y;
            redraw();
        }

        void child(Area &area) {
            area.parent = this;
            children.push_back(area);
        }

        Area* root() {
            if (parent) return parent->root();
            return this;
        }

        void redraw() {
            // TODO...
        }
    };


    class Graphics: public Area {
    protected:

        Display *display = NULL;
        Window window;
        GC gc;
        XFontStruct *fontInfo = NULL;

        // TODO: tools lib
        void replace(int &a, int &b) {
            int t = a;
            a = b;
            b = t;
        }

    public:

        void init(int width, int height, Color c) {
            // Initialize the X display
            display = XOpenDisplay(NULL);
            if (!display)
                throw runtime_error("Unable to open display.");

            // int screen = DefaultScreen(display);

            // Create the window
            window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, (unsigned)width, (unsigned)height, 1, c, c);
            XMapWindow(display, window);

            // Create a graphics context
            gc = XCreateGC(display, window, 0, NULL);

            // Select inputs
            XSelectInput(display, window, 
                ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
                ButtonReleaseMask | PointerMotionMask);

            reset(0, 0, width, height);
        }

        void close() {
            if (fontInfo) XFreeFont(display, fontInfo);
            XFreeGC(display, gc);
            XDestroyWindow(display, window);
            XCloseDisplay(display);
        }

        void clear(Color c) {
            XSetForeground(display, gc, c);
            XFillRectangle(display, window, gc, 0, 0, (unsigned)width(), (unsigned)height());
        }

        int width() {
            XWindowAttributes attr;
            XGetWindowAttributes(display, window, &attr);
            return attr.width;
        }

        int height() {
            XWindowAttributes attr;
            XGetWindowAttributes(display, window, &attr);
            return attr.height;
        }

        void rect(int x1, int y1, int x2, int y2, Color c) {
            XSetForeground(display, gc, c);
            if (x1 > x2) replace(x1, x2);
            if (y1 > y2) replace(y1, y2);
            XDrawRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        void fillRect(int x1, int y1, int x2, int y2, Color c) {
            XSetForeground(display, gc, c);
            if (x1 > x2) replace(x1, x2);
            if (y1 > y2) replace(y1, y2);
            XFillRectangle(display, window, gc, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
        }

        void line(int x1, int y1, int x2, int y2, Color c) {
            XSetForeground(display, gc, c);
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void vline(int x1, int y1, int y2, Color c) {
            XSetForeground(display, gc, c);
            XDrawLine(display, window, gc, x1, y1, x1, y2);
        }

        void hline(int x1, int y1, int x2, Color c) {
            XSetForeground(display, gc, c);
            XDrawLine(display, window, gc, x1, y1, x2, y1);
        }

        void text(int x1, int y1, const string txt, Color c) {
            XSetForeground(display, gc, c);
            XDrawString(display, window, gc, x1, y1, txt.c_str(), (int)txt.length());
        }

        void font(const char* fnt) {
            fontInfo = XLoadQueryFont(display, fnt);
            if (!fontInfo)
            {
                fprintf(stderr, "Unable to load font: %s\n", fnt);
                throw runtime_error("Unable to load font.\n");
            }
            XSetFont(display, gc, fontInfo->fid);
        }

        typedef void (*onResizeHandler)(int, int);
        onResizeHandler onResize;

        void loop() {
            while (true) {
                XEvent event;
                XNextEvent(display, &event);

                int w, h;
                KeySym key;
                char text[32];
                switch (event.type) {
                    case Expose:
                        // Handle expose event (e.g., redraw)
                        w = width();
                        h = height();
                        reset(0, 0, w, h);
                        if (onResize) onResize(w, h);
                        break;

                    case KeyPress:
                        // Handle key press event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        cout << "Key pressed: " << text << endl << flush;
                        break;

                    case KeyRelease:
                        // Handle key release event
                        XLookupString(&event.xkey, text, sizeof(text), &key, NULL);
                        cout << "Key released: " << text << endl << flush;
                        break;

                    case ButtonPress:
                        // Handle mouse button press event
                        cout << "Mouse button pressed: " << event.xbutton.button << " at (" << event.xbutton.x << ", " << event.xbutton.y << ")" << endl << flush;
                        break;

                    case ButtonRelease:
                        // Handle mouse button release event
                        cout << "Mouse button released: " << event.xbutton.button << " at (" << event.xbutton.x << ", " << event.xbutton.y << ")" << endl << flush;
                        break;

                    case MotionNotify:
                        // Handle mouse motion event
                        cout << "Mouse moved to (" << event.xmotion.x << ", " << event.xmotion.y << ")" << endl << flush;
                        break;

                    default:
                        throw runtime_error("Unhandled event type: " + to_string(event.type));
                        break;
                }
            }
        }
    };

}