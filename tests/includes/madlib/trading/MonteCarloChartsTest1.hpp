#pragma once
#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/Factory.hpp"
#include "../../../../src/includes/madlib/trading/periods.hpp"
#include "../../../../src/includes/madlib/trading/CandleStrategyBacktesterMultiChartAccordion.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class MonteCarloChartsTest1: public ManualTestApplication {
protected:
    CandleHistory* history = nullptr;
    CandleStrategyBacktesterMultiChartAccordion* backtester = nullptr;

    const string symbol = "MONTECARLO";
    const ms_t startTime = datetime_to_ms("2020-01-01 00:00:00.000");
    const ms_t endTime = datetime_to_ms("2020-01-10 00:00:00.000");
    const ms_t period = period_to_ms("1h");
    // const double volumeMean = 10000;
    // const double volumeStdDeviation = 10000;
    // const double priceMean = 2000;
    // const double priceStdDeviation = 100;
    // double timeLambda = (double)period_to_ms("10m");
    // const unsigned int seed = 10100;

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

    const vector<string> periods = { ms_to_period(period) };

    Factory<TestExchange> testExchangeFactory = Factory<TestExchange>();
    Factory<CandleStrategy> candleStrategyFactory = Factory<CandleStrategy>();
    Factory<CandleHistory> candleHistoryFactory = Factory<CandleHistory>();

    TestExchange* testExchange = nullptr;

    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(symbol)},
    };

    const bool showBalanceQuotedScale = true;

    CandleStrategy* candleStrategy = nullptr;
public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~MonteCarloChartsTest1() {
        delete backtester;
    }

    virtual void init() override {
        ManualTestApplication::init();
        gui->setTitle("MonteCarloChartsTest1");

        testExchange = testExchangeFactory.createInstance(
            "build/src/shared/trading/exchange/test/DefaultTestExchange/"
            "DefaultTestExchange.so", 
            // new TestExchange::Args({ 
            periods, symbols, pairs, balances 
            //})
        );
        // Create a MonteCarloTradeCandleHistory object with the specified parameters
        history = candleHistoryFactory.createInstance(
            "build/src/shared/trading/history/MonteCarloTradeCandleHistory/"
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
        progress.close();

        candleStrategy = candleStrategyFactory.createInstance(
            "build/src/shared/trading/strategy/ACandleStrategy/"
            "ACandleStrategy.so", nullptr 
        );
    
            // CandleHistory*& candleHistory,
            // TestExchange*& testExchange,
            // CandleStrategy*& candleStrategy,

        backtester = new CandleStrategyBacktesterMultiChartAccordion(
            gfx, 10, 50, 1000, 340,
            startTime, endTime,
            // multiChartAccordion,
            history, // tradeHistoryChart,
            testExchange, candleStrategy, symbol, 
            // multiChartAccordionFramesHeight,
            showBalanceQuotedScale
        );

        backtester->backtest();

        // ----------------

        mainFrame->child(backtester);
    }
};
