#pragma once

#include "../graph/MultiChartAccordion.hpp"

#include "Pair.hpp"
#include "Strategy.hpp"
#include "TestExchange.hpp"
#include "CandleStrategy.hpp"
#include "CandleStrategyBacktester.hpp"
#include "CandleHistoryChart.hpp"

namespace madlib::trading {
    
    class CandleStrategyBacktesterMultiChartAccordion: public MultiChartAccordion {
    protected:

        CandleStrategyBacktester* backtester = nullptr;

        CandleHistory*& candleHistory;
        TestExchange& testExchange;
        Strategy& candleStrategy;
        const string& symbol;
        const bool showBalanceQuotedScale;
        const bool logProgress; // TODO: progress callbacks 
        const bool showProgress;
        const ms_t showProgressAfterMs;
        const ms_t showProgressFreqMs;

        // **** tradeHistoryChart ****
        
        CandleHistoryChart candleHistoryChart = CandleHistoryChart(
            gfx, 0, 0, 0, 0, candleHistory, //candleStrategy.getLabelSeries(),
            true, false, false
        );

        // **** balanceQuotedChart ****

        Chart* balanceQuotedChart = nullptr;
        PointSeries* balanceQuotedFullScale = nullptr;
        PointSeries* balanceQuotedScale = nullptr;

        // **** balanceBaseChart ****

        Chart* balanceBaseChart = nullptr;
        PointSeries* balanceBaseFullScale = nullptr;
        PointSeries* balanceBaseScale = nullptr;

        struct ProgressState {
            // show log and progress
            size_t candlesRemaining = 0, candlesSize = 0;
            ms_t progressUpdatedAt = 0;
            Progress* progress = nullptr;

            // show results on charts:
            vector<Shape*>* balanceQuotedAtCloses = nullptr;
            vector<Shape*>* balanceQuotedFullAtCloses = nullptr;
            vector<Shape*>* balanceBaseAtCloses = nullptr;
            vector<Shape*>* balanceBaseFullAtCloses = nullptr;
            Pair* pair = nullptr;

            bool showProgressStarted = false;

            void createProgress(const string& title) {
                progress = new Progress(title);
            }

            virtual ~ProgressState() {
                if (progress) delete progress;
            }
        } progressState;
        
        static bool onProgressStart(CandleStrategyBacktester::ProgressContext& progressContext) {
            CandleStrategyBacktesterMultiChartAccordion* that = (CandleStrategyBacktesterMultiChartAccordion*)progressContext.context;

            that->clearCharts();
            that->progressState.balanceQuotedAtCloses = 
                &that->balanceQuotedScale->getShapes();
            that->progressState.balanceQuotedFullAtCloses = 
                &that->balanceQuotedFullScale->getShapes();
            that->progressState.balanceBaseAtCloses = 
                &that->balanceBaseScale->getShapes();
            that->progressState.balanceBaseFullAtCloses = 
                &that->balanceBaseFullScale->getShapes();

            if (that->showProgress || that->logProgress) {
                that->progressState.candlesSize = 
                    that->progressState.candlesRemaining = 
                        that->candleHistory->getCandles().size();
                that->progressState.progressUpdatedAt = 
                    now() + that->showProgressAfterMs;
                if (that->logProgress) 
                    LOG("Backtest starts with ", that->progressState.candlesRemaining, " candles...");
                if (that->showProgress)
                    that->progressState.showProgressStarted = false;
            }

            that->progressState.pair = &that->testExchange.getPairAt(that->symbol);

            return true;
        }
        
        static bool onProgressStep(CandleStrategyBacktester::ProgressContext& progressContext) {
            CandleStrategyBacktesterMultiChartAccordion* that = (CandleStrategyBacktesterMultiChartAccordion*)progressContext.context;

            // show progress and log

            if (that->showProgress || that->logProgress) {
                if (now() - that->progressState.progressUpdatedAt >= that->showProgressFreqMs) {
                    that->progressState.progressUpdatedAt = now();
                    int pc100 = (int)((1 - ((double)that->progressState.candlesRemaining / (double)that->progressState.candlesSize)) * 100);
                    if (that->logProgress) LOG("Backtest in progress: ", that->progressState.candlesRemaining, " candles remaining... (" + to_string(pc100) + "% done)");
                    if (that->showProgress) {

                        if (!that->progressState.showProgressStarted) {
                            that->progressState.createProgress("Backtest...");
                            that->progressState.showProgressStarted = true;
                        }

                        if (
                            !that->progressState.progress->update(pc100) ||
                            !that->progressState.progress->update("Remaining candles: " + to_string(that->progressState.candlesRemaining))
                        ) {
                            LOG("User canceled.");
                            return false;
                        }
                    }
                }
                that->progressState.candlesRemaining--; 
            }
            
            // collect data to charts

            ms_t currentTime = progressContext.candle->getEnd();
            Pair* pair = that->progressState.pair;

            // **** balanceQuotedChart ****

            if (that->showBalanceQuotedScale) {
                that->progressState.balanceQuotedAtCloses->push_back(that->balanceQuotedChart->createPointShape(
                    currentTime,
                    that->testExchange.getBalanceQuoted(*pair)
                ));
            }
            
            that->progressState.balanceQuotedFullAtCloses->push_back(that->balanceQuotedChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceQuotedFull(*pair)
            ));

            // **** balanceBaseChart ****
        
            that->progressState.balanceBaseAtCloses->push_back(that->balanceBaseChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceBase(*pair)
            ));
            
            that->progressState.balanceBaseFullAtCloses->push_back(that->balanceBaseChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceBaseFull(*pair)
            ));

            return true;
        }
        
        static bool onProgressFinish(
            CandleStrategyBacktester::ProgressContext& progressContext
        ) {
            CandleStrategyBacktesterMultiChartAccordion* that = 
                (CandleStrategyBacktesterMultiChartAccordion*)progressContext.context;

            if (that->logProgress) LOG("Backtest done.");
            if (that->showProgress && that->progressState.progress) 
                that->progressState.progress->close();

            return true;
        }

    public:

        CandleStrategyBacktesterMultiChartAccordion(
            GFX& gfx, int left, int top, int width,
            const int multiChartAccordionFramesHeight,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            CandleHistory*& candleHistory,
            TestExchange& testExchange,
            CandleStrategy& candleStrategy,
            const string& symbol,
            const bool showBalanceQuotedScale = true, // TODO

            const bool logProgress = true, // TODO
            const bool showProgress = true,
            const ms_t showProgressAfterMs = 0,
            const ms_t showProgressFreqMs = second,

            bool single = false,
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor,
            void* eventContext = nullptr
        ):
            MultiChartAccordion(
                gfx, left, top, width,
                timeRangeBegin, timeRangeEnd,
                single, border, backgroundColor, 
                eventContext
            ),
            candleHistory(candleHistory),
            testExchange(testExchange),
            candleStrategy(candleStrategy),
            symbol(symbol),
            showBalanceQuotedScale(showBalanceQuotedScale),
            logProgress(logProgress),
            showProgress(showProgress),
            showProgressAfterMs(showProgressAfterMs),
            showProgressFreqMs(showProgressFreqMs)
        {

            backtester = new CandleStrategyBacktester(
                this, candleHistory, testExchange, candleStrategy, symbol, 
                onProgressStart, onProgressStep, onProgressFinish
            );

            // **** tradeCandleHistoryChart ****

            createChartFrame(
                "History", candleHistoryChart, multiChartAccordionFramesHeight
            );

            // if (!candleStrategy.getLabelSeries())
            candleStrategy.setLabelSeries(
                candleHistoryChart.getLabelSeries()
            );

            multiChart.attach(candleHistoryChart);

            // **** balanceQuotedChart ****

            balanceQuotedChart = createChart(
                "Balance (quoted)", multiChartAccordionFramesHeight
            );
            balanceQuotedFullScale = balanceQuotedChart->createPointSeries(nullptr, true, lightGreen);
            balanceQuotedScale = balanceQuotedChart->createPointSeries(balanceQuotedFullScale, true, green);

            // **** balanceBaseChart ****

            balanceBaseChart = createChart(
                "Balance (base)", multiChartAccordionFramesHeight
            );
            balanceBaseFullScale = balanceBaseChart->createPointSeries(nullptr, true, yellow);
            balanceBaseScale = balanceBaseChart->createPointSeries(balanceBaseFullScale, true, orange);

            openAll(false);
        }


        virtual ~CandleStrategyBacktesterMultiChartAccordion() {
            delete backtester;
        }

        // void setCandleHistory(const CandleHistory& candleHistory) {
        //     this->candleHistory = candleHistory;
        //     this->backtester->setCandleHistory(candleHistory);
        // }

        CandleHistoryChart& getCandleHistoryChart() {
            return candleHistoryChart;
        }        

        // TODO: separated backtester class that can be reused from command line
        void backtest() {

            if (!backtester->backtest()) {
                LOG("Backtest failed"); // TODO
            }

        }

        virtual void clearCharts() override {
            MultiChartAccordion::clearCharts();
            candleHistoryChart.clearProjectors();
        }

        void resetTimeRangeToHistoryChart() {
            for (Chart* chart: charts) 
                chart->setTimeRangeFullAndApply(
                    candleHistoryChart.getTimeRangeFull()
                );
        }
    };

}