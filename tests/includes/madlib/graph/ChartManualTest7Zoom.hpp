#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class ChartManualTest7Zoom: public ManualTestApplication {
protected:

    static void draw(void* context) {
        ChartManualTest7Zoom* that =
            (ChartManualTest7Zoom*)((Area*)context)
                ->getRoot()->getGFX().getContext();

        that->chart.draw();
        that->closeBtn.draw();
    }

    Frame frame = Frame(gfx, 300, 300, 1000, 300, BUTTON_PUSHED, black);
    Chart chart = Chart(frame);

    SlideBar slideBarHorizontal = SlideBar(gfx, 10, 60, 200, HORIZONTAL);
    ScrollBar scrollBarHorizontal = ScrollBar(gfx, 10, 90, 200, HORIZONTAL);
    IntervalBar intervalBarHorizontal = IntervalBar(gfx, 10, 120, 200, HORIZONTAL);

    SlideBar slideBarVertical = SlideBar(gfx, 260, 60, 200, VERTICAL);
    ScrollBar scrollBarVertical = ScrollBar(gfx, 290, 60, 200, VERTICAL);
    IntervalBar intervalBarVertical = IntervalBar(gfx, 320, 60, 200, VERTICAL);

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

        mainFrame.child(frame);
        frame.onDrawHandlers.push_back(draw);

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
        MonteCarloHistory history(
            symbol, 
            startTime, endTime, period,
            volumeMean, volumeStdDeviation,
            priceMean, priceStdDeviation,
            timeLambda, seed
        );

        const double ratioX = 5;
        const double ratioY = 1;
        Zoom zoom(ratioX, ratioY);
        Chart::CandleStyle candleStyle;
        TradeHistoryChartPlugin candlesPlugin(history, zoom, candleStyle);
        candlesPlugin.project(chart, &history);
    }
};
