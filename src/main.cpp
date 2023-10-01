#include "includes/Tools.hpp"
#include "includes/gfx.hpp"

using namespace gfx;

GFX g;
int i=0;

void loop() {
    //LOG("Draw a rectangle");
    g.drawRectangle(50+i, 50+i, 200, 150, red);
    i++;
}

int main()
{
    g.onLoop = loop;
    g.openWindow(800, 600, gray);

    // // Draw a rectangle
    // LOG("Draw a rectangle");
    // g.drawRectangle(50, 50, 200, 150, red);

    // // Fill a rectangle
    // LOG("Fill a rectangle");
    // g.fillRectangle(300, 50, 450, 150, green);

    // // Draw a line
    // LOG("Draw a line");
    // g.drawLine(100, 200, 300, 200, blue);

    // // Draw text
    // LOG("setFont");
    // g.setFont("fixed");
    // LOG("writeText");
    // g.writeText(100, 250, "Hello, World!", white);

    // Continuously process events until the user closes the window
    LOG("start loop...");
    g.eventLoop();
    

    // LOG("sleep");
    // Tools::sleep(2000);

    g.closeWindow();

    return 0;
}
