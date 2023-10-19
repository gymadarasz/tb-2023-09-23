#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

GFX* chart_manual_test2_gfxPtr;
Chart* chart_manual_test2_chartPtr;

void chart_manual_test2_draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart_manual_test2_chartPtr->draw();
}

void chart_manual_test2_close(void*, unsigned int, int, int) {
    chart_manual_test2_gfxPtr->close = true;
}

int chart_manual_test2()
{
    GFX gfx;
    chart_manual_test2_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, black);
    Chart chart(gui);
    chart_manual_test2_chartPtr = &chart;
    gui.onDrawHandlers.push_back(chart_manual_test2_draw);
    chart.addScale(1, 1, TEXT, black);


    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(chart_manual_test2_close);
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
    

    chart.getScaleAt(0).project(realPoints);
    chart.getScaleAt(1).project(textRealChoords, texts);


    gui.loop();
    
    return 0;
}
