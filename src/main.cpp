#include "includes/Tools.hpp"
#include "includes/graph.hpp"

using namespace graph;

GFX gfx;
GUI gui(&gfx, 800, 600);
Button button(&gfx, 10, 10, 80, 30, "Hello");
Button button2(&gfx, 110, 10, 80, 30, "Click!");
Frame drag2(&gfx, 320, 100, 200, 200);

void doit(void*, unsigned int, int, int) {
    button2.setBackgroundColor(red);
    button2.setTextColor(white);
    button2.draw();
}

void draw(void*) {
    drag2.rect(10, 10, 50, 50, green);
    drag2.line(10, 10, 50, 50, red);
    drag2.line(50, 10, 10, 50, blue);
    drag2.rect(100, 100, 150, 150, green);
    drag2.line(100, 100, 150, 150, red);
    drag2.fillRect(220, 220, 300, 300, green);
    drag2.line(220, 220, 300, 300, red);
    drag2.line(300, 220, 220, 300, blue);
    drag2.rect(25, 25, 325, 325, blue);
    drag2.line(325, 25, 25, 325, black);
    drag2.hLine(30, 30, 130, orange);
    drag2.vLine(30, 30, 130, yellow);
    // TODO: bach painting...
}

void close(void*, unsigned int, int, int) {
    gfx.close = true;
}

int main()
{
    Frame frame(&gfx, 100, 100, 200, 200);
    // frame.fixed = true;
    button2.onTouchHandlers.push_back(doit);
    Button button3(&gfx, 210, 10, 80, 30, "Sticky");
    button3.sticky = true;
    Button button4(&gfx, 310, 10, 80, 30, "Hello4");
    Button button5(&gfx, 10, 50, 250, 50, "Big Willy");
    Button button6(&gfx, 210, 210, 80, 30, "Close");
    button6.onTouchHandlers.push_back(close);
    Label label1(&gfx, 100, 130, 100, 20, "Label1");
    frame.child(&button);
    frame.child(&button2);
    frame.child(&button3);
    frame.child(&button4);
    frame.child(&button5);
    frame.child(&button6);
    frame.child(&label1);
    gui.child(&frame);

    drag2.setScrollXYMax(350, 350);
    drag2.onDrawHandlers.push_back(draw);
    gui.child(&drag2);

    gui.loop();
 
    return 0;
}
