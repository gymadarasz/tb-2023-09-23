#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

int main()
{
    GUI gui(800, 600);
    GraphicsWindow* gwin = gui.getGraphicsWindow();
    Drag drag(gwin, 100, 100, 200, 200);
    Button button(gwin, 10, 10, 80, 30, "Hello");
    Button button2(gwin, 110, 10, 80, 30, "Hello2");
    Button button3(gwin, 210, 10, 80, 30, "Hello3");
    Button button4(gwin, 310, 10, 80, 30, "Hello4");
    drag.child(&button);
    drag.child(&button2);
    drag.child(&button3);
    drag.child(&button4);
    gui.child(&drag);
    gui.loop();
 
    return 0;
}
