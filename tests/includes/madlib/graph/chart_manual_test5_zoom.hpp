#pragma once

#include "../../../../src/includes/madlib/madlib.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

GFX* chart_manual_test5_gfxPtr;
Chart* chart_manual_test5_chartPtr;
Button* chart_manual_test5_closeOkBtnPtr;

void chart_manual_test5_draw(void* /*context*/) {
    chart_manual_test5_chartPtr->draw();
    chart_manual_test5_closeOkBtnPtr->draw();
}

void chart_manual_test5_close(void*, unsigned int, int, int) {
    chart_manual_test5_gfxPtr->close = true;
}

int chart_manual_test5_zoom()
{
    GFX gfx;
    chart_manual_test5_gfxPtr = &gfx;
    GUI gui(gfx, 1600, 600);
    Frame frame(gfx, 10, 10, 1580, 580, BUTTON_PUSHED, black);
    gui.child(frame);
    Chart chart(frame);
    Zoom zoom(1.5, 1.5);
    chart.setZoom(zoom);
    chart_manual_test5_chartPtr = &chart;
    frame.onDrawHandlers.push_back(chart_manual_test5_draw);


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test5_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(chart_manual_test5_close);
    gui.child(closeOkBtn);


    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    const double volumeMean = 100;  // Initial volume
    const double volumeStdDeviation = 10;
    const double priceMean = 100;  // Initial price
    const double priceStdDeviation = 10;
    const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01"); // Current time as the start time
    const ms_t endTime = datetime_to_ms("2021-01-07"); // 300 seconds in the future
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 6;

    // Create a MonteCarloHistory object with the specified parameters
    MonteCarloHistory history(
        // symbol, 
        startTime, endTime, period,
        volumeMean, volumeStdDeviation,
        priceMean, priceStdDeviation,
        timeLambda, seed
    );

    Chart::CandleStyle candleStyle;
    TradeHistoryChartPlugin candlesPlugin(history, zoom, candleStyle);
    candlesPlugin.project(chart);

    gui.loop();
    
    return 0;
}
