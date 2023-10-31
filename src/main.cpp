#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;



class MainApplication: public FrameApplication {
protected:
    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;
    MultiChartAccordion multiChartAccordion = MultiChartAccordion(
        gfx, multiChartAccordionLeft, multiChartAccordionTop, multiChartAccordionWidth);

public:
    void init() override {
        FrameApplication::init();

        const string& symbol = "MONTECARLO";

        const ms_t startTime = datetime_to_ms("2020-01-01 00:00:00.000");
        const ms_t endTime = datetime_to_ms("2026-07-04 00:00:00.000");
        const ms_t period = period_to_ms("1h");
        const double volumeMean = 10000;
        const double volumeStdDeviation = 10000;
        const double priceMean = 2000;
        const double priceStdDeviation = 100;
        double timeLambda = (double)period_to_ms("10m");
        const unsigned int seed = 10100;
        const MonteCarloHistory history(
            symbol, startTime, endTime, period, 
            volumeMean, volumeStdDeviation, 
            priceMean, priceStdDeviation, 
            timeLambda, seed
        );

        
        const Zoom zoom;
        const Chart::CandleStyle candleStyle;
        const bool showCandles = true;
        const bool showPrices = true;
        const bool showVolumes = true;
        const Color priceColor = orange;
        const Color volumeColor = darkGray;
        TradeHistoryChartPlugin tradeHistoryChartPlugin(
            history, zoom, candleStyle, 
            showCandles, showPrices, showVolumes, 
            priceColor, volumeColor
        );


        const double feeMarketPc = 0.04; //0.4;
        const double feeLimitPc = 0.03;
        const Fees fees(feeMarketPc, feeMarketPc, feeLimitPc, feeLimitPc);
        const map<string, Pair> pairs = {
            { symbol, Pair("MONTE", "CARLO", fees, 1000) },
        };
        const map<string, Balance> balances = {
            { "MONTE", Balance(10) },
            { "CARLO", Balance(100000) },
        };
        TestExchange testExchange(pairs, balances);
        ACandleStrategy aCandleStrategy(testExchange);

        
        const int multiChartAccordionFramesHeight = 340;
        const bool showBalanceQuotedScale = true;
        const Chart::LabelStyle buyTextStyle(red);
        const Chart::LabelStyle sellTextStyle(green);
        const Chart::LabelStyle errorTextStyle(gray);
        CandleStrategyTesterPlugin tester(
            history, tradeHistoryChartPlugin,
            testExchange, aCandleStrategy, symbol, 
            multiChartAccordionFramesHeight,
            showBalanceQuotedScale,
            buyTextStyle, sellTextStyle, errorTextStyle
        );
        tester.project(multiChartAccordion, NULL);

        // ----------------

        mainFrame.child(multiChartAccordion);
    }
};

int main()
{
    MainApplication app;
    app.run();

    return 0;
}
