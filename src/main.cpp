#include <iostream>
#include <cstdio>

#include "includes/madlib/madlib.hpp"
#include "includes/madlib/trading/trading.hpp"
#include "includes/madlib/trading/bitstamp/bitstamp.hpp"

using namespace std;
using namespace madlib::graph;
using namespace madlib::trading;
using namespace madlib::trading::bitstamp;


class Select {
protected:

    const string title = "Select";
    const string prompt = "Select"; // TODO

    
    Label* label = NULL;
    Input* input = NULL;
    vector<string> values;
    string defval;

    static void onSelectTouchHandler(void* context, unsigned int, int, int) {
        Area* that = (Area*)context;
        Select* select = (Select*)that->getEventContext();
        string selection = zenity_combo(
            select->title,
            select->prompt,
            select->label->getText(),
            select->values
        );
        if (selection.empty()) select->input->setText(select->defval);
        else select->input->setText(selection);
        select->input->draw();
    }

public:
    Select(
        Area& parent,
        int left, int top, 
        const string& text, 
        const vector<string>& values = {},
        const string& defval = "",
        const int labelWidth = 100, // TODO
        const int inputWidth = 200,
        const int height = 20 // TODO
    ):
        values(values), 
        defval(defval)
    {
        GFX& gfx = parent.getGFX();
        label = new Label(gfx, left, top, labelWidth, height, text);
        input = new Input(gfx, left + labelWidth, top, inputWidth, height, defval);
        parent.child(*label);
        parent.child(*input);
        input->setEventContext(this);
        input->addTouchHandler(onSelectTouchHandler);
    }

    virtual ~Select() {
        delete label;
        delete input;
    }

    Input* getInput() {
        return input;
    }

    void setDefval(const string& defval) {
        this->defval = defval;
    }

    void setValues(const vector<string>& values) {
        this->values = values;
    }
};



struct Config {
    static const string testExchangePath;

    static const string symbol;

    static constexpr const double feeMarketPc = 0.04; //0.4;
    static constexpr const double feeLimitPc = 0.03;
    static const Fees fees;
    static const vector<string> symbols;
    static const map<string, Pair> pairs;
    static const map<string, Balance> balances;
};
const string Config::testExchangePath = "build/src/shared/trading/exchange/test";
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

    TestExchange* testExchange = (TestExchange*)sharedFactory.create(
        "build/src/shared/trading/exchange/test", "DefaultTestExchange", 
        new TestExchange::Args({ Config::symbols, Config::pairs, Config::balances })
    );
    
    CandleStrategy* strategy = (CandleStrategy*)sharedFactory.create(
        "build/src/shared/trading/strategy", "ACandleStrategy", 
        new CandleStrategy::Args({ *testExchange, strategyParameters})
    );

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = 50;
    const int multiChartAccordionWidth = 1000;
    const int multiChartAccordionFramesHeight = 340;
    const bool showBalanceQuotedScale = true;
    
    CandleStrategyBacktesterMultiChart tester = CandleStrategyBacktesterMultiChart(
        gfx, 
        multiChartAccordionLeft, 
        multiChartAccordionTop, 
        multiChartAccordionWidth, 
        multiChartAccordionFramesHeight,
        startTime, endTime,
        history, *testExchange, *strategy, Config::symbol
    );

    Select* exchangeSelect = NULL;
    Select* symbolSelect = NULL;
    Button* startButton = NULL;

    vector<string> getExchangeClasses() {
        DBG("reading exchange files...");
        vector<string> exchanges = file_find_by_extension(Config::testExchangePath, ".so");
        for (string& exchange: exchanges)
            exchange = filename_extract(exchange, true);
        return exchanges;
    }

    static void onExchangeSelected(void*, unsigned int, int, int) {
        DBG("exchange selected.");
        app->exchangeSelect->setDefval(app->exchangeSelect->getInput()->getText());
        app->loadExchange();
        app->loadSymbols();
    }

    static void onSymbolSelected(void*, unsigned int, int, int) {
        DBG("symbol selected.");
        app->symbolSelect->setDefval(app->symbolSelect->getInput()->getText());
    }

    static void onStartPushed(void*, unsigned int, int, int) {
        app->tester.backtest();
        app->tester.draw();
    }

    void createExchangeSelect() {
        DBG("creating exchange select...");
        vector<string> values = getExchangeClasses();
        string defval = values.size() == 1 ? values[0] : "";
        exchangeSelect = new Select(mainFrame, 10, 10, "Exchange", values, defval);
        Input* exchangeInput = exchangeSelect->getInput();
        exchangeInput->addTouchHandler(onExchangeSelected);
        if (!exchangeInput->getText().empty())
            loadExchange();        
    }

    void createSymbolSelect() {
        DBG("creating symbol select...");
        symbolSelect = new Select(mainFrame, 410, 10, "Symbol");
        Input* symbolInput = symbolSelect->getInput();
        symbolInput->addTouchHandler(onSymbolSelected);
        Input* exchangeInput = exchangeSelect->getInput();
        if (!exchangeInput->getText().empty())
            loadSymbols();
    }

    void createStartButton() {
        startButton = new Button(gfx, 800, 10, 100, 20, "Start");
        mainFrame.child(*startButton);
        startButton->addTouchHandler(onStartPushed);
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

    void loadSymbols() {
        DBG("loading symbols...");        
        // If no exchange selected clear symbol selection
        Input* exchangeInput = exchangeSelect->getInput();
        if (exchangeInput->getText().empty()) {
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
        delete startButton;
    }

    void init() override {
        FrameApplication::init();
        gui.setTitle("Bitstamp History Backtest");
        app = this;

        createExchangeSelect();
        createSymbolSelect();
        createStartButton();

        // symbolInput.addTouchHandler(symbolInputTouchHandler);

        // tester.backtest();

        // ----------------

        mainFrame.child(tester);

        // mainFrame.child(symbolLabel);
        // mainFrame.child(symbolInput);
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
