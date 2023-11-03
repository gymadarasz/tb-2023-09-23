#include <iostream>
#include <cstdio>

#include "includes/madlib/madlib.hpp"
#include "includes/madlib/trading/trading.hpp"
#include "includes/madlib/trading/bitstamp/bitstamp.hpp"

using namespace std;
using namespace madlib::trading;
using namespace madlib::trading::bitstamp;

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
};

int main(int argc, char* argv[])
{
    return (TradingApplication(argc, argv)).getResult();
}
