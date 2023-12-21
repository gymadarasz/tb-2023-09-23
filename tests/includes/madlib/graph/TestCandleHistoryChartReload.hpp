#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/time.hpp"
#include "../../../../src/includes/madlib/Factory.hpp"
#include "../../../../src/includes/madlib/graph/MultiChartAccordion.hpp"

using namespace madlib::graph;

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
            "build/src/shared/trading/history/BitstampCandleHistory/BitstampCandleHistory.so", 
            symbol, startTime, endTime, period
        );

        Progress progress;
        candleHistory->load(progress);

        candleHistoryChart = new CandleHistoryChart(
            gfx, 0, 0, 0, 0, candleHistory, //candleStrategy.getLabelSeries(),
            true, false, false
        );
        multiChartAccordion->createChartFrame("History", candleHistoryChart, 300);
        multiChartAccordion->createChart("Second", 300);
        multiChartAccordion->createChart("Third", 300);

        addChartButton = new Button(gfx, 150, 10, 100, 30, "Add..");
        addChartButton->addTouchHandler(addChartTouchHandler);
        
        mainFrame->child(multiChartAccordion);
        mainFrame->child(addChartButton);
    }
};
TestCandleHistoryChartReload* TestCandleHistoryChartReload::app = nullptr;