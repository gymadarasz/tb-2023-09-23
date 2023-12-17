#pragma once

#include <limits.h>

#include "../Log.hpp"
#include "../graph/Chart.hpp"

#include "Exchange.hpp"

namespace madlib::trading {
  
    class Strategy {
    public:

        template<typename T>
        class Limited {
        protected:
            T value, min, max;
        public:
            Limited(T value = 0, T min = 0, T max = 0): min(min), max(max) {
                setValue(value);
            }

            T getValue() const {
                return value;
            }

            void setValue(T value) {
                if (min > value || max < value) 
                    throw ERROR("Out of range limits: " + to_string(value) + 
                        ", min: " + to_string(min) + ", max: " + to_string(max)); 
                this->value = value;
            }

            T getMin() const {
                return min;
            }

            void setMin(T min) {
                this->min = min;
            }

            T getMax() const {
                return max;
            }

            void setMax(T max) {
                this->max = max;
            }
        };
        
        class Parameter {
        public:
            enum Type {STRING, DOUBLE, LONG};
        protected:
            const Type type;
            const string _string = "";
            Limited<double> _double = Limited<double>(0);
            Limited<long> _long = Limited<long>(0);
        public:
            explicit Parameter(const string& _string): 
                type(STRING), _string(_string) {}

            explicit Parameter(double value, double min = -INFINITY, double max = INFINITY): 
                type(DOUBLE), _double(Limited<double>(value, min, max)) {}

            explicit Parameter(long value, long min = LONG_MIN, long max = LONG_MAX):
                type(LONG), _long(Limited<long>(value, min, max)) {}


            virtual ~Parameter() {}

            Type getType() const {
                return type;
            }

            const string& getString() const {
                if (type != STRING) throw ERR_INVALID;
                return _string;
            }

            const Limited<double>& getDouble() const {
                if (type != DOUBLE) throw ERR_INVALID;
                return _double;
            }

            const Limited<long>& getLong() const {
                if (type != LONG) throw ERR_INVALID;
                return _long;
            }
        };

    protected:

        // map<string, Parameter>& parameters;
        LabelSeries* labelSeries = nullptr;

    public:
        Strategy(
            // map<string, Parameter>& parameters,
            // LabelSeries* labelSeries = nullptr
        )//:
            // parameters(parameters),
            // labelSeries(labelSeries)
        {}
        
        virtual ~Strategy() {}

        // LabelSeries* getLabelSeries() const {
        //     return labelSeries;
        // }

        virtual void onCandleClose(Exchange*&, const string&, const Candle&) {
            throw ERR_UNIMP;
        }

        void setLabelSeries(LabelSeries* labelSeries) {
            this->labelSeries = labelSeries;
        }

        void addBuyText(Exchange*& exchange, const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "BUY", Color color = Theme::defaultTradeLabelBuyColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange->getCurrentTime();
            if (!currentPrice) currentPrice = exchange->getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getBuyTextRealChoords(), labelSeries.getBuyTexts(), currentTime, currentPrice, text);
        }

        void addSellText(Exchange*& exchange, const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "SELL", Color color = Theme::defaultTradeLabelSellColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange->getCurrentTime();
            if (!currentPrice) currentPrice = exchange->getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getSellTextRealChoords(), labelSeries.getSellTexts(), currentTime, currentPrice, text);
        }

        void addErrorText(Exchange*& exchange, const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "ERROR", Color color = Theme::defaultTradeLabelErrorColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange->getCurrentTime();
            if (!currentPrice) currentPrice = exchange->getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getErrorTextRealChoords(), labelSeries.getErrorTexts(), currentTime, currentPrice, text);
        }

        bool marketBuy(Exchange*& exchange, const string& symbol, double amount) {
            ms_t currentTime = exchange->getCurrentTime();
            double currentPrice = exchange->getPairAt(symbol).getPrice();
            if (exchange->marketBuy(symbol, amount, false)) {
                addBuyText(exchange, symbol, currentTime, currentPrice);
                return true;
            }
            LOG(
                " Exchange time: " + ms_to_datetime(currentTime) 
                + ", Strategy BUY Error, [" + symbol + "] " + to_string(amount)
            );
            addErrorText(exchange, symbol, currentTime, currentPrice);
            return false;
        }

        bool marketSell(Exchange*& exchange, const string& symbol, double amount) {
            ms_t currentTime = exchange->getCurrentTime();
            double currentPrice = exchange->getPairAt(symbol).getPrice();
            if (exchange->marketSell(symbol, amount, false)) {
                addSellText(exchange, symbol, currentTime, currentPrice);
                return true;
            }
            LOG(
                " Exchange time: " + ms_to_datetime(currentTime) 
                + ", Strategy SELL Error, [" + symbol + "] " + to_string(amount)
            );
            addErrorText(exchange, symbol, currentTime, currentPrice);
            return false;
        }
    };
  
}