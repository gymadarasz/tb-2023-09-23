#include "../../../../../includes/madlib/trading/trading.hpp"

namespace madlib::trading::exchange::test {

    class DefaultTestExchange: public TestExchange {
    public:
        using TestExchange::TestExchange;
        virtual ~DefaultTestExchange() {};
    };
    

    extern "C" DefaultTestExchange* createDefaultTestExchange(
            const vector<string>& periods,
            const vector<string>& symbols,
            const map<string, Pair>& pairs,
            const map<string, Balance>& balances
    ) {
        return new DefaultTestExchange(periods, symbols, pairs, balances);
    }
}