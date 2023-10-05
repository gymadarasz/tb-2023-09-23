#include "includes/madlib/graph/graph.hpp"

using namespace std;
using namespace madlib::graph;

void draw(void* context) {
    Area* area = (Area*)context;
    area->color(white);
    area->line(20, 20, 200, 100);
}

int main()
{
    GFX gfx;
    GUI gui(&gfx, 800, 600);
    Frame frame(&gfx, 10, 10, 700, 500);
    gui.child(&frame);

    frame.onDrawHandlers.push_back(draw);

    gui.loop();
    
    return 0;
}
