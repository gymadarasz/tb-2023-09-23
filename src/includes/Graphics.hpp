#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <cstdio> // Include for fprintf and stderr
#include <stdexcept>
#include <vector>

using namespace std;

class Area {
public:
    int top = 0, left = 0, width = 100, height = 100, vScroll = 0, hScroll = 0;
    // TODO: making UI
};

class Graphics
{
private:
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    XGCValues values;
    XFontStruct *fontInfo;
    const char *font_name = "10x20"; //"9x15"; // Font name and size (see more with xlsfonts command)
    int screen;
public:
    vector<Area> areas;

    void createWindow() {
        // Initialize the X display
        display = XOpenDisplay(NULL);
        if (!display)
        {
            throw runtime_error("Unable to open display.\n");
        }

        // Create a black window
        screen = DefaultScreen(display);
        unsigned long black = BlackPixel(display, screen);
        window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, 800, 600, 0, black, black);

        // Select event types to listen for
        XSelectInput(display, window, ExposureMask | ButtonPressMask);

        // Map the window and create a graphics context (GC)
        XMapWindow(display, window);
        gc = XCreateGC(display, window, 0, &values);
        XSetForeground(display, gc, BlackPixel(display, screen));

        // Load the font
        fontInfo = XLoadQueryFont(display, font_name);
        if (!fontInfo)
        {
            fprintf(stderr, "Unable to load font: %s\n", font_name);
            throw runtime_error("Unable to load font.\n");
        }
        XSetFont(display, gc, fontInfo->fid);
    }

    void closeWindow() {
        // Cleanup and exit
        XFreeFont(display, fontInfo);
        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }

    bool eventLoop() {
        // Main event loop (returns true on exit)

        XNextEvent(display, &event);

        if (event.type == Expose)
        {
            // Redraw the window
            XSetForeground(display, gc, BlackPixel(display, screen));
            XFillRectangle(display, window, gc, 0, 0, 800, 600);

            // Draw a red button
            XSetForeground(display, gc, 0xFF0000); // Red color (format: 0xRRGGBB)
            XFillRectangle(display, window, gc, 100, 100, 200, 100);

            // Draw text on the button
            XSetForeground(display, gc, WhitePixel(display, screen));
            XDrawString(display, window, gc, 170, 150, "Exit", 4);
        }
        else if (event.type == ButtonPress)
        {
            // Check for button click
            if (event.xbutton.x >= 100 && event.xbutton.x <= 300 &&
                event.xbutton.y >= 100 && event.xbutton.y <= 200)
            {
                return true; // Exit the program on button click
            }
        }

        return false;
    }
};
