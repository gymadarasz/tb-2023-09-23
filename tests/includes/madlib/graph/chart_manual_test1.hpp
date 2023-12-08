#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

const double chart_manual_test1_scaleXMin = 0;
const double chart_manual_test1_scaleXMax = 1000;

GFX* chart_manual_test1_gfxPtr;

void chart_manual_test1_generateRealPoints(vector<Coord>& coords) {
    coords.clear();

    double y_ = randd(0, 5);
    for (double x = chart_manual_test1_scaleXMin; x <= chart_manual_test1_scaleXMax; x += 1) {

        double y = y_ + randd(0, 5);
        y_++;
        if (y_ > 100 + randd(0, 5)) y_ = randd(0, 5);

        Coord coord(x, y);
        coords.push_back(coord);
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
    Chart chart(gfx, 50, 50, 700, 500);

    Button closeOkBtn(gfx, 10, 10, 100, 30, "Ok");
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test1_close);
    gui.child(closeOkBtn);

    // set up UI
    chart.setBackgroundColor(black);
    gui.child(chart);
    
    // generate data and show on scales
    vector<Coord> coords;
    chart_manual_test1_generateRealPoints(coords);

    chart.createScale(LINE, &lightGreen).project(coords);
    Chart::Scale& secondScale = chart.createScale(LINE, &lightCyan);
    secondScale.setZoomRatio(2.0, 1.5);
    secondScale.project(coords);
    

    gui.loop();
    
    return 0;
}
