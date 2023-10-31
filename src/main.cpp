#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;


class ACandleStrategy: public CandleStrategy {
protected:
    const string symbol = "MONTECARLO";
    const double buyAmount = 1;
    Candle prevCandle;
    int stage = 0;
    long tick = 0;
    double exitAt = 0;
public:
    using CandleStrategy::CandleStrategy;

    void onCandleClose(const Candle& candle) override {
        logger.date().writeln(
            ms_to_datetime(candle.getEnd()), 
            " ", candle.getClose(), 
            " QF:", exchange.getBalanceQuotedFull(symbol), 
            " Q:", exchange.getBalanceQuoted(symbol),
            " B:", exchange.getBalanceBase(symbol)
        );  

        double balanceBase09;
        switch (stage)
        {
            case 0: // buy
                balanceBase09 = exchange.getBalanceBase(symbol) * 0.9;
                marketBuy(symbol, buyAmount > balanceBase09 ? balanceBase09 : buyAmount);
                stage = 1;
                break;

            case 1: // sell
                if (tick == 10) {
                    balanceBase09 = exchange.getBalanceBase(symbol) * 0.9;
                    marketSell(symbol, buyAmount > balanceBase09 ? balanceBase09 : buyAmount);
                    stage = 2;
                }
                break;
                
            case 2: // wait  
                if (tick == 30) {
                    tick = -1;
                    stage = 0;
                }
                break;
            
            default:
                break;
        }

        tick++;
        prevCandle = candle;

        // cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
    }
};


class CandleStrategyTester {
public:
    CandleStrategyTester() {

    }
};

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
        const ms_t endTime = datetime_to_ms("2020-01-04 00:00:00.000");
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
        
        const int multiChartAccordionFramesHeight = 340;

        // ---------------

        Chart& historyChart = 
            multiChartAccordion.addChart("History", multiChartAccordionFramesHeight);
        Chart& balanceQuotedChart =
            multiChartAccordion.addChart("Balance (quoted)", multiChartAccordionFramesHeight);
        Chart& balanceBaseChart =
            multiChartAccordion.addChart("Balance (base)", multiChartAccordionFramesHeight);
        multiChartAccordion.openAll();

        // // --------------- 

        // mainFrame.child(multiChartAccordion);


        const double feeMarketPc = 0.01;
        const double feeLimitPc = 0.001;
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

        // strategy test runner:
        const bool showBalanceQuotedScale = true;

        vector<Candle> candles = history.getCandles();
        vector<RealPoint> balanceQuotedAtCloses;
        vector<RealPoint> balanceQuotedFullAtCloses;
        vector<RealPoint> balanceBaseAtCloses;
        vector<RealPoint> balanceBaseFullAtCloses;
        Pair& pair = testExchange.getPairs().at(symbol);
        for (const Candle& candle: candles) {
            testExchange.setCurrentTime(candle.getEnd());
            pair.setPrice(candle.getClose()); // TODO: set the price to a later price (perhaps next open price) so that, we can emulate some exchange communication latency
           
            double currentTime = (double)candle.getEnd();

            balanceQuotedAtCloses.push_back(RealPoint(
                currentTime,
                testExchange.getBalanceQuoted(pair)
            ));
            
            balanceQuotedFullAtCloses.push_back(RealPoint(
                currentTime,
                testExchange.getBalanceQuotedFull(pair)
            ));
           
            balanceBaseAtCloses.push_back(RealPoint(
                currentTime,
                testExchange.getBalanceBase(pair)
            ));
            
            balanceBaseFullAtCloses.push_back(RealPoint(
                currentTime,
                testExchange.getBalanceBaseFull(pair)
            ));

            aCandleStrategy.onCandleClose(candle);
        }
        
        const Zoom zoom;
        const Chart::CandleStyle candleStyle;
        const bool showCandles = true;
        const bool showPrices = false;
        const bool showVolumes = false;
        const Color priceColor = orange;
        const Color volumeColor = darkGray;
        TradeHistoryChartPlugin tradeHistoryChartPlugin(
            history, zoom, candleStyle, 
            showCandles, showPrices, showVolumes, 
            priceColor, volumeColor
        );
        tradeHistoryChartPlugin.project(historyChart);

        Chart::LabelStyle buyTextStyle(red);
        Chart::LabelStyle sellTextStyle(green);
        Chart::LabelStyle errorTextStyle(gray);
        Chart::Scale& buyTextScale = historyChart.addScale(LABEL, &buyTextStyle);
        Chart::Scale& sellTextScale = historyChart.addScale(LABEL, &sellTextStyle);
        Chart::Scale& errorTextScale = historyChart.addScale(LABEL, &errorTextStyle);
        Chart::Scale::alignXY(historyChart.getScaleAt(0), buyTextScale);
        Chart::Scale::alignXY(historyChart.getScaleAt(0), sellTextScale);
        Chart::Scale::alignXY(historyChart.getScaleAt(0), errorTextScale);
        buyTextScale.project(aCandleStrategy.getBuyTextRealChoords(), aCandleStrategy.getBuyTexts(), false);
        sellTextScale.project(aCandleStrategy.getSellTextRealChoords(), aCandleStrategy.getSellTexts(), false);
        errorTextScale.project(aCandleStrategy.getErrorTextRealChoords(), aCandleStrategy.getErrorTexts(), false);

        Chart::Scale& balanceQuotedFullScale = balanceQuotedChart.addScale(LINE, &lightGreen);
        balanceQuotedFullScale.adaptXY(balanceQuotedFullAtCloses);
        if (showBalanceQuotedScale) {
            Chart::Scale& balanceQuotedScale = balanceQuotedChart.addScale(LINE, &green);
            balanceQuotedScale.adaptXY(balanceQuotedAtCloses);
            Chart::Scale::alignXY(balanceQuotedScale, balanceQuotedFullScale);
            balanceQuotedScale.project(balanceQuotedAtCloses, false);
        }
        balanceQuotedFullScale.project(balanceQuotedFullAtCloses, false);

        Chart::Scale& balanceBaseScaleFull = balanceBaseChart.addScale(LINE, &yellow);
        balanceBaseScaleFull.adaptXY(balanceBaseFullAtCloses);
        Chart::Scale& balanceBaseScale = balanceBaseChart.addScale(LINE, &orange);
        balanceBaseScale.adaptXY(balanceBaseAtCloses);
        Chart::Scale::alignXY(balanceBaseScale, balanceBaseScaleFull);
        balanceBaseScale.project(balanceBaseAtCloses, false);
        balanceBaseScaleFull.project(balanceBaseFullAtCloses, false);


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
