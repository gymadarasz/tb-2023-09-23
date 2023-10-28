#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

// class CandleStrategy {
// public:
//     virtual void onCandleClose(const Candle&) {
//         throw ERR_UNIMP;
//     }
// };

// class ACandleStrategy: public CandleStrategy {
// public:
//     void onCandleClose(const Candle& candle) override {
//         cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
//     }
// };

class MainApplication: public Application {
public:
    void init() override {
        logger.writeln("TODO...");
    }
};

int main()
{
    MainApplication app;
    app.run();

    return 0;
}
