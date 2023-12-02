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

public:

    void init() override {
        ManualTestApplication::init();
        gui.setTitle("LabelManualTest1");

        mainFrame.child(label1);
    }
};
