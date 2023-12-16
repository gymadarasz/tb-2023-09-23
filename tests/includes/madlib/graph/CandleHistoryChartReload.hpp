#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace madlib::graph;

class CandleHistoryChartReload: public ManualTestApplication {
protected:

public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~CandleHistoryChartReload() {}

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("CandleHistoryChartReload");

        
    }
};
