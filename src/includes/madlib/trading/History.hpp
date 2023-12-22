#pragma once

#include "../Progress.hpp"
#include "../graph/SettingsHolder.hpp"

using namespace madlib::graph;

namespace madlib::trading {
    
    class History: public SettingsHolder {
    protected:

        string symbol;
        ms_t startTime;
        ms_t endTime;
        ms_t period;

    public:
    
        History(
            const string& symbol,
            const ms_t startTime,
            const ms_t endTime,
            const ms_t period
        ): 
            SettingsHolder(),
            symbol(symbol),
            startTime(startTime),
            endTime(endTime),
            period(period)
        {}

        virtual ~History() {}

        void setSymbol(const string& symbol) {
            this->symbol = symbol;
        }

        ms_t getStartTime() const {
            return startTime;
        }

        void setStartTime(ms_t startTime) {
            this->startTime = startTime;
        }

        ms_t getEndTime() const {
            return endTime;
        }

        void setEndTime(ms_t endTime) {
            this->endTime = endTime;
        }

        void setPeriod(ms_t period) {
            this->period = period;
        }

        virtual void load(Progress&) {
            throw ERR_UNIMP;
        }

        virtual void reload(Progress&) {
            throw ERR_UNIMP;
        }

        // virtual void clear() {
        //     throw ERR_UNIMP;
        // }
    };

}