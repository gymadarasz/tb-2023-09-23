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
    TradeHistoryChart tradeHistoryChartPlugin = TradeHistoryChart(
        history, zoom, candleStyle, 
        showCandles, showPrices, showVolumes, 
        priceColor, volumeColor
    );

    const double feeMarketPc = 0.04; //0.4;
    const double feeLimitPc = 0.03;
    const Fees fees = Fees(feeMarketPc, feeMarketPc, feeLimitPc, feeLimitPc);
    const map<string, Pair> pairs = {
        { symbol, Pair("BTC", "USD", fees, 38000) },
    };
    const map<string, Balance> balances = {
        { "BTC", Balance(1) },
        { "USD", Balance(10000) },
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
            {"download-bitstamp-csv", {
                TradingApplication::downloadBitstampCsv, 
                "Download Bitstamp candle history csv\n"
                "Parameters:\n"
                "\tdownload-bitstamp-csv {symbol} {from-year(optional, default=[actual year])} {to-year(optional, default=[actual year])} {period(optional, default=minute)} {override(optional, default=false or true if no year given so override the actual year data)}\n"
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

    static int downloadBitstampCsv(CommandLineApplication*, CommandArguments args) {
        const string symbol = args.at(1);
        const int yearFrom = parse<int>(vector_has(args, 2) ? args.at(2) : ms_to_datetime(now(), "%Y", false));
        const int yearTo = parse<int>(vector_has(args, 3) ? args.at(3) : ms_to_datetime(now(), "%Y", false));
        const string period = vector_has(args, 4) ? args.at(4) : "minute";
        const bool overwrite = vector_has(args, 5) ? parse_bool(args.at(3)) : !(vector_has(args, 2) || vector_has(args, 3));
        bitstamp_download_candle_history_csv_all(symbol, yearFrom, yearTo, period, overwrite);
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
