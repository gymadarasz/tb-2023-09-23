#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class ChartManualTest7Zoom: public ManualTestApplication {
protected:

    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    const double volumeMean = 100;  // Initial volume
    const double volumeStdDeviation = 10;
    const double priceMean = 100;  // Initial price
    const double priceStdDeviation = 10;
    const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01");
    const ms_t endTime = datetime_to_ms("2021-01-07");
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 6000;

    // Create a MonteCarloHistory object with the specified parameters
    MonteCarloHistory history = MonteCarloHistory(
        symbol, 
        startTime, endTime, period,
        volumeMean, volumeStdDeviation,
        priceMean, priceStdDeviation,
        timeLambda, seed
    );

    TradeHistoryChart chart = TradeHistoryChart(
        gfx, zoom, 300, 300, 1000, 300, history
    );


    // test sliders...

    SlideBar slideBarHorizontal = SlideBar(gfx, zoom, 10, 60, 200, HORIZONTAL);
    ScrollBar scrollBarHorizontal = ScrollBar(gfx, zoom, 10, 90, 200, HORIZONTAL);
    IntervalBar intervalBarHorizontal = IntervalBar(gfx, zoom, 10, 120, 200, HORIZONTAL);

    SlideBar slideBarVertical = SlideBar(gfx, zoom, 260, 60, 200, VERTICAL);
    ScrollBar scrollBarVertical = ScrollBar(gfx, zoom, 290, 60, 200, VERTICAL);
    IntervalBar intervalBarVertical = IntervalBar(gfx, zoom, 320, 60, 200, VERTICAL);

public:

    void init() override {
        ManualTestApplication::init();
        gui.setTitle("ChartManualTest7Zoom");

        mainFrame.child(slideBarHorizontal);
        mainFrame.child(scrollBarHorizontal);
        mainFrame.child(intervalBarHorizontal);
        mainFrame.child(slideBarVertical);
        mainFrame.child(scrollBarVertical);
        mainFrame.child(intervalBarVertical);

        mainFrame.child(chart);

        chart.project();
    }
};
