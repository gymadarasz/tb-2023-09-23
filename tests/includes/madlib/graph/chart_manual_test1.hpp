#pragma once

#include "../../../../src/includes/madlib/rand.hpp"
#include "../../../../src/includes/madlib/graph/GUI.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/graph/Button.hpp"

using namespace std;
using namespace madlib::graph;

const ms_t chart_manual_test1_scaleXMin = datetime_to_ms("2020-01-01");
const ms_t chart_manual_test1_scaleXMax = datetime_to_ms("2020-01-07");

GFX* chart_manual_test1_gfxPtr;

void chart_manual_test1_generateRealPoints(Chart& chart, vector<Shape*>& points, int div) {
    double y_ = randd(0, 5);
    for (ms_t x = chart_manual_test1_scaleXMin; x <= chart_manual_test1_scaleXMax; x += hour / div) {

        double y = y_ + randd(0, 5);
        y_++;
        if (y_ > 100 + randd(0, 5)) y_ = randd(0, 5);

        points.push_back(chart.createPointShape(x, y));
    }
}


void chart_manual_test1_close(void*, unsigned int, int, int) {
    chart_manual_test1_gfxPtr->close = true;
}

int chart_manual_test1()
{
    GFX gfx;
    chart_manual_test1_gfxPtr = &gfx;
    GUI gui(&gfx, 800, 600, "chart_manual_test1");
    Chart chart(&gfx, 50, 50, 700, 500, 
        chart_manual_test1_scaleXMin, 
        chart_manual_test1_scaleXMax
    );

    Button closeOkBtn(&gfx, 10, 10, 100, 30, "Ok");
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test1_close);
    gui.child(&closeOkBtn);

    // set up UI
    chart.setBackgroundColor(black);
    gui.child(&chart);
    
    // generate data and show on scales
    PointSeries* mainProjector = chart.createPointSeries(nullptr, lightGreen);
    chart_manual_test1_generateRealPoints(
        chart, mainProjector->getShapes(), 1
    );
    chart_manual_test1_generateRealPoints(
        chart, chart.createPointSeries(mainProjector, lightCyan)->getShapes(), 2
    );
    
    gui.loop();
    
    return 0;
}
