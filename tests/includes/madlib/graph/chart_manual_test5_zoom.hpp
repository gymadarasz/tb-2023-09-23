#pragma once

#include "../../../../src/includes/madlib/Factory.hpp"
#include "../../../../src/includes/madlib/graph/GUI.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/graph/Button.hpp"

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
    chart_manual_test5_gfxPtr->close();
}

int chart_manual_test5_zoom()
{
    GFX gfx;
    chart_manual_test5_gfxPtr = &gfx;
    GUI gui(&gfx, 1600, 600, "chart_manual_test5_zoom");
    // Chart chart(gfx, zoom, 10, 10, 1580, 580, PUSHED, black);
    // gui.child(chart);
    // chart.setZoom(zoom);
    // chart_manual_test5_chartPtr = &chart;
    // chart.addDrawHandler(chart_manual_test5_draw);


    Button closeOkBtn(&gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test5_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test5_close);
    gui.child(&closeOkBtn);


    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    // const double volumeMean = 100;  // Initial volume
    // const double volumeStdDeviation = 10;
    // const double priceMean = 100;  // Initial price
    // const double priceStdDeviation = 10;
    // const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01"); // Current time as the start time
    const ms_t endTime = datetime_to_ms("2021-01-07"); // 300 seconds in the future
    const ms_t period = period_to_ms("1h");
    // const unsigned int seed = 6;

    Factory factory = Factory<CandleHistory>();

    // Create a MonteCarloTradeCandleHistory object with the specified parameters
    CandleHistory* history = factory.createInstance(
        "build/release/src/shared/trading/history/MonteCarloTradeCandleHistory/"
        "MonteCarloTradeCandleHistory.so",
        // new TradeCandleHistory::Args({
        symbol, 
        startTime, endTime, period
            // volumeMean, volumeStdDeviation,
            // priceMean, priceStdDeviation,
            // timeLamda, seed
        // })
    );
    Progress progress;
    history->load(progress);

    CandleHistoryChart chart(
        &gfx, 10, 10, 1580, 580,
        history
    );
    chart.setZoomRatio(1.5, 1.5);
    gui.child(&chart);
    chart_manual_test5_chartPtr = &chart;
    chart.addDrawHandler(chart_manual_test5_draw);
    chart.draw();

    gui.loop(false);
    
    return 0;
}
