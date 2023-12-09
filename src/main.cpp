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
    const ms_t startTime = datetime_to_ms("2015-09-25 00:50:00");
    const ms_t endTime = datetime_to_ms("2023-10-25 05:00:00");
    const ms_t period = period_to_ms("1m");
    BitstampHistory history = BitstampHistory(symbol, startTime, endTime, period);

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
    
    ACandleStrategy strategy = ACandleStrategy(
        testExchange, strategyParameters
    );

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;
    const int multiChartAccordionFramesHeight = 340;
    const bool showBalanceQuotedScale = true;
    
    CandleStrategyBacktester tester = CandleStrategyBacktester(
        gfx, 
        multiChartAccordionLeft, 
        multiChartAccordionTop, 
        multiChartAccordionWidth, 
        multiChartAccordionFramesHeight,
        startTime, endTime,
        history, testExchange, strategy, symbol
    );

public:
    void init() override {
        FrameApplication::init();
        gui.setTitle("Bitstamp History Backtest");

        tester.backtest();

        // ----------------

        mainFrame.child(tester);
    }
};

int help(int, const char* argv[]) {
    cout <<
        "Usages: $ " << argv[0] << " [COMMAND] [OPTIONS...]\n"
        "Commands:\n\n"

        "   test-command\n\n"

        "   download-bitstamp-csv\n\n"

        "       Download Bitstamp candle history csv\n"
        "       Parameters:\n"
        "           download-bitstamp-csv {symbol} {from-year(optional, default=[actual year])} {to-year(optional, default=[actual year])} {period(optional, default=minute)} {override(optional, default=false or true if no year given so override the actual year data)}\n"
        "       see more at https://www.cryptodatadownload.com/data/bitstamp/\n\n"

        "   bitstamp-history\n\n" // TODO...
        << endl;
    return 0;
}

int test(int argc, const char* argv[]) {
    const args_t args = args_parse(argc, argv);
    cout << "Hello Test!" << endl;
    cout << "Your argument(s): " << endl;
    for (const auto& arg : args) {
        cout << "   [" << arg.first << "] => \"" << arg.second << "\"" << endl;
    }
    return 0;
}

int download_bitstamp_csv(int argc, const char* argv[]) {

    const args_shortcuts_t shortcuts = {
        {'s', "symbol"},
        {'f', "year-from"},
        {'t', "year-to"},
        {'p', "period"},
        {'o', "overwrite"},
    };
    const args_t args = args_parse(argc, argv, &shortcuts);

    const string symbol = args_get(args, "symbol");
    const string nowstr = ms_to_datetime(now(), "%Y", false);
    const int yearFrom = parse<int>(args_get(args, "year-from", &nowstr));
    const int yearTo = parse<int>(args_get(args, "year-to", &nowstr));
    const string period = args_get(args, "period", "minute");
    const bool overwrite = args_has(args, "overwrite") 
        ? args_has(args, "year-to") 
        : !(args_has(args, "year-from") || args_has(args, "year-to"));
    // const bool overwrite = vector_has(args, 5) ? parse_bool(args.at(3)) : !(vector_has(args, 2) || vector_has(args, 3));
    bitstamp_download_candle_history_csv_all(symbol, yearFrom, yearTo, period, overwrite);
    bitstamp_parse_candle_history_csv_all(symbol, yearFrom, yearTo, period);
    return 0;
}

int bitstamp_history(int, const char* []) {
    // TODO: pass arguments
    (new BitstampHistoryApplication)->run();
    return 0;
}

int main(int argc, const char* argv[])
{
    LOG("Start...");
    // return (TradingApplication(argc, argv)).getResult();
    try {
        if (argv[1] && !strcmp(argv[1], "test-command")) return test(argc, argv);
        if (argv[1] && !strcmp(argv[1], "download-bitstamp-csv")) return download_bitstamp_csv(argc, argv);
        if (argv[1] && !strcmp(argv[1], "bitstamp-history")) return bitstamp_history(argc, argv);
    } catch (exception &e) {
        cout << "Error: " << e.what() << endl;
        cout << "Use '$ " << argv[0] << " help' for more info..." << endl;
        return 1;
    }
    return help(argc, argv);
}
