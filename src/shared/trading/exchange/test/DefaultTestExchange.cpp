#include "../../../../includes/madlib/trading/trading.hpp"

namespace madlib::trading::exchange::test {

    class DefaultTestExchange: public TestExchange {
    public:
        using TestExchange::TestExchange;
        virtual ~DefaultTestExchange() final {};
    };

    EXPORT_CLASS(DefaultTestExchange)
}