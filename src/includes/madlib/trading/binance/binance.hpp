#pragma once

#include <vector>
#include "../trading.hpp"

namespace madlib::trading::binance  {

    class BinanceHistory: public History {
    public:
        virtual vector<Candle> fetch(const string symbol, const string interval, const time_t from, const time_t to) const {
            
        }
            
    };

}
