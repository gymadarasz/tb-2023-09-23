#pragma once

#include "Candle.hpp"
#include "Strategy.hpp"

namespace madlib::trading {
    
    class CandleStrategy: public Strategy {
    public:
        
        using Strategy::Strategy;

        virtual ~CandleStrategy() {}
    };


}