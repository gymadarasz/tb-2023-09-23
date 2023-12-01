#pragma once

#include "../../../../src/includes/madlib/trading/trading.hpp"
#include "../../../../src/includes/madlib/trading/strategy/ACandleStrategy.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../ManualTestApplication.hpp"

using namespace madlib::graph;
using namespace madlib::trading;
using namespace madlib::trading::strategy;

class MonteCarloChartsTest1: public ManualTestApplication {
protected:

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;

    MultiChartAccordion multiChartAccordion = MultiChartAccordion(
        gfx, zoom, 
        multiChartAccordionLeft, 
        multiChartAccordionTop, 
        multiChartAccordionWidth
    );

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
    const MonteCarloHistory history = MonteCarloHistory(
        symbol, 
        startTime, endTime, period, 
        volumeMean, volumeStdDeviation, 
        priceMean, priceStdDeviation, 
        timeLambda, seed
    );

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
    TestExchange testExchange = TestExchange(pairs, balances);
    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(symbol)},
    };
    TradeTexts tradeTexts;
    ACandleStrategy aCandleStrategy = ACandleStrategy(
        testExchange, strategyParameters, tradeTexts
    );
    
    const bool showCandles = true;
    const bool showPrices = true;
    const bool showVolumes = true;
    const bool showTexts = true;
    const Color priceColor = orange;
    const Color volumeColor = darkGray;
    const Border border = Theme::chartBorder;
    const Color backgroundColor = Theme::chartBackgroundColor;
    const Chart::CandleStyle candleStyle;
    const Chart::LabelStyle buyTextStyle = Chart::LabelStyle(red);
    const Chart::LabelStyle sellTextStyle = Chart::LabelStyle(green);
    const Chart::LabelStyle errorTextStyle = Chart::LabelStyle(gray);
    TradeHistoryChart tradeHistoryChart = TradeHistoryChart(
        gfx, zoom, 10, 10, 300, 150, history, aCandleStrategy.getTradeTexts(), 
        showCandles, showPrices, showVolumes, showTexts, 
        priceColor, volumeColor, border, backgroundColor, 
        candleStyle, buyTextStyle, sellTextStyle, errorTextStyle
    );


    
    const int multiChartAccordionFramesHeight = 300;
    const bool showBalanceQuotedScale = true;
    CandleStrategyBacktester backtester = CandleStrategyBacktester(
        multiChartAccordion, zoom,
        history, tradeHistoryChart,
        testExchange, aCandleStrategy, symbol, 
        multiChartAccordionFramesHeight,
        showBalanceQuotedScale
    );

public:
    void init() override {
        ManualTestApplication::init();
        gui.setTitle("MonteCarloChartsTest1");
        backtester.backtest();

        // ----------------

        mainFrame.child(multiChartAccordion);
    }
};