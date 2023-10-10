#include "../../../../src/includes/madlib/Tools.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;

GFX graph_manual_test1_gfx;
GUI graph_manual_test1_gui(graph_manual_test1_gfx, 800, 600);
Button graph_manual_test1_closeOkBtn(graph_manual_test1_gfx, 10, 10, 100, 30, "Ok");
Button graph_manual_test1_button(graph_manual_test1_gfx, 10, 10, 80, 30, "Hello");
Button graph_manual_test1_button2(graph_manual_test1_gfx, 110, 10, 80, 30, "Click!");
Frame graph_manual_test1_frame2(graph_manual_test1_gfx, 320, 100, 200, 200);
Button graph_manual_test1_lbtn(graph_manual_test1_gfx, 50, 50, 100, 30, "Hello");

void graph_manual_test1_doit(void*, unsigned int, int, int) {
    graph_manual_test1_button2.setBackgroundColor(red);
    graph_manual_test1_button2.setTextColor(white);
    graph_manual_test1_button2.draw();
}

void graph_manual_test1_draw(void*) {
    graph_manual_test1_frame2.color(green);
    graph_manual_test1_frame2.rect(10, 10, 50, 50);
    graph_manual_test1_frame2.rect(120, 100, 150, 150);
    graph_manual_test1_frame2.fillRect(220, 220, 300, 300);
    graph_manual_test1_frame2.color(red);
    graph_manual_test1_frame2.line(10, 10, 50, 50);
    graph_manual_test1_frame2.line(120, 100, 150, 150);
    graph_manual_test1_frame2.line(220, 220, 300, 300);
    graph_manual_test1_frame2.color(blue);
    graph_manual_test1_frame2.line(50, 10, 10, 50);
    graph_manual_test1_frame2.line(300, 220, 220, 300);
    graph_manual_test1_frame2.rect(25, 25, 325, 325);
    graph_manual_test1_frame2.color(black);
    graph_manual_test1_frame2.line(325, 25, 25, 325);
    graph_manual_test1_frame2.color(orange);
    graph_manual_test1_frame2.hLine(30, 30, 130);
    graph_manual_test1_frame2.color(yellow);
    graph_manual_test1_frame2.vLine(30, 30, 130);
    // TODO: bach painting...
}

void graph_manual_test1_close(void*, unsigned int, int, int) {
    graph_manual_test1_gfx.close = true;
}

void graph_manual_test1_lbtn_draw(void* context) {
    Area* area = (Area*)context;
    area->color(red);
    area->fillRect(5, 5, 25, 25);
}

int graph_manual_test1()
{
    graph_manual_test1_closeOkBtn.setBackgroundColor(green);
    graph_manual_test1_closeOkBtn.setTextColor(white);
    graph_manual_test1_closeOkBtn.onTouchHandlers.push_back(graph_manual_test1_close);
    graph_manual_test1_gui.child(graph_manual_test1_closeOkBtn);

    Frame frame(graph_manual_test1_gfx, 100, 100, 200, 200);
    // frame.fixed = true;    
    graph_manual_test1_button2.onTouchHandlers.push_back(graph_manual_test1_doit);
    Button button3(graph_manual_test1_gfx, 210, 10, 80, 30, "Sticky");
    button3.sticky = true;
    Button button4(graph_manual_test1_gfx, 310, 10, 80, 30, "Hello4");
    Button button5(graph_manual_test1_gfx, 10, 50, 250, 50, "Big Willy");
    Button button6(graph_manual_test1_gfx, 210, 210, 80, 30, "Close");
    button6.onTouchHandlers.push_back(graph_manual_test1_close);
    Label label1(graph_manual_test1_gfx, 100, 130, 100, 20, "Label1");
    frame.child(graph_manual_test1_button);
    frame.child(graph_manual_test1_button2);
    frame.child(button3);
    frame.child(button4);
    frame.child(button5);
    frame.child(button6);
    frame.child(label1);
    graph_manual_test1_gui.child(frame);

    graph_manual_test1_frame2.child(graph_manual_test1_lbtn);
    graph_manual_test1_frame2.setScrollXYMax(350, 350);
    graph_manual_test1_frame2.onDrawHandlers.push_back(graph_manual_test1_draw);
    graph_manual_test1_gui.child(graph_manual_test1_frame2);

    graph_manual_test1_lbtn.onDrawHandlers.push_back(graph_manual_test1_lbtn_draw);

    graph_manual_test1_gui.loop();

    return 0;
}
