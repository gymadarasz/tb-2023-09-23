#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

int main()
{
    GUI gui(800, 600);
    GraphicsWindow* gwin = gui.getGraphicsWindow();
    Area area(gwin, 100, 100, 200, 200);
    area.setBorder(SIMPLE);
    area.setBorderColor(red);
    Button button(gwin, 10, 10, 80, 30, "Hello");
    Button button2(gwin, 210, 10, 80, 30, "Hello2");
    area.child(button);
    area.child(button2);
    gui.child(area);
    gui.loop();
 
    return 0;
}
