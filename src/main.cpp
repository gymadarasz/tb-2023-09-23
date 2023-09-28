#include "includes/Graphics.hpp"

using namespace gfx;

int main()
{
    Graphics gfx;
    gfx.init(800, 600, gray);
    while(true) gfx.loop();
    gfx.close();

    return 0;
}
