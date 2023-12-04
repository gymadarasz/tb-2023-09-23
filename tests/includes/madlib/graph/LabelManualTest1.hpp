#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class LabelManualTest1: public ManualTestApplication {
protected:
        
    Label label1 = Label(gfx, 10, 50, 200, 30, "Hello World!");
    Chart chart1 = Chart(gfx, 10, 100, 500, 300);

public:

    void init() override {
        ManualTestApplication::init();
        gui.setTitle("LabelManualTest1");

        vector<Coord> coords;
        double y = 200;
        for (double x = 100; x < 110; x++) 
            coords.push_back(
                Coord(x, y += randd(0, 1))
            );

        chart1.createScale(LINE, true, &yellow).setRealPoints(coords);
        chart1.projectScales();

        mainFrame.child(label1);
        mainFrame.child(chart1);
    }
};
