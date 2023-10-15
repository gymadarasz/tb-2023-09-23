#pragma once

#include <vector>
#include <string>
#include <../Vector.hpp>

using namespace std;
using namespace madlib;

namespace madlib::trading {

    class Candle {
    protected:
        double open, close, low, high, volume;
        time_t start, end;
    public:
        Candle(double open, double close, double low, double high, double volume, time_t start, time_t end):
            open(open), close(close), low(low), high(high), volume(volume), start(start), end(end) {}

        double getOpen() const {
            return open;
        }

        void setOpen(double open) {
            this->open = open;
        }

        double getClose() const {
            return close;
        }

        void setClose(double close) {
            this->close = close;
        }

        double getLow() const {
            return low;
        }

        void setLow(double low) {
            this->low = low;
        }

        double getHigh() const {
            return high;
        }

        void setHigh(double high) {
            this->high = high;
        }

        double getVolume() const {
            return volume;
        }

        void setVolume(double volume) {
            this->volume = volume;
        }

        time_t getStart() const {
            return start;
        }

        void setStart(time_t start) {
            this->start = start;
        }

        time_t getEnd() const {
            return end;
        }

        void setEnd(time_t end) {
            this->end = end;
        }
    };

    class History {
    public:
        virtual vector<Candle> fetch(const string symbol, const string interval, const time_t from, const time_t to) const;
        
        void save(const string filename, vector<Candle> candles) const {
            Vector::save<Candle>(filename, candles);
        }
        
        vector<Candle> load(const string filename) const {
            return Vector::load<Candle>(filename);
        }
        
        vector<Candle>& load(const string filename, vector<Candle>& data) const {
            return Vector::load<Candle>(filename, data);
        }
    };

}
