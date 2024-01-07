#include <iostream>
#include <cstdio>

#include "../libs/clib/clib/time.hpp"
#include "includes/madlib/Factory.hpp"
#include "includes/madlib/graph/FrameApplication.hpp"
#include "includes/madlib/graph/Select.hpp"
#include "includes/madlib/graph/DateRange.hpp"
#include "includes/madlib/graph/Input.hpp"
#include "includes/madlib/graph/Mixed.hpp"
#include "includes/madlib/trading/periods.hpp"
#include "includes/madlib/trading/Fees.hpp"
#include "includes/madlib/trading/Pair.hpp"
#include "includes/madlib/trading/Balance.hpp"
#include "includes/madlib/trading/TestExchange.hpp"
#include "includes/madlib/trading/CandleStrategy.hpp"
#include "includes/madlib/trading/CandleHistory.hpp"
#include "includes/madlib/trading/CandleStrategyBacktesterMultiChartAccordion.hpp"

using namespace std;
using namespace clib;
using namespace madlib;
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

// class SettingsForm: public FrameApplication { // TODO: !@# seems breaks the Montecarlo history settings but otherwise works
// protected:
//     const int paddingTop = 10;
//     const int paddingLeft = 10;
//     const int lineHeight = 30; // TODO

//     SettingsHolder& settingsHolder;
//     vector<Mixed> settings;

//     Button* okButton = nullptr;
//     Button* cancelButton = nullptr;

//     static void onOkButtonTouchHandler(void* context, unsigned int, int, int) {
//         Button* that = (Button*)context;
//         SettingsForm* form = (SettingsForm*)that->getEventContext("SettingsForm");
        
//         // save before close;
//         if (!form->settingsHolder.isValidSettings(form->settings)) return; // Invalid data
//         if (!form->settingsHolder.setSettings(form->settings)) throw ERROR("Unable to set data.");

//         that->getGFX()->close();
//     }

//     static void onCancelButtonTouchHandler(void* context, unsigned int, int, int) {
//         Button* that = (Button*)context;
//         that->getGFX()->close();
//     }    

// public:
//     explicit SettingsForm(
//         SettingsHolder& settingsHolder,
//         int width = Theme::defaultSettingsFormWidth,
//         int height = Theme::defaultSettingsFormHeight,
//         const char* title = Theme::defaultSettingsFormTitle
//     ): 
//         FrameApplication(width, height, title),
//         settingsHolder(settingsHolder),
//         settings(settingsHolder.getSettings())
//     {}

//     virtual ~SettingsForm() {
//         delete okButton;
//         delete cancelButton;
//     }

//     virtual void init() override {
//         FrameApplication::init();
        
//         int row = 0;
//         for (Mixed& setting: settings)
//             setting.createInput(mainFrame, paddingLeft, paddingTop + row++ * lineHeight);
        
//         row++;
        
//         okButton = new Button(gfx, 200, row * lineHeight, 90, 20, "Ok");
//         okButton->setBackgroundColor(green);
//         okButton->setTextColor(white);
//         okButton->setEventContext("SettingsForm", this);
//         okButton->addTouchHandler(onOkButtonTouchHandler);
//         mainFrame->child(okButton);

//         cancelButton = new Button(gfx, 300, row * lineHeight, 90, 20, "Cancel");
//         cancelButton->addTouchHandler(onCancelButtonTouchHandler);
//         mainFrame->child(cancelButton);
//     }
// };

class BitstampHistoryApplication: public FrameApplication {
protected:


    static BitstampHistoryApplication* app;

    Factory<TestExchange> testExchangeFactory = Factory<TestExchange>();
    Factory<CandleStrategy> candleStrategyFactory = Factory<CandleStrategy>();
    Factory<CandleHistory> candleHistoryFactory = Factory<CandleHistory>();


    map<string, Strategy::Parameter> strategyParameters = {
        {"symbol", Strategy::Parameter(Config::symbol)},
    };

    const int settingsLeft = 10;
    const int settingsColWidth = 100;

    const int settingsTop = 10;
    const int settingsRowHeight = 30;

    int getSettingsLeft(int col) {
        return settingsLeft + settingsColWidth * col;
    }

    int getSettingsTop(int row) {
        return settingsTop + settingsRowHeight * row;
    }

    const int buttonWidth = 90;
    const int buttonHeight = 20;

    // const int multiChartAccordionLeft = 10;
    // const int multiChartAccordionTop = settingsTop + settingsRowHeight * 2;
    const int multiChartAccordionWidth = 1000;
    const int multiChartAccordionFramesHeight = 340;

    const bool showBalanceQuotedScale = true;

    
    CandleHistory* candleHistory = nullptr;
    TestExchange* testExchange = nullptr;
    CandleStrategy* candleStrategy = nullptr;

    Select* historySelect = nullptr;
    Button* historySetupButton = nullptr;
    DateRange* historyDateRange = nullptr;
    Select* exchangeSelect = nullptr;
    Select* periodSelect = nullptr;
    Select* symbolSelect = nullptr;
    Select* candleStrategySelect = nullptr;
    Button* reloadButton = nullptr;
    Button* startButton = nullptr;
    CandleStrategyBacktesterMultiChartAccordion* candleStrategyBacktesterMultiChartAccordion = nullptr;

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

    static void onHistorySetupButtonTouch(void*, unsigned int, int, int) {
        MixedInputListForm form(
            *app->candleHistory,
            520, 300, "History Settings"
        );
        form.run(false);
        app->gfx->triggerFakeEvent({ GFX::RELEASE });
    }

    static void onHistoryDateRangeFromTouch(void*, unsigned int, int, int) {
        app->loadHistoryData();
        app->updateCandleStrategyBacktesterMultiChartAccordion();
    }

    static void onHistoryDateRangeToTouch(void*, unsigned int, int, int) {
        app->loadHistoryData();
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

    static void onReloadTouch(void*, unsigned int, int, int) {
        const string symbol = app->symbolSelect->getInput()->getText();        
        const ms_t from = datetime_to_ms(app->historyDateRange->getFromInput()->getText());
        const ms_t to = datetime_to_ms(app->historyDateRange->getToInput()->getText());
        const ms_t period = period_to_ms(app->periodSelect->getInput()->getText());
        
        app->candleHistory->setSymbol(symbol);
        app->candleHistory->setStartTime(from);
        app->candleHistory->setEndTime(to);
        app->candleHistory->setPeriod(period);
        Progress progress("Reload history...", false);
        app->candleHistory->reload(progress);
        app->gfx->triggerFakeEvent({ GFX::RELEASE });
    }

    static void onStartTouch(void*, unsigned int, int, int) {
        app->destroyCandleStrategyBacktesterMultiChartAccordion();
        app->createCandleStrategyBacktesterMultiChartAccordion();
        app->candleStrategyBacktesterMultiChartAccordion->backtest();
        app->candleStrategyBacktesterMultiChartAccordion->draw();
    }

    void createHistorySelect() {
        vector<string> values = getHistoryClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        historySelect = new Select(
            mainFrame, 
            getSettingsLeft(0), getSettingsTop(0), 
            "History", values, defval
        );
        Input* historyInput = historySelect->getInput();
        historyInput->addTouchHandler(onHistorySelected);
        if (!defval.empty()) {
            loadHistoryModule();
            loadHistoryData();
        }
    }
    
    void createHistorySetupButton() {
        historySetupButton = new Button(
            gfx, 
            getSettingsLeft(3), getSettingsTop(0), 
            buttonWidth, buttonHeight, 
            "Settings.."
        );
        mainFrame->child(historySetupButton);
        historySetupButton->addTouchHandler(onHistorySetupButtonTouch);
    }

    void createHistoryDateRange() {
        historyDateRange = new DateRange(
            mainFrame, 
            getSettingsLeft(4), getSettingsTop(0), 
            "Date", Config::startTime, Config::endTime
        );
        historyDateRange->getFromInput()->addTouchHandler(onHistoryDateRangeFromTouch);
        historyDateRange->getToInput()->addTouchHandler(onHistoryDateRangeToTouch);
    }

    void createExchangeSelect() {
        vector<string> values = getExchangeClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        exchangeSelect = new Select(
            mainFrame, 
            getSettingsLeft(4), getSettingsTop(1), 
            "Exchange", values, defval
        );
        Input* exchangeInput = exchangeSelect->getInput();
        exchangeInput->addTouchHandler(onExchangeSelected);
        if (!defval.empty()) loadExchangeModule();
    }

    void createPeriodSelect() {
        periodSelect = new Select(
            mainFrame, 
            getSettingsLeft(7), getSettingsTop(0), 
            "Period", {}, "", 50
        );
        Input* periodInput = periodSelect->getInput();
        periodInput->addTouchHandler(onPeriodSelected);
        loadPeriods();
    }

    void createSymbolSelect() {
        symbolSelect = new Select(
            mainFrame, 
            getSettingsLeft(7), getSettingsTop(1), 
            "Symbol", {}, "", 80
        );
        Input* symbolInput = symbolSelect->getInput();
        symbolInput->addTouchHandler(onSymbolSelected);
        loadSymbols();
    }

    void createStrategySelect() {
        vector<string> values = getStrategyClasses();
        string defval = values.size() >= 1 ? values[0] : "";
        candleStrategySelect = new Select(
            mainFrame, 
            getSettingsLeft(0), getSettingsTop(1), 
            "Strategy", values, defval
        );
        Input* candleStrategyInput = candleStrategySelect->getInput();
        candleStrategyInput->addTouchHandler(onCandleStrategySelected);
        if (!defval.empty()) loadStrategyModule();
    }

    void createReloadButton() {
        reloadButton = new Button(
            gfx, 
            getSettingsLeft(9), getSettingsTop(0), 
            buttonWidth, buttonHeight, 
            "Reload"
        );
        mainFrame->child(reloadButton);
        reloadButton->addTouchHandler(onReloadTouch);
    }

    void createStartButton() {
        startButton = new Button(
            gfx, 
            getSettingsLeft(9), getSettingsTop(1), 
            buttonWidth, buttonHeight, 
            "Start"
        );
        mainFrame->child(startButton);
        startButton->addTouchHandler(onStartTouch);
    }

    void createCandleStrategyBacktesterMultiChartAccordion() {
        const ms_t startTime = datetime_to_ms(historyDateRange->getFromInput()->getText());
        const ms_t endTime = datetime_to_ms(historyDateRange->getToInput()->getText());
        
        candleStrategyBacktesterMultiChartAccordion = 
            new CandleStrategyBacktesterMultiChartAccordion(
                gfx, 
                getSettingsLeft(0), getSettingsTop(2), 
                multiChartAccordionWidth, 
                multiChartAccordionFramesHeight,
                startTime, endTime,
                candleHistory, testExchange, candleStrategy, Config::symbol
            );
        mainFrame->child(candleStrategyBacktesterMultiChartAccordion);
    }

    void destroyCandleStrategyBacktesterMultiChartAccordion() {
        mainFrame->remove(candleStrategyBacktesterMultiChartAccordion);
        delete candleStrategyBacktesterMultiChartAccordion;
    }

    void loadHistoryModule() {
        const string period = periodSelect->getInput()->getText();
        const string symbol = symbolSelect->getInput()->getText();
        const ms_t start = datetime_to_ms(historyDateRange->getFromInput()->getText());
        const ms_t end = datetime_to_ms(historyDateRange->getToInput()->getText());

        string moduleName = historySelect->getInput()->getText();

        candleHistory = candleHistoryFactory.updateInstance(
            candleHistory,
            Config::candleHistoryPath + "/" + moduleName + "/" + moduleName + ".so",
            symbol, start, end, period_to_ms(period) 
        );
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
        Progress progress("Loading history...", false);
        candleHistory->load(progress);
    }

    void loadExchangeModule() {
        // Load the selected exchange lib
        string moduleName = exchangeSelect->getInput()->getText();
        
        testExchange = testExchangeFactory.updateInstance(
            testExchange,
            Config::testExchangePath + "/" + moduleName + "/" + moduleName + ".so", 
            // TODO: args from user with a settings form...??
            Config::periods, Config::symbols, Config::pairs, Config::balances
        );
    }

    void loadStrategyModule() {
        // load the selected strategy lib
        string moduleName = candleStrategySelect->getInput()->getText();
        
        candleStrategy = candleStrategyFactory.updateInstance(
            candleStrategy, 
            Config::candleStrategyPath + "/" + moduleName + "/" + moduleName + ".so", 
            // TODO: args from user with a settings form...??
            *testExchange, strategyParameters 
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
        candleStrategyBacktesterMultiChartAccordion->getCandleHistoryChart()->generateFromHistory();
        candleStrategyBacktesterMultiChartAccordion->getCandleHistoryChart()->fitTimeRangeToHistory();
        candleStrategyBacktesterMultiChartAccordion->draw();
    }

public:

    using FrameApplication::FrameApplication;

    virtual ~BitstampHistoryApplication() {
        delete historySelect;
        delete historySetupButton;
        delete historyDateRange;
        delete exchangeSelect;
        delete periodSelect;
        delete symbolSelect;
        delete candleStrategySelect;
        delete reloadButton;
        delete startButton;
        delete candleStrategyBacktesterMultiChartAccordion;
    }

    virtual void init() override {
        FrameApplication::init();
        gui->setTitle("Bitstamp History Backtest");
        app = this;

        createExchangeSelect(); // TODO: test each form element and fix them
        createStrategySelect();
        createPeriodSelect();
        createSymbolSelect();
        createHistoryDateRange();
        createHistorySelect();
        createHistorySetupButton();
        createReloadButton();
        createStartButton();
        createCandleStrategyBacktesterMultiChartAccordion();
    }
};
BitstampHistoryApplication* BitstampHistoryApplication::app = nullptr;

int backtest(int, const char* []) {
    // TODO: pass arguments
    BitstampHistoryApplication app; // = new BitstampHistoryApplication();
    app.run();
    // delete app;
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
    LOGI("Main thread started.");
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
