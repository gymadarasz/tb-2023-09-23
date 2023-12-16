#pragma once

#include "Candle.hpp"
#include "Strategy.hpp"

namespace madlib::trading {
    
    class CandleStrategy: public Strategy {
    public:
        
        using Strategy::Strategy;

        virtual ~CandleStrategy() {}

        virtual void onCandleClose(Exchange&, const string&, const Candle&) {
            throw ERR_UNIMP;
        }
    };


}