#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

GFX* chart_manual_test2_gfxPtr;
Chart* chart_manual_test2_chartPtr;
Button* chart_manual_test2_closeOkBtnPtr;

void chart_manual_test2_draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart_manual_test2_chartPtr->draw();
    chart_manual_test2_closeOkBtnPtr->draw();
}

void chart_manual_test2_close(void*, unsigned int, int, int) {
    chart_manual_test2_gfxPtr->close = true;
}

int chart_manual_test2()
{
    GFX gfx;
    Zoom zoom;
    chart_manual_test2_gfxPtr = &gfx;
    GUI gui(gfx, zoom, 800, 600, "chart_manual_test2", black);
    Chart chart(gfx, zoom, 5, 5, 790, 590);
    chart.createScale(zoom, LINE, &green);
    chart_manual_test2_chartPtr = &chart;
    gui.addDrawHandler(chart_manual_test2_draw);
    chart.createScale(zoom, LABEL, &black);


    Button closeOkBtn(gfx, zoom, 10, 10, 100, 30, "Ok");
    chart_manual_test2_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test2_close);
    gui.child(closeOkBtn);


    vector<RealPoint> realPoints;
    vector<RealPoint> textRealChoords;
    vector<string> texts;
    int i = 0;
    for (double x = 0; x < 100; x++) {
        double y = randd(0, 100) + x*5;
        RealPoint realPoint(x, y);
        realPoints.push_back(realPoint);
        i++;
        if (i > 10) {
            i = 0;
            textRealChoords.push_back(realPoint);
            texts.push_back(to_string(x));
        }
    }
    

    chart.getScaleAt(0)->project(realPoints);
    chart.getScaleAt(1)->project(textRealChoords, texts);
    chart.draw();


    gui.loop();
    
    return 0;
}
