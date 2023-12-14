#pragma once

#include "../../../../src/includes/madlib/trading/trading.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../ManualTestApplication.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class MonteCarloChartsTest1: public ManualTestApplication {
protected:
    MonteCarloTradeCandleHistory* history = NULL;
    CandleStrategyBacktesterMultiChartAccordion* backtester = NULL;

    const string symbol = "MONTECARLO";
    const ms_t startTime = datetime_to_ms("2020-01-01 00:00:00.000");
    const ms_t endTime = datetime_to_ms("2020-01-10 00:00:00.000");
    const ms_t period = period_to_ms("1h");
    const double volumeMean = 10000;
    const double volumeStdDeviation = 10000;
    const double priceMean = 2000;
    const double priceStdDeviation = 100;
    double timeLambda = (double)period_to_ms("10m");
    const unsigned int seed = 10100;

    const double feeMarketPc = 0.04; //0.4;
    const double feeLimitPc = 0.03;
    const Fees fees = Fees(feeMarketPc, feeMarketPc, feeLimitPc, feeLimitPc);
    const map<string, Pair> pairs = {
        { symbol, Pair("MONTE", "CARLO", fees, 1000) },
    };
    const map<string, Balance> balances = {
        { "MONTE", Balance(10) },
        { "CARLO", Balance(100000) },
    };
    const vector<string> symbols = {
        "MONTECARLO"
    };
    TestExchange* testExchange = (TestExchange*)sharedFactory.create(
        "build/src/shared/trading/exchange/test", "DefaultTestExchange", 
        new TestExchange::Args({ { ms_to_period(period) }, symbols, pairs, balances })
    );
    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(symbol)},
    };

    const bool showBalanceQuotedScale = true;

    CandleStrategy* candleStrategy = 
        (CandleStrategy*)sharedFactory.create(
            "build/src/shared/trading/strategy",
            "ACandleStrategy", 
            new CandleStrategy::Args({ *testExchange, strategyParameters })
        );
public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~MonteCarloChartsTest1() {
        delete history;
        delete backtester;
    }

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("MonteCarloChartsTest1");


        MonteCarloTradeCandleHistory::Args context = MonteCarloTradeCandleHistory::Args({
            symbol, 
            startTime, endTime, period,
            volumeMean, volumeStdDeviation,
            priceMean, priceStdDeviation,
            timeLambda, seed
        });
        history = new MonteCarloTradeCandleHistory(&context);
        history->init();

    
        backtester = new CandleStrategyBacktesterMultiChartAccordion(
            gfx, 10, 50, 1000, 340,
            startTime, endTime,
            // multiChartAccordion,
            *history, // tradeHistoryChart,
            *testExchange, *candleStrategy, symbol, 
            // multiChartAccordionFramesHeight,
            showBalanceQuotedScale
        );

        backtester->backtest();

        // ----------------

        mainFrame.child(*backtester);
    }
};