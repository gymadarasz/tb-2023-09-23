#include "includes/Tools.hpp"
#include "includes/graph.hpp"

using namespace graph;

GFX gfx;
GUI gui(&gfx, 800, 600);
Button button2(&gfx, 110, 10, 80, 30, "Click!");

void doit(void*, unsigned int, int, int) {
    button2.setBackgroundColor(red);
    button2.setTextColor(white);
    button2.draw();
}

int main()
{
    Drag drag(&gfx, 100, 100, 200, 200);
    drag.fixed = true;
    Button button(&gfx, 10, 10, 80, 30, "Hello");
    button2.onTouchHandlers.push_back(doit);
    Button button3(&gfx, 210, 10, 80, 30, "Sticky");
    button3.sticky = true;
    Button button4(&gfx, 310, 10, 80, 30, "Hello4");
    Button button5(&gfx, 10, 50, 250, 50, "Big Willy");
    Button button6(&gfx, 210, 210, 80, 30, "Hello6");
    Label label1(&gfx, 100, 130, 100, 20, "Label1");
    drag.child(&button);
    drag.child(&button2);
    drag.child(&button3);
    drag.child(&button4);
    drag.child(&button5);
    drag.child(&button6);
    drag.child(&label1);
    gui.child(&drag);
    gui.loop();
 
    return 0;
}
