#pragma once

#include "CandleHistory.hpp"

namespace madlib::trading {
    
    class TradeCandleHistory: public CandleHistory {
    protected:
        vector<Trade> trades;

        // TODO: yagni??
        void convertToCandles() {

            auto tradeEventIter = trades.begin();
            
            ms_t currentStart = startTime;
            ms_t currentEnd = currentStart + period;

            candles.clear();
            while (tradeEventIter != trades.end()) {
                double open = tradeEventIter->price;
                double close = open;
                double low = open;
                double high = open;
                double volume = 0.0;
                
                while (tradeEventIter != trades.end() && tradeEventIter->timestamp >= currentStart && tradeEventIter->timestamp < currentEnd) {
                    close = tradeEventIter->price;
                    low = low < close ? low : close;
                    high = high > close ? high : close;
                    volume += tradeEventIter->volume;
                    ++tradeEventIter;
                }
                
                candles.push_back(Candle(open, close, low, high, volume, currentStart, currentEnd));
                
                if (currentStart >= endTime) {
                    break;  // Exit the loop if we've reached or passed the end time
                }

                currentStart += period;
                currentEnd += period;
            }
        }

    public:

        using CandleHistory::CandleHistory;

        virtual ~TradeCandleHistory() {}

        virtual vector<Trade> getTrades() const override {
            return trades;
        }
    };

}