#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <cstdio> // Include for fprintf and stderr

int main() {
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    XGCValues values;
    XFontStruct *fontInfo;
    char *font_name = (char *)"9x15"; // Font name and size

    // Initialize the X display
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open display.\n");
        return 1;
    }

    // Create a black window
    int screen = DefaultScreen(display);
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
    if (!fontInfo) {
        fprintf(stderr, "Unable to load font: %s\n", font_name);
        return 1;
    }
    XSetFont(display, gc, fontInfo->fid);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            // Redraw the window
            XSetForeground(display, gc, BlackPixel(display, screen));
            XFillRectangle(display, window, gc, 0, 0, 800, 600);
            
            // Draw a red button
            XSetForeground(display, gc, 0xFF0000); // Red color (format: 0xRRGGBB)
            XFillRectangle(display, window, gc, 100, 100, 200, 100);

            // Draw text on the button
            XSetForeground(display, gc, WhitePixel(display, screen));
            XDrawString(display, window, gc, 170, 150, "Exit", 4);
        } else if (event.type == ButtonPress) {
            // Check for button click
            if (event.xbutton.x >= 100 && event.xbutton.x <= 300 &&
                event.xbutton.y >= 100 && event.xbutton.y <= 200) {
                break; // Exit the program on button click
            }
        }
    }

    // Cleanup and exit
    XFreeFont(display, fontInfo);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
