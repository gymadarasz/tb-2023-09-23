#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class ChartManualTest6: public ManualTestApplication {
protected:

    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    // const double volumeMean = 100;  // Initial volume
    // const double volumeStdDeviation = 10;
    // const double priceMean = 100;  // Initial price
    // const double priceStdDeviation = 10;
    // const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01");
    const ms_t endTime = datetime_to_ms("2021-01-02");
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 6000;

    TradeCandleHistory* history = NULL;
    CandleHistoryChart* chart = NULL;
public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~ChartManualTest6() {
        delete chart;
    }

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("ChartManualTest6");

        // Create a MonteCarloTradeCandleHistory object with the specified parameters
        history = (TradeCandleHistory*)sharedFactory.create(
            "build/src/shared/trading/history/MonteCarloTradeCandleHistory", 
            "MonteCarloTradeCandleHistory",
            new TradeCandleHistory::Args({
                symbol, 
                startTime, endTime, period,
                // volumeMean, volumeStdDeviation,
                // priceMean, priceStdDeviation,
                // timeLamda, seed
            })
        );

        chart = new CandleHistoryChart(
            gfx, 10, 60, 1580, 780, *history
        );

        mainFrame.child(*chart);

        chart->draw();
    }
};
