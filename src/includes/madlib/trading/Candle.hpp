#pragma once

#include <cmath>

#include "../../../../libs/clib/clib/time.hpp"
#include "../ERROR.hpp"

using namespace std;
using namespace clib;

namespace madlib::trading {
    
    class Candle {
    protected:
        double open, close, low, high, volume;
        ms_t start, end; // ms
        bool strict;

        void validate() {
            if (open < 0 || close < 0 || low < 0 || high < 0) throw ERROR("Candle prices cannot be negative.");
            if (isnan(open) || isnan(close) || isnan(low) || isnan(high) || isnan(volume)) throw ERROR("Invalid NaN value(s).");
            if (end < start) throw ERROR("Candle start time cannot be greater than end time.");
            if (volume < 0) throw ERROR("Candle volume cannot be negative.");
            if (low > open || low > close) throw ERROR("Candle low should be the lowest.");
            if (high < open || high < close) throw ERROR("Candle high should be the highest.");
        }

    public:
        Candle(
            double open = 0, double close = 0, double low = 0, double high = 0, double volume = 0, 
            ms_t start = 0, ms_t end = 0, bool strict = false
        ):
            open(open), close(close), low(low), high(high), volume(volume), 
            start(start), end(end), strict(strict)
        {
            if (strict) validate();
        }

        double getOpen() const {
            return open;
        }

        void setOpen(double open) {
            this->open = open;
            if (strict) validate();
        }

        double getClose() const {
            return close;
        }

        void setClose(double close) {
            this->close = close;
            if (strict) validate();
        }

        double getLow() const {
            return low;
        }

        void setLow(double low) {
            this->low = low;
            if (strict) validate();
        }

        double getHigh() const {
            return high;
        }

        void setHigh(double high) {
            this->high = high;
            if (strict) validate();
        }

        double getVolume() const {
            return volume;
        }

        void setVolume(double volume) {
            this->volume = volume;
            if (strict) validate();
        }

        ms_t getStart() const {
            return start;
        }

        void setStart(ms_t start) {
            this->start = start;
            if (strict) validate();
        }

        ms_t getEnd() const {
            return end;
        }

        void setEnd(ms_t end) {
            this->end = end;
            if (strict) validate();
        }
    };

}