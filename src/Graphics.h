#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <cstdio> // Include for fprintf and stderr

class Graphics
{
private:
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    XGCValues values;
    XFontStruct *fontInfo;
    const char *font_name = "9x15"; // Font name and size
    int screen;
public:
    void createWindow();

    // Cleanup and exit
    void closeWindow();

    // Main event loop (returns true on exit)
    bool eventLoop();
};
