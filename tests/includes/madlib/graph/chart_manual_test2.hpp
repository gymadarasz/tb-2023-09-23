#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

GFX* chart_manual_test2_gfxPtr;
Chart* chart_manual_test2_chartPtr;
Button* chart_manual_test2_closeOkBtnPtr;

void chart_manual_test2_draw(void* /*context*/) {
    chart_manual_test2_chartPtr->draw();
    chart_manual_test2_closeOkBtnPtr->draw();
}

void chart_manual_test2_close(void*, unsigned int, int, int) {
    chart_manual_test2_gfxPtr->close = true;
}

int chart_manual_test2()
{
    GFX gfx;
    chart_manual_test2_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test2", black);
    Chart chart(gfx, 5, 5, 790, 590);
    chart.createScale(LINE, &green);
    chart_manual_test2_chartPtr = &chart;
    gui.addDrawHandler(chart_manual_test2_draw);
    chart.createScale(LABEL);


    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    chart_manual_test2_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test2_close);
    gui.child(closeOkBtn);


    vector<Coord> coords;
    vector<Coord> textRealChoords;
    vector<string> texts;
    int i = 0;
    for (double x = 0; x < 100; x++) {
        double y = randd(0, 100) + x*5;
        Coord coord(x, y);
        coords.push_back(coord);
        i++;
        if (i > 10) {
            i = 0;
            textRealChoords.push_back(coord);
            texts.push_back(to_string(x));
        }
    }
    

    chart.getScaleAt(0).project(coords);
    chart.getScaleAt(1).project(textRealChoords, texts);
    chart.draw();


    gui.loop();
    
    return 0;
}
