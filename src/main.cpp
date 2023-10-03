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
    area.child(button);
    gui.child(area);
    gui.loop();
 
    return 0;
}
