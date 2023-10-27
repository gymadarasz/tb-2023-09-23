#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

const double chart_manual_test1_scaleXMin = 0;
const double chart_manual_test1_scaleXMax = 1000;

GFX* chart_manual_test1_gfxPtr;
Chart* chart_manual_test1_chartPtr;

void chart_manual_test1_draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart_manual_test1_chartPtr->drawLines(0, lightGreen);
    chart_manual_test1_chartPtr->drawPoints(0, white);

    chart_manual_test1_chartPtr->drawLines(1, lightCyan);
}

void chart_manual_test1_generateRealPoints(vector<RealPoint>& realPoints) {
    realPoints.clear();

    double y_ = randd(0, 5);
    for (double x = chart_manual_test1_scaleXMin; x <= chart_manual_test1_scaleXMax; x += 1) {

        double y = y_ + randd(0, 5);
        y_++;
        if (y_ > 100 + randd(0, 5)) y_ = randd(0, 5);

        RealPoint realPoint(x, y);
        realPoints.push_back(realPoint);
    }
}


void chart_manual_test1_close(void*, unsigned int, int, int) {
    chart_manual_test1_gfxPtr->close = true;
}

int chart_manual_test1()
{
    GFX gfx;
    chart_manual_test1_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test1");
    Frame frame(gfx, 50, 50, 700, 500);
    Chart chart(frame);
    chart_manual_test1_chartPtr = &chart;

    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(chart_manual_test1_close);
    gui.child(closeOkBtn);

    // set up UI
    frame.fixed = false;
    frame.setBackgroundColor(black);
    gui.child(frame);
    frame.onDrawHandlers.push_back(chart_manual_test1_draw);
    
    // generate data and show on scales
    vector<RealPoint> realPoints;
    chart_manual_test1_generateRealPoints(realPoints);

    chart.addScale().project(realPoints);
    Scale& secondScale = chart.addScale();
    secondScale.setZoom(Zoom(2.0, 1.5));
    secondScale.project(realPoints);
    

    gui.loop();
    
    return 0;
}
