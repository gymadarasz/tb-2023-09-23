#include <iostream>
#include <cstdio>

#include "includes/madlib/madlib.hpp"
#include "includes/madlib/trading/trading.hpp"
#include "includes/madlib/trading/bitstamp/bitstamp.hpp"

using namespace std;
using namespace madlib::graph;
using namespace madlib::trading;
using namespace madlib::trading::bitstamp;


struct Config {
    static const string testExchangePath;
    static const string candleStrategyPath;

    static const string symbol;

    static constexpr const double feeMarketPc = 0.04; //0.4;
    static constexpr const double feeLimitPc = 0.03;
    static const Fees fees;
    static const vector<string> symbols;
    static const map<string, Pair> pairs;
    static const map<string, Balance> balances;
};
const string Config::testExchangePath = "build/src/shared/trading/exchange/test";
const string Config::candleStrategyPath = "build/src/shared/trading/strategy/candles";
const string Config::symbol = "BTCUSD";
const Fees Config::fees = Fees(
    Config::feeMarketPc, Config::feeMarketPc, 
    Config::feeLimitPc, Config::feeLimitPc
);
const vector<string> Config::symbols = {
    "BTCUSD", "ETHUSD", "DOGEUSD"
};
const map<string, Pair> Config::pairs = {
    { Config::symbol, Pair("BTC", "USD", Config::fees, 38000) },
};
const map<string, Balance> Config::balances = {
    { "BTC", Balance(1) },
    { "USD", Balance(10000) },
};
// TestExchange Config::testExchange = TestExchange(Config::symbols, pairs, balances);



class BitstampHistoryApplication: public FrameApplication {
protected:

    static BitstampHistoryApplication* app;

    const ms_t startTime = datetime_to_ms("2014-01-01 00:00:00");
    const ms_t endTime = datetime_to_ms("2023-10-25 05:00:00");
    const ms_t period = period_to_ms("1m");
    BitstampHistory history = BitstampHistory(Config::symbol, startTime, endTime, period);


    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(Config::symbol)},
    };

    TestExchange* testExchange = NULL;
    // (TestExchange*)sharedFactory.create(
    //     "build/src/shared/trading/exchange/test", "DefaultTestExchange", 
    //     new TestExchange::Args({ Config::symbols, Config::pairs, Config::balances })
    // );
    
    CandleStrategy* candleStrategy = NULL;
    // (Strategy*)sharedFactory.create(
    //     "build/src/shared/trading/strategy", "ACandleStrategy", 
    //     new Strategy::Args({ *testExchange, strategyParameters})
    // );

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;
    const int multiChartAccordionFramesHeight = 340;
    const bool showBalanceQuotedScale = true;
    

    Select* exchangeSelect = NULL;
    Select* symbolSelect = NULL;
    Select* candleStrategySelect = NULL;
    Button* startButton = NULL;
    CandleStrategyBacktesterMultiChart* candleStrategyBacktesterMultiChart = NULL;

    vector<string> getExchangeClasses() {
        DBG("reading exchange files...");
        vector<string> exchangeFiles = file_find_by_extension(Config::testExchangePath, ".so");
        for (string& exchangeFile: exchangeFiles) exchangeFile = filename_extract(exchangeFile, true);
        return exchangeFiles;
    }

    vector<string> getStrategyClasses() {
        DBG("reading strategy files...");
        vector<string> strategyFiles = file_find_by_extension(Config::candleStrategyPath, ".so");
        for (string& strategyFile: strategyFiles) strategyFile = filename_extract(strategyFile, true);
        return strategyFiles;
    }

    static void onExchangeSelected(void*, unsigned int, int, int) {
        DBG("exchange selected.");
        app->exchangeSelect->setDefval(app->exchangeSelect->getInput()->getText());
        app->loadExchange();
        app->loadSymbols();
        app->symbolSelect->getInput()->draw();
    }

    static void onSymbolSelected(void*, unsigned int, int, int) {
        DBG("symbol selected.");
        app->symbolSelect->setDefval(app->symbolSelect->getInput()->getText());
    }

    static void onCandleStrategySelected(void*, unsigned int, int, int) {
        DBG("strategy selected.");
        app->candleStrategySelect->setDefval(app->candleStrategySelect->getInput()->getText());
        app->loadStrategy();
    }

    static void onStartPushed(void*, unsigned int, int, int) {
        app->candleStrategyBacktesterMultiChart->backtest();
        app->candleStrategyBacktesterMultiChart->draw();
    }

    void createExchangeSelect() {
        DBG("creating exchange select...");
        vector<string> values = getExchangeClasses();
        string defval = values.size() == 1 ? values[0] : "";
        exchangeSelect = new Select(mainFrame, 10, 10, "Exchange", values, defval);
        Input* exchangeInput = exchangeSelect->getInput();
        exchangeInput->addTouchHandler(onExchangeSelected);
        if (!defval.empty()) loadExchange();
    }

    void createSymbolSelect() {
        DBG("creating symbol select...");
        symbolSelect = new Select(mainFrame, 300, 10, "Symbol", {}, "", 80);
        Input* symbolInput = symbolSelect->getInput();
        symbolInput->addTouchHandler(onSymbolSelected);
        loadSymbols();
    }

    void createStrategySelect() {
        DBG("creating strategy select...");
        vector<string> values = getStrategyClasses();
        string defval = values.size() == 1 ? values[0] : "";
        candleStrategySelect = new Select(mainFrame, 500, 10, "Strategy", values, defval);
        Input* candleStrategyInput = candleStrategySelect->getInput();
        candleStrategyInput->addTouchHandler(onCandleStrategySelected);
        if (!defval.empty()) loadStrategy();
    }

    void createStartButton() {
        startButton = new Button(gfx, 800, 10, 100, 20, "Start");
        mainFrame.child(*startButton);
        startButton->addTouchHandler(onStartPushed);
    }

    void createCandleStrategyBacktesterMultiChart() {
        candleStrategyBacktesterMultiChart = new CandleStrategyBacktesterMultiChart(
            gfx, 
            multiChartAccordionLeft, 
            multiChartAccordionTop, 
            multiChartAccordionWidth, 
            multiChartAccordionFramesHeight,
            startTime, endTime,
            history, *testExchange, *candleStrategy, Config::symbol
        );
        mainFrame.child(*candleStrategyBacktesterMultiChart);
    }

    void loadExchange() {
        DBG("loading exchange...");
        // load the selected exchange lib
        Input* exchangeInput = exchangeSelect->getInput();
        testExchange = (TestExchange*)sharedFactory.create(
            Config::testExchangePath, exchangeInput->getText(), 
            new TestExchange::Args(
                // TODO: args from user with a settings form...??
                { Config::symbols, Config::pairs, Config::balances }
            )
        );
    }

    void loadStrategy() {
        DBG("loading strategy...");
        // load the selected strategy lib
        Input* candleStrategyInput = candleStrategySelect->getInput();
        candleStrategy = (CandleStrategy*)sharedFactory.create(
            Config::candleStrategyPath, candleStrategyInput->getText(), 
            new CandleStrategy::Args(
                // TODO: args from user with a settings form...??
                { *testExchange, strategyParameters }
            )
        );
    }

    void loadSymbols() {
        DBG("loading symbols...");        
        // If no exchange selected clear symbol selection
        if (!testExchange) {
            DBG("no exchange, remove symbols...");
            symbolSelect->setValues({});
            symbolSelect->setDefval("");
            symbolSelect->getInput()->setText("");
            return;
        }

        DBG("updating symbols select...");
        vector<string> symbols = testExchange->getSymbols();
        const string defaultSymbol = symbols.size() > 0 ? symbols[0] : "";
        symbolSelect->setValues(symbols);
        symbolSelect->setDefval(defaultSymbol);
        symbolSelect->getInput()->setText(defaultSymbol);
    }

public:

    using FrameApplication::FrameApplication;

    virtual ~BitstampHistoryApplication() {
        delete exchangeSelect;
        delete symbolSelect;
        delete candleStrategySelect;
        delete startButton;
        delete candleStrategyBacktesterMultiChart;
    }

    void init() override {
        FrameApplication::init();
        gui.setTitle("Bitstamp History Backtest");
        app = this;

        createExchangeSelect();
        createSymbolSelect();
        createStrategySelect();
        createStartButton();
        createCandleStrategyBacktesterMultiChart();
    }
};
BitstampHistoryApplication* BitstampHistoryApplication::app = NULL;

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
    delete (new BitstampHistoryApplication)->run();
    return 0;
}

int main(int argc, const char* argv[])
{
    LOG("Main thread started.");
    try { 
        if (argv[1] && !strcmp(argv[1], "test-command")) return test(argc, argv);
        if (argv[1] && !strcmp(argv[1], "download-bitstamp-csv")) return download_bitstamp_csv(argc, argv);
        if (argv[1] && !strcmp(argv[1], "bitstamp-history")) return bitstamp_history(argc, argv);
    } catch (exception &e) {
        const string errmsg = "Exception in main thread: " + string(e.what());
        LOG(errmsg);
        cout << errmsg << endl;
        cout << "Use '$ " << argv[0] << " help' for more info..." << endl;
        return 1;
    }
    return help(argc, argv);
}
