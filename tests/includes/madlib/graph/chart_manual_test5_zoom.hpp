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
    //chart_manual_test5_chartPtr->draw();
    chart_manual_test5_closeOkBtnPtr->draw();
}

void chart_manual_test5_close(void*, unsigned int, int, int) {
    chart_manual_test5_gfxPtr->close = true;
}

int chart_manual_test5_zoom()
{
    GFX gfx;
    Zoom zoom(1.5, 1.5);
    chart_manual_test5_gfxPtr = &gfx;
    GUI gui(gfx, zoom, 1600, 600);
    // Chart chart(gfx, zoom, 10, 10, 1580, 580, BUTTON_PUSHED, black);
    // gui.child(chart);
    // chart.setZoom(zoom);
    // chart_manual_test5_chartPtr = &chart;
    // chart.addDrawHandler(chart_manual_test5_draw);


    Button closeOkBtn(gfx, zoom, 15, 15, 100, 30, "Ok");
    chart_manual_test5_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test5_close);
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
        symbol, 
        startTime, endTime, period,
        volumeMean, volumeStdDeviation,
        priceMean, priceStdDeviation,
        timeLambda, seed
    );

    TradeHistoryChart chart(gfx, zoom, 10, 10, 1580, 580, history);
    gui.child(chart);
    chart.setZoom(zoom);
    chart_manual_test5_chartPtr = &chart;
    chart.addDrawHandler(chart_manual_test5_draw);    
    chart.project();

    gui.loop();
    
    return 0;
}
