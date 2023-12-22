#pragma once

#include "Candle.hpp"
#include "Trade.hpp"
#include "History.hpp"

namespace madlib::trading {
    
    class CandleHistory: public History {
    protected:
        vector<Candle> candles;

    public:
        using History::History; // TODO: bug: some history having overlaping period that may runs multiple times in tests

        virtual ~CandleHistory() {}

        virtual const vector<Trade>& getTrades() const {
            throw ERR_UNIMP;
        }

        virtual const vector<Candle>& getCandles() const {
            return candles;
        }
        
        // TODO: yagni?
        // virtual void saveCandles(const string &filename, const vector<Candle>& candles) const {
        //     vector_save<Candle>(filename, candles);
        // }
        
        // virtual vector<Candle> loadCandles(const string &filename) const {
        //     return vector_load<Candle>(filename);
        // }
        
        // virtual vector<Candle>& loadCandles(const string &filename, vector<Candle>& data) const {
        //     return vector_load<Candle>(filename, data);
        // }
    };

}