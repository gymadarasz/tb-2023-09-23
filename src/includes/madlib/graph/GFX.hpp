#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "../ERROR.hpp"
#include "../time.hpp"

#include "defs.hpp"
#include "Color.hpp"
#include "Theme.hpp"
#include "EventHandler.hpp"

namespace madlib::graph {

    class GFX: public EventHandler {
    protected:

        static Display *display;
        Window window;
        GC gc;
        const char* font = nullptr;
        XFontStruct *fontInfo = nullptr;

        Viewport viewport;

        void* context = nullptr;

    public:

        bool close = false;

        GFX(void* context = nullptr): context(context) {}

        void* getContext() const {
            return context;
        }

        void setColor(Color color) const {
            XSetForeground(display, gc, color);
        }

        void setViewport(Viewport viewport) {
            this->viewport = viewport;
        }

        void openWindow(
            int width, int height, 
            const char* title = Theme::defaultWindowTitle, 
            Color color = Theme::defaultWindowColor, 
            const char* font = Theme::defaultWindowFont
        ) {
            // Initialize the X display
            if (!display) display = XOpenDisplay(nullptr);
            if (!display) throw ERROR("Unable to open display.");

            // int screen = DefaultScreen(display);

            // Create the window
            window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, (unsigned)width, (unsigned)height, 1, color, color);
            XMapWindow(display, window);

            // Set the window title
            XStoreName(display, window, title);

            // Create a graphics context
            gc = XCreateGC(display, window, 0, nullptr);

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
            Viewport rect(x1, y1, x2, y2);
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
            Viewport rect(x1, y1, x2, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XFillRectangle(display, window, gc, rect.x1, rect.y1, (unsigned)(rect.x2 - rect.x1), (unsigned)(rect.y2 - rect.y1));
        }

        void drawLine(int x1, int y1, int x2, int y2) const {
            Viewport rect(x1, y1, x2, y2);
            
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
            //     rect = Viewport(x1, y1, x2, y2);
            // }

            if (!viewport.containsCompletely(rect)) {
                // TODO: cut the line at the edge of viewport
                return;
            }

            // Draw the clipped line
            XDrawLine(display, window, gc, x1, y1, x2, y2);
        }

        void drawVerticalLine(int x1, int y1, int y2) const {
            Viewport rect(x1, y1, x1, y2);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x1, rect.y2);
        }
        
        void drawHorizontalLine(int x1, int y1, int x2) const {
            Viewport rect(x1, y1, x2, y1);
            rect.intersect(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
            XDrawLine(display, window, gc, rect.x1, rect.y1, rect.x2, rect.y1);
        }

        void setFont(const char* font) { 
            if (!font) throw ERROR("No font name set");
            this->font = font;           
            fontInfo = XLoadQueryFont(display, font);
            if (!fontInfo)
            {
                // Note: uncomment the following line to see awailable fonts:
                // LOG("Available fonts:\n\t" + vector_concat(getAvailableFonts(), "\n\t"));
                throw ERROR("Unable to load font:" + string(font));
            }
            XSetFont(display, gc, fontInfo->fid);
        }
        
        vector<string> getAvailableFonts() {
            vector<string> fonts;

            int nFonts;
            char** fontNames = XListFonts(display, "*", 1000, &nFonts);

            if (fontNames == nullptr) {
                fprintf(stderr, "Unable to list fonts.\n");
                return fonts;
            }

            for (int i = 0; i < nFonts; i++) {
                fonts.push_back(fontNames[i]);
            }

            XFreeFontNames(fontNames);
            XCloseDisplay(display);

            return fonts;
        }
        
        void writeText(int x, int y, const string& text) {
            // Cut text to fit into the viewport first
            string txt = text;
            if (!fontInfo) setFont(font);
            if (!fontInfo) throw ERROR("No font info");
            int asc = fontInfo->ascent;
            int desc = fontInfo->descent;
            y += asc;
            while (!txt.empty()) {
                int width, height;
                getTextSize(txt, width, height);
                int x1 = x;
                int y1 = y + desc;
                int x2 = x + width;
                int y2 = y - height + desc;
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

        enum FakeEventType { RELEASE }; // TODO: add more event types...

        typedef struct {
	        unsigned int button = AnyButton;
	        int x = 0, y = 0;
        } FakeEventData;

        typedef struct {
            FakeEventType type = RELEASE;
            FakeEventData data = {};
            XEvent toXEvent() {
                XEvent event;
                switch (type){
                    case RELEASE:
                        event.type = ButtonRelease;
                        event.xbutton.button = data.button;
                        event.xbutton.x = data.x;
                        event.xbutton.y = data.y;
                        break;                    
                    default:
                        throw ERROR("Invalid fake event");
                }
                return event;
            }
        } FakeEvent;

        vector<FakeEvent> fakeEvents;

        void triggerFakeEvent(FakeEvent fakeEvent) {
            this->fakeEvents.push_back(fakeEvent);
        }

        void eventLoop(unsigned long ms = Theme::defaultGFXEventLoopMs) {

            while (!close) {

                if (!onLoopHandlers.empty() && XPending(display) <= 0) {
                    sleep_ms(ms);
                    for (const onLoopHandler& onLoop: onLoopHandlers) 
                        onLoop(eventContext);
                    continue;
                }

                XEvent event;               

                if (!fakeEvents.empty()) {
                    FakeEvent fakeEvent = fakeEvents.front();
                    fakeEvents.erase(fakeEvents.begin());
                    event  = fakeEvent.toXEvent();
                    handleEvent(event);
                    continue;
                }

                XNextEvent(display, &event);

                // Flush the event queue to discard any pending events
                while (XPending(display) > 0) XNextEvent(display, &event);

                handleEvent(event);
            }
        }

        void setCursor(unsigned int cursor = XC_arrow) const {
            Cursor blockCursor = XCreateFontCursor(display, cursor);
            XDefineCursor(display, window, blockCursor);
            XFlush(display);
        }
    
        void handleEvent(XEvent event) const {
            setCursor(XC_X_cursor);
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
                    XLookupString(&event.xkey, text, sizeof(text), &key, nullptr);
                    for (const onKeyPressHandler& onKeyPress: onKeyPressHandlers)
                        onKeyPress(eventContext, key);
                    break;

                case KeyRelease:
                    // Handle key release event
                    XLookupString(&event.xkey, text, sizeof(text), &key, nullptr);
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
            setCursor();
        }
    };

    Display* GFX::display = nullptr;

}