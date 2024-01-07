#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../libs/clib/clib/time.hpp"
#include "../../../../src/includes/madlib/Factory.hpp"
#include "../../../../src/includes/madlib/graph/MultiChartAccordion.hpp"
#include "../../../../src/includes/madlib/trading/CandleHistory.hpp"
#include "../../../../src/includes/madlib/trading/periods.hpp"
#include "../../../../src/includes/madlib/trading/inicators/EmaIndicator.hpp"

using namespace clib;
using namespace madlib::graph;
using namespace madlib::trading;

class TestCandleHistoryChartReload: public ManualTestApplication {
protected:
    static TestCandleHistoryChartReload* app;

    Factory<CandleHistory> candleHistoryFactory;

    MultiChartAccordion* multiChartAccordion = nullptr;
    CandleHistory* candleHistory = nullptr;
    CandleHistoryChart* candleHistoryChart = nullptr;
    Button* addChartButton = nullptr;

    static void addChartTouchHandler(void*, unsigned int, int, int) {
        app->multiChartAccordion->createChart("New", 300);
        app->multiChartAccordion->draw();
    }

public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~TestCandleHistoryChartReload() {
        delete multiChartAccordion;
        delete candleHistoryChart;
        delete addChartButton;
    }

    virtual void init() override {
        ManualTestApplication::init();
        gui->setTitle("CandleHistoryChartReload");

        app = this;

        ms_t startTime = datetime_to_ms("2020-01-01");
        ms_t endTime = datetime_to_ms("2023-01-01");
        const string symbol = "BTCUSD";
        ms_t period = period_to_ms("1m");

        multiChartAccordion = new MultiChartAccordion(gfx, 10, 50, 1000, startTime, endTime);

        candleHistory = candleHistoryFactory.createInstance(
            "build/release/src/shared/trading/history/BitstampCandleHistory/BitstampCandleHistory.so", 
            symbol, startTime, endTime, period
        );

        Progress progress;
        candleHistory->load(progress);

        candleHistoryChart = new CandleHistoryChart(
            gfx, 0, 0, 0, 0, candleHistory, //candleStrategy.getLabelSeries(),
            true, false, false
        );
        multiChartAccordion->createChartFrame("History", candleHistoryChart, 600);
        // multiChartAccordion->createChart("Second", 300);
        // multiChartAccordion->createChart("Third", 300);

        addChartButton = new Button(gfx, 150, 10, 100, 30, "Add..");
        addChartButton->addTouchHandler(addChartTouchHandler);
        
        mainFrame->child(multiChartAccordion);
        mainFrame->child(addChartButton);

        
        const vector<Candle>& candles = candleHistory->getCandles();
        EmaIndicator emaIndicator1 = EmaIndicator(
            candleHistoryChart, candles[0].getOpen(), 2000, blue
        );
        EmaIndicator emaIndicator2 = EmaIndicator(
            candleHistoryChart, candles[0].getOpen(), 4000, green
        );
        EmaIndicator emaIndicator3 = EmaIndicator(
            candleHistoryChart, candles[0].getOpen(), 16000, orange
        );
        for (const Candle& candle: candles) {
            emaIndicator1.calc(candle.getEnd(), candle.getClose());    
            emaIndicator2.calc(candle.getEnd(), candle.getClose());    
            emaIndicator3.calc(candle.getEnd(), candle.getClose());    
        }
    }
};
TestCandleHistoryChartReload* TestCandleHistoryChartReload::app = nullptr;