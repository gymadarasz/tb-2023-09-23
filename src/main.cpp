#include <iostream>
#include <cstdio>

#include "includes/madlib/madlib.hpp"
#include "includes/madlib/trading/trading.hpp"
#include "includes/madlib/trading/bitstamp/bitstamp.hpp"
#include "includes/madlib/trading/strategy/ACandleStrategy.hpp"

using namespace std;
using namespace madlib::trading;
using namespace madlib::trading::bitstamp;
using namespace madlib::trading::strategy;


class BitstampHistoryApplication: public FrameApplication {
protected:

    const string symbol = "BTCUSD";
    const ms_t startTime = datetime_to_ms("2023-10-25 00:50:00");
    const ms_t endTime = datetime_to_ms("2023-10-28 01:00:00");
    const ms_t period = period_to_ms("1m");
    BitstampHistory history = BitstampHistory(symbol, startTime, endTime, period);

    Zoom zoom;
    Chart::CandleStyle candleStyle;
    
    const bool showCandles = true;
    const bool showPrices = true;
    const bool showVolumes = true;
    const Color priceColor = orange;
    const Color volumeColor = darkGray;
    TradeHistoryChartPlugin tradeHistoryChartPlugin = TradeHistoryChartPlugin(
        history, zoom, candleStyle, 
        showCandles, showPrices, showVolumes, 
        priceColor, volumeColor
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
    ACandleStrategy aCandleStrategy = ACandleStrategy(testExchange, strategyParameters);

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;
    MultiChartAccordion multiChartAccordion = MultiChartAccordion(
        gfx, multiChartAccordionLeft, multiChartAccordionTop, multiChartAccordionWidth);

    const int multiChartAccordionFramesHeight = 340;
    const bool showBalanceQuotedScale = true;
    const Chart::LabelStyle buyTextStyle = Chart::LabelStyle(red);
    const Chart::LabelStyle sellTextStyle = Chart::LabelStyle(green);
    const Chart::LabelStyle errorTextStyle = Chart::LabelStyle(gray);
    CandleStrategyTesterPlugin tester = CandleStrategyTesterPlugin(
        history, tradeHistoryChartPlugin,
        testExchange, aCandleStrategy, symbol, 
        multiChartAccordionFramesHeight,
        showBalanceQuotedScale,
        buyTextStyle, sellTextStyle, errorTextStyle
    );
public:
    void init() override {
        FrameApplication::init();
        gui.setTitle("Bitstamp History");

        tester.project(multiChartAccordion, NULL);

        // ----------------

        mainFrame.child(multiChartAccordion);
    }
};


class TradingApplication: public CommandLineApplication {
public:
    TradingApplication(int argc, char* argv[]):
        CommandLineApplication(argc, argv, {
            {"test-command", {
                TradingApplication::testCommand, 
                "Test command"
            }},
            {"parse-bitstamp-csv", {
                TradingApplication::parseBitstampCsv, 
                "Parsing Bitstamp candle history csv\n"
                "Parameters:\n"
                "\tparse-bitstamp-csv {symbol} {from-year} {to-year} {period(optional, default=minute)} {override(TODO)}\n"
                "Note: to add more candle history, you should download and add manually\n"
                ".csv files first two line ignored as header and source informations\n"
                "see more at https://www.cryptodatadownload.com/data/bitstamp/"
            }},
            {"bitstamp-history", {
                TradingApplication::backtestBitstamp,
                "In progress..."
            }}
        })
    {}

    static int testCommand(CommandLineApplication*, CommandArguments args) {
        cout << "Hello Test!" << endl;
        cout << "Your argument(s): " << vector_concat(args, ", ") << endl;
        return 0;
    }

    static int parseBitstampCsv(CommandLineApplication*, CommandArguments args) {
        const string symbol = args.at(0);
        const int yearFrom = parse<int>(args.at(1));
        const int yearTo = parse<int>(args.at(2));
        const string period = vector_has(args, 3) ? args.at(3) : "period";
        bitstamp_parse_candle_history_csv_all(symbol, yearFrom, yearTo, period);
        return 0;
    }

    static int backtestBitstamp(CommandLineApplication*, CommandArguments) {
        // TODO: pass arguments
        (new BitstampHistoryApplication)->run();
        return 0;
    }
};

int main(int argc, char* argv[])
{
    return (TradingApplication(argc, argv)).getResult();
}
