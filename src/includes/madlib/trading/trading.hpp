#pragma once

#include <vector>
#include <string>

#include "../Vector.hpp"
#include "../graph/Chart.hpp"

using namespace std;
using namespace chrono;
using namespace madlib;
using namespace madlib::graph;

namespace madlib::trading {

    ms_t period_to_ms(const string &period) {
        map<const string, ms_t> periods = {
            {"1s", MS_PER_SEC},
            {"1m", MS_PER_MIN},
            {"5m", 5 * MS_PER_MIN},
            {"10m", 10 * MS_PER_MIN},
            {"30m", 30 * MS_PER_MIN},
            {"1h", MS_PER_HOUR},
            {"4h", 4 * MS_PER_HOUR},
            {"6h", 6 * MS_PER_HOUR},
            {"12h", 12 * MS_PER_HOUR},
            {"1d", MS_PER_DAY},
            {"3d", 3 * MS_PER_DAY},
            {"4d", 4 * MS_PER_DAY},
            {"1w", MS_PER_WEEK},
            {"2w", 2 * MS_PER_WEEK},
            {"4w", 4 * MS_PER_WEEK},
        };
        return periods[period];
    }

    class Candle {
    protected:
        double open, close, low, high, volume;
        ms_t start, end; // ms
    public:
        Candle(
            double open = 0, double close = 0, double low = 0, double high = 0, double volume = 0, 
            ms_t start = 0, ms_t end = 0
        ):
            open(open), close(close), low(low), high(high), volume(volume), 
            start(start), end(end) {}

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

        ms_t getStart() const {
            return start;
        }

        void setStart(ms_t start) {
            this->start = start;
        }

        ms_t getEnd() const {
            return end;
        }

        void setEnd(ms_t end) {
            this->end = end;
        }
    };

    // Define a struct for trade event data
    struct Trade {
        double volume;
        double price;
        ms_t timestamp; // ms
    };

    class TradeHistory {
    protected:

        const string& symbol;
        const ms_t startTime; // ms
        const ms_t endTime; // ms
        const ms_t period;
        vector<Trade> trades;
        vector<Candle> candles;

        virtual void initTrades() {
            throw ERR_UNIMP;
        };

        virtual void initCandles() {
            auto tradeEventIter = trades.begin();
            
            ms_t currentStart = startTime;
            ms_t currentEnd = currentStart + period;
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

        void init() {
            initTrades();
            initCandles();
        }

    public:

        TradeHistory(
            const string& symbol, 
            const ms_t startTime, 
            const ms_t endTime, 
            const ms_t period
        ): 
            symbol(symbol), 
            startTime(startTime), 
            endTime(endTime), 
            period(period)
        {}

        virtual vector<Trade> getTrades() const {
            return trades;
        }

        virtual vector<Candle> getCandles() const {
            return candles;
        }
        
        virtual void saveCandles(const string &filename, const vector<Candle>& candles) const {
            Vector::save<Candle>(filename, candles);
        }
        
        virtual vector<Candle> loadCandles(const string &filename) const {
            return Vector::load<Candle>(filename);
        }
        
        virtual vector<Candle>& loadCandles(const string &filename, vector<Candle>& data) const {
            return Vector::load<Candle>(filename, data);
        }
    };


    class MonteCarloHistory: public TradeHistory {
    protected:
        double volumeMean;
        double volumeStdDeviation;
        double priceMean;
        double priceStdDeviation;
        double timeLambda;
        mt19937 gen;

        // Function to init events within a specified time range
        void initTrades() override {
            ms_t previousTime = startTime;
            double previousPrice = priceMean;
            double previousVolume = volumeMean;

            while (previousTime < endTime) {
                Trade trade;


                // Generate elapsed time using exponential distribution
                exponential_distribution<double> timeDistribution(1.0 / timeLambda);
                ms_t elapsed_time = ((ms_t)timeDistribution(gen)) + 1;

                // Accumulate elapsed time to calculate the timestamp
                previousTime += elapsed_time;

                // Generate price movement
                normal_distribution<double> priceDistribution(0, priceStdDeviation);
                double priceMovement = priceDistribution(gen);

                // Calculate the new price based on the previous price and movement
                trade.price = previousPrice + priceMovement;

                // Generate volume movement
                normal_distribution<double> volumeDistribution(0, volumeStdDeviation);
                double volumeMovement = volumeDistribution(gen);

                // Calculate the new volume based on the previous volume and movement
                trade.volume = previousVolume + volumeMovement;

                if (previousTime >= endTime) {
                    break;  // Stop generating events if we've reached or passed the end time
                }

                trade.timestamp = previousTime;
                trades.push_back(trade);

                previousPrice = trade.price;  // Update the previous_price
                previousVolume = trade.volume;  // Update the previous_volume
            }
        }

    public:
        MonteCarloHistory(
            const string& symbol, ms_t startTime, ms_t endTime, ms_t period,  
            double volumeMean, double volumeStdDeviation, 
            double priceMean, double priceStdDeviation,
            double timeLambda,
            unsigned int seed = random_device()() // Add a seed parameter with a default value
        ):
            TradeHistory(symbol, startTime, endTime, period),
            volumeMean(volumeMean), volumeStdDeviation(volumeStdDeviation),
            priceMean(priceMean), priceStdDeviation(priceStdDeviation),
            timeLambda(timeLambda),
            gen(seed)
        {
            init();
        }
        

        // Function to print the generated events
        void PrintEvents() {
            for (const Trade& trade : trades) {
                cout << "Event: Volume=" << trade.volume 
                    << ", Price=" << trade.price 
                    << ", Timestamp=" << trade.timestamp;
            }
        }
    };

    class TradeHistoryChartPlugin: public ChartPlugin {
    protected:
        const TradeHistory& history;
        const Zoom& zoom;
        const Chart::CandleStyle& candleStyle;
        const bool showCandles = true, showPrices = true, showVolumes = true; // TODO
        const Color& priceColor = orange; // TODO
        const Color& volumeColor = darkGray; // TODO
    public:

        TradeHistoryChartPlugin(
            const TradeHistory& history,
            const Zoom& zoom,
            const Chart::CandleStyle& candleStyle,
            const bool showCandles = true, // TODO
            const bool showPrices = true, // TODO
            const bool showVolumes = true, // TODO
            const Color& priceColor = orange, // TODO
            const Color& volumeColor = darkGray // TODO
        ): 
            history(history),
            zoom(zoom),
            candleStyle(candleStyle),
            showCandles(showCandles),
            showPrices(showPrices),
            showVolumes(showVolumes),
            priceColor(priceColor),
            volumeColor(volumeColor)
        {}

        void project(Chart& chart, const void* = NULL) const override {

            Chart::Scale* candlesScale = NULL;
            if (showCandles) {
                vector<Candle> candles = history.getCandles();
                vector<RealPoint> candlesRealPoints;
                for (const Candle& candle: candles) {
                    double start = static_cast<double>(candle.getStart());
                    double end = static_cast<double>(candle.getEnd());
                    double open = candle.getOpen();
                    double close = candle.getClose();
                    double low = candle.getLow();
                    double high = candle.getHigh();
                    double middle = start + (end - start) / 2;
                    candlesRealPoints.push_back(RealPoint(start, open));
                    candlesRealPoints.push_back(RealPoint(end, close));
                    candlesRealPoints.push_back(RealPoint(middle, low));
                    candlesRealPoints.push_back(RealPoint(middle, high));
                }
                candlesScale = &chart.addScale(CANDLE, &candleStyle, &zoom);
                candlesScale->project(candlesRealPoints);
            }

            if (showPrices || showVolumes) {
                vector<Trade> trades = history.getTrades();
                vector<RealPoint> pricesRealPoints;
                vector<RealPoint> volumesRealPoints;
                for (const Trade& trade: trades) {
                    pricesRealPoints.push_back(
                        RealPoint(static_cast<double>(trade.timestamp), trade.price));
                    volumesRealPoints.push_back(
                        RealPoint(static_cast<double>(trade.timestamp), trade.volume));
                }
                
                if (showPrices) {
                    Chart::Scale& priceScale = chart.addScale(LINE, &priceColor, &zoom);
                    priceScale.project(pricesRealPoints);
                    if (candlesScale) Chart::Scale::alignXY(priceScale, *candlesScale);
                }
                if (showVolumes) 
                    chart.addScale(LINE, &volumeColor, &zoom).project(volumesRealPoints);
            }
        }
    };


    class Fees {
    protected:

        const double marketBuyPc = 0.0;
        const double marketSellPc = 0.0;
        const double limitBuyPc = 0.0;
        const double limitSellPc = 0.0;

    public:

        Fees(
            const double marketBuyPc,
            const double marketSellPc,
            const double limitBuyPc,
            const double limitSellPc
        ):
            marketBuyPc(marketBuyPc),
            marketSellPc(marketSellPc),
            limitBuyPc(limitBuyPc),
            limitSellPc(limitSellPc)
        {}

        double getMarketBuyPc() const {
            return marketBuyPc;
        }

        double getMarketSellPc() const {
            return marketSellPc;
        }

        double getLimitBuyPc() const {
            return limitBuyPc;
        }

        double getLimitSellPc() const {
            return limitSellPc;
        }

    };

    class Pair {
    public:

        const string& baseCurrency;
        const string& quotedCurrency;
        const Fees& fees;

    protected:
        double price;

    public:
        Pair(
            const string& baseCurrency, const string& quotedCurrency, 
            const Fees& fees, double price): 
            baseCurrency(baseCurrency), quotedCurrency(quotedCurrency), 
            fees(fees), price(price)
        {}

        const string& getBaseCurrency() const {
            return baseCurrency;
        }

        const string& getQuotedCurrency() const {
            return quotedCurrency;
        }

        const Fees& getFees() const {
            return fees;
        }

        double getPrice() const {
            return price;
        }

        void setPrice(double price) {
            this->price = price;
        }
    };


    class Balance {
    protected:
        double amount = 0;
        bool canGoNegative = false;

        bool checkIfNegative(bool throws = true) const {
            bool error = !canGoNegative && amount < 0.0;
            if(error && throws) 
                throw ERROR(
                    "Balance can not go negative: " + to_string(amount)
                );
            return error;
        }

    public:
        Balance(double amount = 0, bool canGoNegative = false): 
            amount(amount), canGoNegative(canGoNegative)
            {}
        
        void setAmount(double amount) {
            this->amount = amount;
        }

        double getAmount() const {
            return amount;
        }

        bool increment(double increment, bool throws = true) {
            amount += increment;
            if (checkIfNegative(throws)) {
                amount -= increment;
                return false;
            }
            return true;
        }

        bool decrement(double decrement, bool throws = true) {
            amount -= decrement;
            if (checkIfNegative(throws)) {
                amount += decrement;
                return false;
            }
            return true;
        }
    };

    class Exchange {
    protected:
        
        map<string, Pair> pairs;
        map<string, Balance> balances;

    public:

        Exchange() {}

        map<string, Pair>& getPairs() {
            return pairs;
        }

        const map<string, Balance>& getBalances() const {
            return balances;
        }

        double getBalanceBase(const string& symbol) {
            return getBalances().at(getPairs().at(symbol).getBaseCurrency()).getAmount();
        }

        double getBalanceBase(const Pair& pair) {
            return getBalances().at(pair.getBaseCurrency()).getAmount();
        }

        double getBalanceQuoted(const string& symbol) {
            return getBalances().at(getPairs().at(symbol).getQuotedCurrency()).getAmount();
        }

        double getBalanceQuoted(const Pair& pair) {
            return getBalances().at(pair.getQuotedCurrency()).getAmount();
        }

        double getBalanceQuotedFull(const string& symbol) {
            const Pair& pair = getPairs().at(symbol);
            return getBalanceQuoted(symbol) + getBalanceBase(symbol) * pair.getPrice();
        }

        double getBalanceQuotedFull(const Pair& pair) {
            return getBalanceQuoted(pair) + getBalanceBase(pair) * pair.getPrice();
        }

        double getBalanceBaseFull(const string& symbol) {
            const Pair& pair = getPairs().at(symbol);
            return getBalanceQuoted(symbol) / pair.getPrice() + getBalanceBase(symbol);
        }

        double getBalanceBaseFull(const Pair& pair) {
            return getBalanceQuoted(pair) / pair.getPrice() + getBalanceBase(pair);
        }

        virtual ms_t getCurrentTime() const {
            throw ERR_UNIMP;
        }

        virtual bool marketBuy(const string& /*symbol*/, double /*amount*/, bool = true) {
            throw ERR_UNIMP;
        }

        virtual bool marketSell(const string& /*symbol*/, double /*amount*/, bool = true) {
            throw ERR_UNIMP;
        }

        virtual void limitBuy(const string& /*symbol*/, double /*amount*/, double /*limitPrice*/) {
            throw ERR_UNIMP;
        }

        virtual void limitSell(const string& /*symbol*/, double /*amount*/, double /*limitPrice*/) {
            throw ERR_UNIMP;
        }
    };

    class TestExchange: public Exchange {
    protected:

        ms_t currentTime;
        double currentPrice;

        struct MarketOrderInfos {
            const double price;
            const Fees& fees;
            Balance& baseBalance;
            Balance& quotedBalance;
        };

        MarketOrderInfos getMarketOrderInfos(const string& symbol) {
            const Pair& pair = pairs.at(symbol);
            const Fees& fees = pair.getFees();
            const double price = pair.getPrice();
            return { 
                price, 
                fees, 
                balances.at(pair.getBaseCurrency()), 
                balances.at(pair.getQuotedCurrency()) 
            };
        }

    public:
        TestExchange(
            const map<string, Pair>& pairs,
            const map<string, Balance>& balances
        ): Exchange() {
            this->pairs = pairs;
            this->balances = balances;
        }

        void setCurrentTime(ms_t currentTime) {
            this->currentTime = currentTime;
        }

        ms_t getCurrentTime() const override {
            return currentTime;
        }

        virtual bool marketBuy(const string& symbol, double amount, bool throws = true) override {
            MarketOrderInfos marketOrderInfos = getMarketOrderInfos(symbol);
            double cost = amount * marketOrderInfos.price;
            double fee = amount * marketOrderInfos.fees.getMarketBuyPc();
            return marketOrderInfos.quotedBalance.decrement(cost, throws)
                && marketOrderInfos.baseBalance.increment(amount - fee, throws);
        }

        virtual bool marketSell(const string& symbol, double amount, bool throws = true) override {
            MarketOrderInfos marketOrderInfos = getMarketOrderInfos(symbol);
            double cost = amount * marketOrderInfos.price;
            double fee = cost * marketOrderInfos.fees.getMarketSellPc();
            return marketOrderInfos.baseBalance.decrement(amount, throws)
                && marketOrderInfos.quotedBalance.increment(cost - fee, throws);
        }

        // TODO:
        // virtual void limitBuy(const string& symbol, double amount, double limitPrice) {
        //     throw ERR_UNIMP;
        // }

        // TODO:
        // virtual void limitSell(const string& symbol, double amount, double limitPrice) {
        //     throw ERR_UNIMP;
        // }
    };

    class Strategy {
    protected:
        Exchange& exchange;
        vector<RealPoint> buyTextRealChoords;
        vector<string> buyTexts;
        vector<RealPoint> sellTextRealChoords;
        vector<string> sellTexts;
        vector<RealPoint> errorTextRealChoords;
        vector<string> errorTexts;
    public:
        Strategy(Exchange& exchange): exchange(exchange) {}

        const vector<RealPoint>& getBuyTextRealChoords() const {
            return buyTextRealChoords;
        }

        const vector<string>& getBuyTexts() const {
            return buyTexts;
        }

        const vector<RealPoint>& getSellTextRealChoords() const {
            return sellTextRealChoords;
        }

        const vector<string>& getSellTexts() const {
            return sellTexts;
        }

        const vector<RealPoint>& getErrorTextRealChoords() const {
            return errorTextRealChoords;
        }

        const vector<string>& getErrorTexts() const {
            return errorTexts;
        }

        void addText(vector<RealPoint>& textRealChoords, vector<string>& texts, ms_t currentTime, double currentPrice, const string& text) {
            textRealChoords.push_back(RealPoint((double)currentTime, currentPrice));
            texts.push_back(text);
        }

        void addBuyText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "BUY") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairs().at(symbol).getPrice();
            addText(buyTextRealChoords, buyTexts, currentTime, currentPrice, text);
        }

        void addSellText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "SELL") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairs().at(symbol).getPrice();
            addText(sellTextRealChoords, sellTexts, currentTime, currentPrice, text);
        }

        void addErrorText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "ERROR") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairs().at(symbol).getPrice();
            addText(errorTextRealChoords, errorTexts, currentTime, currentPrice, text);
        }

        bool marketBuy(const string& symbol, double amount) {
            ms_t currentTime = exchange.getCurrentTime();
            double currentPrice = exchange.getPairs().at(symbol).getPrice();
            if (exchange.marketBuy(symbol, amount, false)) {
                addBuyText(symbol, currentTime, currentPrice);
                return true;
            }
            logger.date().writeln(
                " Exchange time: " + ms_to_datetime(currentTime) 
                + ", Strategy BUY Error, [" + symbol + "] " + to_string(amount)
            );
            addErrorText(symbol, currentTime, currentPrice);
            return false;
        }

        bool marketSell(const string& symbol, double amount) {
            ms_t currentTime = exchange.getCurrentTime();
            double currentPrice = exchange.getPairs().at(symbol).getPrice();
            if (exchange.marketSell(symbol, amount, false)) {
                addSellText(symbol, currentTime, currentPrice);
                return true;
            }
            logger.date().writeln(
                " Exchange time: " + ms_to_datetime(currentTime) 
                + ", Strategy SELL Error, [" + symbol + "] " + to_string(amount)
            );
            addErrorText(symbol, currentTime, currentPrice);
            return false;
        }
    };


    class CandleStrategy: public Strategy {
    public:
        using Strategy::Strategy;

        virtual void onCandleClose(const Candle&) {
            throw ERR_UNIMP;
        }
    };
}
