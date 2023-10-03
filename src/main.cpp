#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

int main()
{
    GUI gui(800, 600);
    gui.child(Button(gui.getGraphicsWindow(), 200, 100, 80, 30, "Hello"));
    gui.loop();
 
    return 0;
}
