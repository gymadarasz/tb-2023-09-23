#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

int main()
{
    Graphics gfx;
    gfx.openWindow(800, 600, gray);

    // // Draw a rectangle
    // LOG("Draw a rectangle");
    // gfx.drawRectangle(50, 50, 200, 150, red);

    // // Fill a rectangle
    // LOG("Fill a rectangle");
    // gfx.fillRectangle(300, 50, 450, 150, green);

    // // Draw a line
    // LOG("Draw a line");
    // gfx.drawLine(100, 200, 300, 200, blue);

    // // Draw text
    // LOG("setFont");
    // gfx.setFont("fixed");
    // LOG("writeText");
    // gfx.writeText(100, 250, "Hello, World!", white);

    // Continuously process events until the user closes the window
    LOG("start loop...");
    while (true) {
        gfx.eventLoop();
    }

    // LOG("sleep");
    // Tools::sleep(2000);

    gfx.closeWindow();

    return 0;
}
