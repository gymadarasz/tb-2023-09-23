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
    const ms_t start = datetime_to_ms("2020-01-01");
    const ms_t finish = datetime_to_ms("2020-01-07");
    GFX gfx;
    chart_manual_test2_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test2", black);
    Chart chart(gfx, 5, 5, 790, 590, start, finish);
    PointSeries* mainSeriesProjector = chart.createPointSeries(NULL, true, green);
    chart_manual_test2_chartPtr = &chart;
    gui.addDrawHandler(chart_manual_test2_draw);
    LabelSeries* textSeriesProjector = chart.createLabelSeries(mainSeriesProjector);


    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    chart_manual_test2_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test2_close);
    gui.child(closeOkBtn);


    vector<Shape*>& coords = mainSeriesProjector->getShapes();
    vector<Shape*>& textRealChoords = textSeriesProjector->getShapes();
    int i = 0;
    for (ms_t x = start; x < finish; x+= hour) {
        double y = randd(0, 100000000) + (double)x*5;
        coords.push_back(chart.createPointShape(x, y));
        i++;
        if (i > 10) {
            i = 0;
            textRealChoords.push_back(chart.createLabelShape(x, y, to_string(x)));
        }
    }

    gui.loop();
    
    return 0;
}
