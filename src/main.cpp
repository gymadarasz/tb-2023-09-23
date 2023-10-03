#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

int main()
{
    GUI gui(800, 600);
    gui.button(200, 100, 80, 30, "Hello");
    gui.eventLoop();
 
    return 0;
}
