#include <iostream>
#include <cstdio>

#include "includes/madlib/madlib.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib::graph;
using namespace madlib::trading;


struct Config {
    static const string candleHistoryPath;
    static const string testExchangePath;
    static const string candleStrategyPath;

    static const string symbol;

    static constexpr const double feeMarketPc = 0.04; //0.4;
    static constexpr const double feeLimitPc = 0.03;
    static const Fees fees;
    static const vector<string> periods;
    static const vector<string> symbols;
    static const map<string, Pair> pairs;
    static const map<string, Balance> balances;
    
    static const ms_t startTime;
    static const ms_t endTime;
};
const string Config::candleHistoryPath = "build/src/shared/trading/history";
const string Config::testExchangePath = "build/src/shared/trading/exchange/test";
const string Config::candleStrategyPath = "build/src/shared/trading/strategy";

const string Config::symbol = "BTCUSD";


const Fees Config::fees = Fees(
    Config::feeMarketPc, Config::feeMarketPc, 
    Config::feeLimitPc, Config::feeLimitPc
);
const vector<string> Config::periods = {
    "1m", "1h", "1d"
};
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
const ms_t Config::startTime = datetime_to_ms("2023-01-01 00:00:00");
const ms_t Config::endTime = now();


class BitstampHistoryApplication: public FrameApplication {
protected:

    static BitstampHistoryApplication* app;


    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(Config::symbol)},
    };



    const int settingsTop = 10;
    const int settingsRowHeight = 30;

    const int multiChartAccordionLeft = 10;
    const int multiChartAccordionTop = settingsTop + settingsRowHeight * 2;
    const int multiChartAccordionWidth = 1000;
    const int multiChartAccordionFramesHeight = 340;

    const bool showBalanceQuotedScale = true;

    
    CandleHistory* candleHistory = NULL;
    TestExchange* testExchange = NULL;
    CandleStrategy* candleStrategy = NULL;

    Select* historySelect = NULL;
    DateRange* historyDateRange = NULL;
    Select* exchangeSelect = NULL;
    Select* periodSelect = NULL;
    Select* symbolSelect = NULL;
    Select* candleStrategySelect = NULL;
    Button* downloadButton = NULL;
    Button* startButton = NULL;
    CandleStrategyBacktesterMultiChartAccordion* candleStrategyBacktesterMultiChartAccordion = NULL;

    vector<string> getClassFolders(const string& path) {
        vector<string> files = file_find_by_extension(path); // , ".so");
        for (string& file: files) file = filename_extract(file, true);
        return files;
    }

    vector<string> getHistoryClasses() {
        return getClassFolders(Config::candleHistoryPath);
    }

    vector<string> getExchangeClasses() {
        return getClassFolders(Config::testExchangePath);
    }

    vector<string> getStrategyClasses() {
        return getClassFolders(Config::candleStrategyPath);
    }

    static void onHistorySelected(void*, unsigned int, int, int) {
        app->historySelect->setValue(app->historySelect->getInput()->getText());
        app->historySelect->getInput()->draw();
        app->loadHistoryModule();
        app->loadHistoryData();
        app->updateCandleStrategyBacktesterMultiChartAccordion();
    }

    static void onHistoryDateRangeFromTouch(void*, unsigned int, int, int) {
        app->loadHistoryData();
        app->updateCandleStrategyBacktesterMultiChartAccordion();
    }

    static void onHistoryDateRangeToTouch(void*, unsigned int, int, int) {
        app->loadHistoryData(); // TODO: validate the interval in DateRange handlers (from can not be greater then to!!)
        app->updateCandleStrategyBacktesterMultiChartAccordion();
    }

    static void onExchangeSelected(void*, unsigned int, int, int) {
        app->exchangeSelect->setValue(app->exchangeSelect->getInput()->getText());
        app->loadExchangeModule();
        app->loadPeriods();
        app->loadSymbols();
        app->periodSelect->getInput()->draw();
        app->symbolSelect->getInput()->draw();
    }

    static void onPeriodSelected(void*, unsigned int, int, int) {
        app->periodSelect->setValue(app->periodSelect->getInput()->getText());
    }

    static void onSymbolSelected(void*, unsigned int, int, int) {
        app->symbolSelect->setValue(app->symbolSelect->getInput()->getText());
    }

    static void onCandleStrategySelected(void*, unsigned int, int, int) {
        app->candleStrategySelect->setValue(app->candleStrategySelect->getInput()->getText());
        app->loadStrategyModule();
    }

    static void onDownloadTouch(void*, unsigned int, int, int) {
        const string symbol = app->symbolSelect->getInput()->getText();        
        const ms_t from = datetime_to_ms(app->historyDateRange->getFromInput()->getText());
        const ms_t to = datetime_to_ms(app->historyDateRange->getToInput()->getText());
        const ms_t period = period_to_ms(app->periodSelect->getInput()->getText());
        const bool override = zenity_question("Override", "Do you want to override if data already exists?");
        
        app->candleHistory->setSymbol(symbol);
        app->candleHistory->setStartTime(from);
        app->candleHistory->setEndTime(to);
        app->candleHistory->setPeriod(period);
        Progress progress("Download history...", true);
        app->candleHistory->download(progress, override);
        progress.close();
    }

    static void onStartTouch(void*, unsigned int, int, int) {
        app->candleStrategyBacktesterMultiChartAccordion->backtest();
        app->candleStrategyBacktesterMultiChartAccordion->draw();
    }

    void createHistorySelect() {
        vector<string> values = getHistoryClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        historySelect = new Select(mainFrame, 10, settingsTop, "History", values, defval);
        Input* historyInput = historySelect->getInput();
        historyInput->addTouchHandler(onHistorySelected);
        if (!defval.empty()) {
            loadHistoryModule();
            loadHistoryData();
        }
    }

    void createHistoryDateRange() {
        historyDateRange = new DateRange(mainFrame, 300, settingsTop, "Date", Config::startTime, Config::endTime);
        historyDateRange->getFromInput()->addTouchHandler(onHistoryDateRangeFromTouch);
        historyDateRange->getToInput()->addTouchHandler(onHistoryDateRangeToTouch);
    }

    void createExchangeSelect() {
        vector<string> values = getExchangeClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        exchangeSelect = new Select(mainFrame, 10, settingsTop + settingsRowHeight, "Exchange", values, defval);
        Input* exchangeInput = exchangeSelect->getInput();
        exchangeInput->addTouchHandler(onExchangeSelected);
        if (!defval.empty()) loadExchangeModule();
    }

    void createPeriodSelect() {
        periodSelect = new Select(mainFrame, 600, settingsTop, "Period", {}, "", 50);
        Input* periodInput = periodSelect->getInput();
        periodInput->addTouchHandler(onPeriodSelected);
        loadPeriods();
    }

    void createSymbolSelect() {
        symbolSelect = new Select(mainFrame, 300, settingsTop + settingsRowHeight, "Symbol", {}, "", 80);
        Input* symbolInput = symbolSelect->getInput();
        symbolInput->addTouchHandler(onSymbolSelected);
        loadSymbols();
    }

    void createStrategySelect() {
        vector<string> values = getStrategyClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        candleStrategySelect = new Select(mainFrame, 600, settingsTop + settingsRowHeight, "Strategy", values, defval);
        Input* candleStrategyInput = candleStrategySelect->getInput();
        candleStrategyInput->addTouchHandler(onCandleStrategySelected);
        if (!defval.empty()) loadStrategyModule();
    }

    void createDownloadButton() {
        downloadButton = new Button(gfx, 910, settingsTop, 90, 20, "Download");
        mainFrame.child(*downloadButton);
        downloadButton->addTouchHandler(onDownloadTouch);
    }

    void createStartButton() {
        startButton = new Button(gfx, 910, settingsTop + settingsRowHeight, 90, 20, "Start");
        mainFrame.child(*startButton);
        startButton->addTouchHandler(onStartTouch);
    }

    void createCandleStrategyBacktesterMultiChartAccordion() {
        const ms_t startTime = datetime_to_ms(historyDateRange->getFromInput()->getText());
        const ms_t endTime = datetime_to_ms(historyDateRange->getToInput()->getText());

        // if (candleStrategyBacktesterMultiChartAccordion) {
        //     candleStrategyBacktesterMultiChartAccordion->clear();
        //     return;
        //     // delete candleStrategyBacktesterMultiChartAccordion;
        //     // candleStrategyBacktesterMultiChartAccordion = NULL;
        // }
        
        candleStrategyBacktesterMultiChartAccordion = 
            new CandleStrategyBacktesterMultiChartAccordion(
                gfx, 
                multiChartAccordionLeft, 
                multiChartAccordionTop, 
                multiChartAccordionWidth, 
                multiChartAccordionFramesHeight,
                startTime, endTime,
                candleHistory, *testExchange, *candleStrategy, Config::symbol
            );
        mainFrame.child(*candleStrategyBacktesterMultiChartAccordion);
    }

    void loadHistoryModule() {
        const string period = periodSelect->getInput()->getText();
        const string symbol = symbolSelect->getInput()->getText();
        const ms_t start = datetime_to_ms(historyDateRange->getFromInput()->getText());
        const ms_t end = datetime_to_ms(historyDateRange->getToInput()->getText());

        string moduleName = historySelect->getInput()->getText();

        // if (!candleHistory)
        candleHistory = (CandleHistory*)sharedFactory.create(
            candleHistory,
            Config::candleHistoryPath + "/" + moduleName, moduleName,
            new CandleHistory::Args(
                { symbol, start, end, period_to_ms(period) }
            )
        );

        // if (candleStrategyBacktesterMultiChartAccordion)
        //     candleStrategyBacktesterMultiChartAccordion->setCandleHistory(*candleHistory);

        // createCandleStrategyBacktesterMultiChartAccordion();
        // else candleHistory->clear();

        // loadHistoryData();
    }

    void loadHistoryData() {

        const ms_t startTime = datetime_to_ms(historyDateRange->getFromInput()->getText());
        const ms_t endTime = datetime_to_ms(historyDateRange->getToInput()->getText());
        const string symbol = symbolSelect->getInput()->getText();
        const ms_t period = period_to_ms(periodSelect->getInput()->getText());
                
        candleHistory->setStartTime(startTime);
        candleHistory->setEndTime(endTime);
        candleHistory->setSymbol(symbol);
        candleHistory->setPeriod(period);
        Progress progress("Loading history...");
        candleHistory->load(progress);
        progress.close();

        // if (candleStrategyBacktesterMultiChartAccordion) {
        //     candleStrategyBacktesterMultiChartAccordion->clear();
        //     candleStrategyBacktesterMultiChartAccordion->draw();
        // }
    }

    void loadExchangeModule() {
        // load the selected exchange lib
        string moduleName = exchangeSelect->getInput()->getText();
        // if (!testExchange)
        testExchange = (TestExchange*)sharedFactory.create(
            testExchange,
            Config::testExchangePath + "/" + moduleName, moduleName, 
            new TestExchange::Args(
                // TODO: args from user with a settings form...??
                { Config::periods, Config::symbols, Config::pairs, Config::balances }
            )
        );
    }

    void loadStrategyModule() {
        // load the selected strategy lib
        string moduleName = candleStrategySelect->getInput()->getText();
        // if (!candleStrategy)
        candleStrategy = (CandleStrategy*)sharedFactory.create(
            candleStrategy, 
            Config::candleStrategyPath + "/" + moduleName, moduleName, 
            new CandleStrategy::Args(
                // TODO: args from user with a settings form...??
                { *testExchange, strategyParameters }
            )
        );
    }

    void loadPeriods() {  
        // If no exchange selected clear period selection
        if (!testExchange) {
            periodSelect->setValues({});
            periodSelect->setValue("");
            periodSelect->getInput()->setText("");
            return;
        }

        vector<string> periods = testExchange->getPeriods();
        const string defaultPeriod = periods.size() > 0 ? periods[0] : "";
        periodSelect->setValues(periods);
        periodSelect->setValue(defaultPeriod);
        periodSelect->getInput()->setText(defaultPeriod);
    }

    void loadSymbols() {     
        // If no exchange selected clear symbol selection
        if (!testExchange) {
            symbolSelect->setValues({});
            symbolSelect->setValue("");
            symbolSelect->getInput()->setText("");
            return;
        }

        vector<string> symbols = testExchange->getSymbols();
        const string defaultSymbol = symbols.size() > 0 ? symbols[0] : "";
        symbolSelect->setValues(symbols);
        symbolSelect->setValue(defaultSymbol);
        symbolSelect->getInput()->setText(defaultSymbol);
    }


    void updateCandleStrategyBacktesterMultiChartAccordion() {
        candleStrategyBacktesterMultiChartAccordion->clearCharts();
        candleStrategyBacktesterMultiChartAccordion->getCandleHistoryChart().generateFromHistory();
        candleStrategyBacktesterMultiChartAccordion->getCandleHistoryChart().fitTimeRangeToHistory();
        candleStrategyBacktesterMultiChartAccordion->draw();
    }

public:

    using FrameApplication::FrameApplication;

    virtual ~BitstampHistoryApplication() {
        delete historySelect;
        delete historyDateRange;
        delete exchangeSelect;
        delete periodSelect;
        delete symbolSelect;
        delete candleStrategySelect;
        delete downloadButton;
        delete startButton;
        delete candleStrategyBacktesterMultiChartAccordion;
    }

    virtual void init() override {
        FrameApplication::init();
        gui.setTitle("Bitstamp History Backtest");
        app = this;

        createExchangeSelect(); // TODO: !@# test each form element and fix them
        createStrategySelect();
        createPeriodSelect();
        createSymbolSelect();
        createHistoryDateRange();
        createHistorySelect();
        createDownloadButton();
        createStartButton();
        createCandleStrategyBacktesterMultiChartAccordion();
    }
};
BitstampHistoryApplication* BitstampHistoryApplication::app = NULL;

int backtest(int, const char* []) {
    // TODO: pass arguments
    delete (new BitstampHistoryApplication)->run();
    return 0;
}

int help(int, const char* argv[]) {
    cout <<
        "Usages: $ " << argv[0] << " [COMMAND] [OPTIONS...]\n"
        "Commands:\n\n"
        // "   test-command\n\n"
        "   help\n\n"
        "   backtest\n\n" // TODO...
        << endl;
    return 0;
}

int main(int argc, const char* argv[])
{
    LOG("Main thread started.");
    try { 
        // if (argv[1] && !strcmp(argv[1], "test-command")) return test(argc, argv);
        if (argv[1] && !strcmp(argv[1], "backtest")) return backtest(argc, argv);
    } catch (exception &e) {
        const string errmsg = "Exception in main thread: " + string(e.what());
        cout << errmsg << endl;
        cout << "Use '$ " << argv[0] << " help' for more info..." << endl;
        return 1;
    }
    return help(argc, argv);
}
