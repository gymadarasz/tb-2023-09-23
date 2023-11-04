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
    // const int multiChartAccordionLeft = 10;
    // const int multiChartAccordionTop = 50;
    // const int multiChartAccordionWidth = 1000;
    // MultiChartAccordion multiChartAccordion = MultiChartAccordion(
    //     gfx, multiChartAccordionLeft, multiChartAccordionTop, multiChartAccordionWidth);

    static void draw(void* context) {
        BitstampHistoryApplication* that =
            (BitstampHistoryApplication*)((Area*)context)
                ->getRoot()->getGFX().getContext();

        that->chart.draw();
        // that->closeBtn.draw();
    }

    Frame frame = Frame(gfx, 10, 10, 1580, 880, BUTTON_PUSHED, black);
    Chart chart = Chart(frame);
    
public:
    void init() override {
        FrameApplication::init();
        gui.setTitle("Binance History");

        mainFrame.child(frame);
        frame.onDrawHandlers.push_back(draw);

        // // Define parameters and desired time range
        // const string symbol = "MONTECARLO";
        // const double volumeMean = 100;  // Initial volume
        // const double volumeStdDeviation = 10;
        // const double priceMean = 100;  // Initial price
        // const double priceStdDeviation = 10;
        // const double timeLambda = MS_PER_MIN;
        // const ms_t startTime = datetime_to_ms("2021-01-01");
        // const ms_t endTime = datetime_to_ms("2021-01-07");
        // const ms_t period = period_to_ms("1h");
        // const unsigned int seed = 6000;

        // // Create a MonteCarloHistory object with the specified parameters
        // MonteCarloHistory history(
        //     symbol, 
        //     startTime, endTime, period,
        //     volumeMean, volumeStdDeviation,
        //     priceMean, priceStdDeviation,
        //     timeLambda, seed
        // );

        const string symbol = "BTCUSD";
        const ms_t startTime = datetime_to_ms("2012-12-30");
        const ms_t endTime = datetime_to_ms("2023-11-01");
        const ms_t period = period_to_ms("1m");
        BitstampHistory history(symbol, startTime, endTime, period);

        Zoom zoom;
        Chart::CandleStyle candleStyle;
        TradeHistoryChartPlugin candlesPlugin(history, zoom, candleStyle);
        candlesPlugin.project(chart, &history);
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

    //static BitstampHistoryApplication* app;

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
