#pragma once

#include <cassert>

#include "../../../../src/includes/madlib/graph/GUI.hpp"
#include "../../../../src/includes/madlib/graph/GFX.hpp"
#include "../../../../src/includes/madlib/graph/Frame.hpp"
#include "../../../../src/includes/madlib/graph/Button.hpp"
#include "../../../../src/includes/madlib/graph/Label.hpp"

using namespace std;
using namespace madlib::graph;

GFX* graph_manual_test1_gfxPtr;
Button* graph_manual_test1_button2Ptr;
Frame* graph_manual_test1_frame2Ptr;

void graph_manual_test1_doit(void*, unsigned int, int, int) {
    graph_manual_test1_button2Ptr->setBackgroundColor(red);
    graph_manual_test1_button2Ptr->setTextColor(white);
    graph_manual_test1_button2Ptr->draw();
}

void graph_manual_test1_draw(void*) {
    graph_manual_test1_frame2Ptr->brush(green);
    graph_manual_test1_frame2Ptr->rect(10, 10, 50, 50);
    graph_manual_test1_frame2Ptr->rect(120, 100, 150, 150);
    graph_manual_test1_frame2Ptr->fRect(220, 220, 300, 300);
    graph_manual_test1_frame2Ptr->brush(red);
    graph_manual_test1_frame2Ptr->line(10, 10, 50, 50);
    graph_manual_test1_frame2Ptr->line(120, 100, 150, 150);
    graph_manual_test1_frame2Ptr->line(220, 220, 300, 300);
    graph_manual_test1_frame2Ptr->brush(blue);
    graph_manual_test1_frame2Ptr->line(50, 10, 10, 50);
    graph_manual_test1_frame2Ptr->line(300, 220, 220, 300);
    graph_manual_test1_frame2Ptr->rect(25, 25, 325, 325);
    graph_manual_test1_frame2Ptr->brush(black);
    graph_manual_test1_frame2Ptr->line(325, 25, 25, 325);
    graph_manual_test1_frame2Ptr->brush(orange);
    graph_manual_test1_frame2Ptr->hLine(30, 30, 130);
    graph_manual_test1_frame2Ptr->brush(yellow);
    graph_manual_test1_frame2Ptr->vLine(30, 30, 130);
    // TODO: bach painting...
}

void graph_manual_test1_close(void*, unsigned int, int, int) {
    graph_manual_test1_gfxPtr->close = true;
}

void graph_manual_test1_lbtn_draw(void* context) {
    Area* area = (Area*)context;
    area->brush(red);
    area->fRect(5, 5, 25, 25);
}

int graph_manual_test1()
{
    GFX gfx;
    graph_manual_test1_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "graph_manual_test1");
    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    Button button(gfx, 10, 10, 80, 30, "Hello");
    Button button2(gfx, 110, 10, 80, 30, "Click!");
    graph_manual_test1_button2Ptr = &button2;
    Frame frame2(gfx, 320, 100, 200, 200);
    graph_manual_test1_frame2Ptr = &frame2;
    Button lbtn(gfx, 50, 50, 100, 30, "Hello");

    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(graph_manual_test1_close);
    gui.child(closeOkBtn);

    Frame frame(gfx, 100, 100, 200, 200);
    button2.addTouchHandler(graph_manual_test1_doit);
    Button button3(gfx, 210, 10, 80, 30, "Sticky");
    button3.setSticky(true);
    Button button4(gfx, 310, 10, 80, 30, "Hello4");
    Button button5(gfx, 10, 50, 250, 50, "Big Willy");
    Button button6(gfx, 210, 210, 80, 30, "Close");
    button6.addTouchHandler(graph_manual_test1_close);
    Label label1(gfx, 100, 130, 100, 20, "Label1", LEFT, PUSHED);
    frame.child(button);
    frame.child(button2);
    frame.child(button3);
    frame.child(button4);
    frame.child(button5);
    frame.child(button6);
    frame.child(label1);
    gui.child(frame);

    frame2.child(lbtn);
    frame2.setScrollXYMax(350, 350);
    frame2.addDrawHandler(graph_manual_test1_draw);
    gui.child(frame2);

    lbtn.addDrawHandler(graph_manual_test1_lbtn_draw);

    gui.loop();

    return 0;
}
