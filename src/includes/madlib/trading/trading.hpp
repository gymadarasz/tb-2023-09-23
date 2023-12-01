#pragma once

#include <vector>
#include <string>
#include <climits>

#include "../madlib.hpp"
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

        virtual vector<Trade> getTrades() const final {
            return trades;
        }

        virtual vector<Candle> getCandles() const final {
            return candles;
        }
        
        virtual void saveCandles(const string &filename, const vector<Candle>& candles) const {
            vector_save<Candle>(filename, candles);
        }
        
        virtual vector<Candle> loadCandles(const string &filename) const {
            return vector_load<Candle>(filename);
        }
        
        virtual vector<Candle>& loadCandles(const string &filename, vector<Candle>& data) const {
            return vector_load<Candle>(filename, data);
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

                // Calculate the price based on the previous price and movement
                trade.price = previousPrice + priceMovement;

                // Generate volume movement
                normal_distribution<double> volumeDistribution(0, volumeStdDeviation);
                double volumeMovement = volumeDistribution(gen);

                // Calculate the volume based on the previous volume and movement
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
            const string& symbol, 
            ms_t startTime, ms_t endTime, ms_t period,  
            double volumeMean, double volumeStdDeviation, 
            double priceMean, double priceStdDeviation,
            double timeLambda,
            unsigned int seed = random_device()() // Add a seed parameter with a default value
        ):
            TradeHistory(
            symbol, 
            startTime, endTime, period),
            volumeMean(volumeMean), volumeStdDeviation(volumeStdDeviation),
            priceMean(priceMean), priceStdDeviation(priceStdDeviation),
            timeLambda(timeLambda),
            gen(seed)
        {
            init();
        }
        
        virtual ~MonteCarloHistory() {}

        // Function to print the generated events
        void PrintEvents() {
            for (const Trade& trade : trades) {
                cout << "Event: Volume=" << trade.volume 
                    << ", Price=" << trade.price 
                    << ", Timestamp=" << trade.timestamp;
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

        string baseCurrency;
        string quotedCurrency;
        const Fees& fees;

    protected:
        double price;

    public:
        Pair(
            const string& baseCurrency, const string& quotedCurrency, 
            const Fees& fees, double price = 0): 
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

        Pair& getPairAt(const string& symbol) {
            if (pairs.count(symbol) == 1) return pairs.at(symbol);
            throw ERROR("No symbol: " + symbol);
        }

        const map<string, Balance>& getBalances() const {
            return balances;
        }

        double getBalanceBase(const string& symbol) {
            return getBalances().at(getPairAt(symbol).getBaseCurrency()).getAmount();
        }

        double getBalanceBase(const Pair& pair) {
            return getBalances().at(pair.getBaseCurrency()).getAmount();
        }

        double getBalanceQuoted(const string& symbol) {
            return getBalances().at(getPairAt(symbol).getQuotedCurrency()).getAmount();
        }

        double getBalanceQuoted(const Pair& pair) {
            return getBalances().at(pair.getQuotedCurrency()).getAmount();
        }

        double getBalanceQuotedFull(const string& symbol) {
            const Pair& pair = getPairAt(symbol);
            return getBalanceQuoted(symbol) + getBalanceBase(symbol) * pair.getPrice();
        }

        double getBalanceQuotedFull(const Pair& pair) {
            return getBalanceQuoted(pair) + getBalanceBase(pair) * pair.getPrice();
        }

        double getBalanceBaseFull(const string& symbol) {
            const Pair& pair = getPairAt(symbol);
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


    class TradeTexts {
    protected:

        vector<RealPoint> buyTextRealChoords;
        vector<string> buyTexts;
        vector<RealPoint> sellTextRealChoords;
        vector<string> sellTexts;
        vector<RealPoint> errorTextRealChoords;
        vector<string> errorTexts;

    public:
        TradeTexts(
            const vector<RealPoint> buyTextRealChoords,
            const vector<string> buyTexts,
            const vector<RealPoint> sellTextRealChoords,
            const vector<string> sellTexts,
            const vector<RealPoint> errorTextRealChoords,
            const vector<string> errorTexts

        ):
            buyTextRealChoords(buyTextRealChoords),
            buyTexts(buyTexts),
            sellTextRealChoords(sellTextRealChoords),
            sellTexts(sellTexts),
            errorTextRealChoords(errorTextRealChoords),
            errorTexts(errorTexts) 
        {}
        TradeTexts():
            buyTextRealChoords({}),
            buyTexts({}),
            sellTextRealChoords({}),
            sellTexts({}),
            errorTextRealChoords({}),
            errorTexts({}) 
        {}

        vector<RealPoint>& getBuyTextRealChoords() {
            return buyTextRealChoords;
        }

        vector<string>& getBuyTexts() {
            return buyTexts;
        }

        vector<RealPoint>& getSellTextRealChoords() {
            return sellTextRealChoords;
        }

        vector<string>& getSellTexts() {
            return sellTexts;
        }

        vector<RealPoint>& getErrorTextRealChoords() {
            return errorTextRealChoords;
        }

        vector<string>& getErrorTexts() {
            return errorTexts;
        }
    };

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
            Parameter(const string& _string): 
                type(STRING), _string(_string) {}

            Parameter(double value, double min = -INFINITY, double max = INFINITY): 
                type(DOUBLE), _double(Limited<double>(value, min, max)) {}

            Parameter(long value, long min = LONG_MIN, long max = LONG_MAX):
                type(LONG), _long(Limited<long>(value, min, max)) {}

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

        void static addText(vector<RealPoint>& textRealChoords, vector<string>& texts, ms_t time, double price, const string& text) {
            textRealChoords.push_back(RealPoint((double)time, price));
            texts.push_back(text);
        }

        Exchange& exchange;
        map<string, Parameter> parameters;
        TradeTexts& tradeTexts;
    public:
        Strategy(
            Exchange& exchange,
            map<string, Parameter> parameters,
            TradeTexts& tradeTexts
        ):
            exchange(exchange),
            parameters(parameters),
            tradeTexts(tradeTexts)
        {}

        TradeTexts& getTradeTexts() {
            return tradeTexts;
        }

        void addBuyText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "BUY") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            addText(tradeTexts.getBuyTextRealChoords(), tradeTexts.getBuyTexts(), currentTime, currentPrice, text);
        }

        void addSellText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "SELL") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            addText(tradeTexts.getSellTextRealChoords(), tradeTexts.getSellTexts(), currentTime, currentPrice, text);
        }

        void addErrorText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "ERROR") {
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            addText(tradeTexts.getErrorTextRealChoords(), tradeTexts.getErrorTexts(), currentTime, currentPrice, text);
        }

        bool marketBuy(const string& symbol, double amount) {
            ms_t currentTime = exchange.getCurrentTime();
            double currentPrice = exchange.getPairAt(symbol).getPrice();
            if (exchange.marketBuy(symbol, amount, false)) {
                addBuyText(symbol, currentTime, currentPrice);
                return true;
            }
            LOG(
                " Exchange time: " + ms_to_datetime(currentTime) 
                + ", Strategy BUY Error, [" + symbol + "] " + to_string(amount)
            );
            addErrorText(symbol, currentTime, currentPrice);
            return false;
        }

        bool marketSell(const string& symbol, double amount) {
            ms_t currentTime = exchange.getCurrentTime();
            double currentPrice = exchange.getPairAt(symbol).getPrice();
            if (exchange.marketSell(symbol, amount, false)) {
                addSellText(symbol, currentTime, currentPrice);
                return true;
            }
            LOG(
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

    class TradeHistoryChart: public Chart {
    protected:

        const TradeHistory& history;
        TradeTexts& tradeTexts;
        // const Zoom& zoom;
        const bool 
            showCandles, 
            showPrices, 
            showVolumes, 
            showTexts; // TODO
        const Color& priceColor; // TODO
        const Color& volumeColor; // TODO
        const CandleStyle& candleStyle;
        const LabelStyle& buyTextStyle; // TODO
        const LabelStyle& sellTextStyle; // TODO
        const LabelStyle& errorTextStyle; // TODO

    public:

        TradeHistoryChart(
            GFX& gfx, Zoom& zoom, int left, int top, int width, int height,
            const TradeHistory& history,
            TradeTexts& tradeTexts,
            const bool showCandles = true, // TODO
            const bool showPrices = true, // TODO
            const bool showVolumes = true, // TODO
            const bool showTexts = true, // TODO
            const Color& priceColor = orange, // TODO
            const Color& volumeColor = darkGray, // TODO
            const Border border = Theme::chartBorder,
            const Color backgroundColor = Theme::chartBackgroundColor,
            const CandleStyle& candleStyle = Scale::defaultCandleStyle,
            const LabelStyle& buyTextStyle = LabelStyle(red), // TODO
            const LabelStyle& sellTextStyle = LabelStyle(green), // TODO
            const LabelStyle& errorTextStyle = LabelStyle(gray) // TODO
        ):  
            Chart(
                gfx, zoom, left, top, width, height,
                border, backgroundColor
            ),
            history(history),
            tradeTexts(tradeTexts),
            // zoom(zoom),
            showCandles(showCandles),
            showPrices(showPrices),
            showVolumes(showVolumes),
            showTexts(showTexts),
            priceColor(priceColor),
            volumeColor(volumeColor),
            candleStyle(candleStyle),
            buyTextStyle(buyTextStyle),
            sellTextStyle(sellTextStyle),
            errorTextStyle(errorTextStyle)
        {}

        virtual ~TradeHistoryChart() {}

        void project() override final {
            destroyScales();
            Scale* candlesScale = NULL;
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
                candlesScale = createScale(zoom, CANDLE, &candleStyle);
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
                    Scale* priceScale = createScale(zoom, LINE, &priceColor);
                    priceScale->project(pricesRealPoints);
                    if (candlesScale) Scale::alignXY(*priceScale, *candlesScale);
                }
                if (showVolumes) 
                    createScale(zoom, LINE, &volumeColor)->project(volumesRealPoints);
            }

            Scale* scale0 = getScaleAt(0);
            if (scale0 && showTexts) {
                Scale* buyTextScale = createScale(zoom, LABEL, &buyTextStyle);
                Scale* sellTextScale = createScale(zoom, LABEL, &sellTextStyle);
                Scale* errorTextScale = createScale(zoom, LABEL, &errorTextStyle);
                Scale::alignXY(*scale0, *buyTextScale);
                Scale::alignXY(*scale0, *sellTextScale);
                Scale::alignXY(*scale0, *errorTextScale);
                buyTextScale->project(
                    tradeTexts.getBuyTextRealChoords(), tradeTexts.getBuyTexts(), false);
                sellTextScale->project(
                    tradeTexts.getSellTextRealChoords(), tradeTexts.getSellTexts(), false);
                errorTextScale->project(
                    tradeTexts.getErrorTextRealChoords(), tradeTexts.getErrorTexts(), false);
            }
        }
    };

    class CandleStrategyBacktester {
    protected:
        MultiChartAccordion& multiChartAccordion;
        Zoom& zoom;
        const TradeHistory& history;
        TradeHistoryChart& tradeHistoryChart;
        TestExchange& testExchange;
        CandleStrategy& candleStrategy;
        const string& symbol;
        const int multiChartAccordionFramesHeight = 340; // TODO
        const bool showBalanceQuotedScale = true; // TODO
    public:

        CandleStrategyBacktester(
            MultiChartAccordion& multiChartAccordion, 
            Zoom& zoom, 
            const TradeHistory& history,
            TradeHistoryChart& tradeHistoryChart,
            TestExchange& testExchange,
            CandleStrategy& candleStrategy,
            const string& symbol,
            const int multiChartAccordionFramesHeight = 340, // TODO
            const bool showBalanceQuotedScale = true // TODO
        ):
            multiChartAccordion(multiChartAccordion),
            zoom(zoom),
            history(history),
            tradeHistoryChart(tradeHistoryChart),
            testExchange(testExchange),
            candleStrategy(candleStrategy),
            symbol(symbol),
            multiChartAccordionFramesHeight(multiChartAccordionFramesHeight),
            showBalanceQuotedScale(showBalanceQuotedScale)
        {}

        void backtest() const {
            multiChartAccordion.addChart(
                "History", tradeHistoryChart, multiChartAccordionFramesHeight
            );
            Chart& balanceQuotedChart =
                multiChartAccordion.createChart(
                    "Balance (quoted)", multiChartAccordionFramesHeight
                );
            Chart& balanceBaseChart =
                multiChartAccordion.createChart(
                    "Balance (base)", multiChartAccordionFramesHeight
                );
            multiChartAccordion.openAll();


            vector<Candle> candles = history.getCandles();
            vector<RealPoint> balanceQuotedAtCloses;
            vector<RealPoint> balanceQuotedFullAtCloses;
            vector<RealPoint> balanceBaseAtCloses;
            vector<RealPoint> balanceBaseFullAtCloses;
            Pair& pair = testExchange.getPairAt(symbol);
            for (const Candle& candle: candles) {
                testExchange.setCurrentTime(candle.getEnd());
                pair.setPrice(candle.getClose()); // TODO: set the price to a later price (perhaps next open price) so that, we can emulate some exchange communication latency
            
                double currentTime = (double)candle.getEnd();

                balanceQuotedAtCloses.push_back(RealPoint(
                    currentTime,
                    testExchange.getBalanceQuoted(pair)
                ));
                
                balanceQuotedFullAtCloses.push_back(RealPoint(
                    currentTime,
                    testExchange.getBalanceQuotedFull(pair)
                ));
            
                balanceBaseAtCloses.push_back(RealPoint(
                    currentTime,
                    testExchange.getBalanceBase(pair)
                ));
                
                balanceBaseFullAtCloses.push_back(RealPoint(
                    currentTime,
                    testExchange.getBalanceBaseFull(pair)
                ));

                candleStrategy.onCandleClose(candle);
            }
            
            tradeHistoryChart.project();


            Chart::Scale* balanceQuotedFullScale = balanceQuotedChart.createScale(zoom, LINE, &lightGreen);
            balanceQuotedFullScale->adaptXY(balanceQuotedFullAtCloses);
            if (showBalanceQuotedScale) {
                Chart::Scale* balanceQuotedScale = balanceQuotedChart.createScale(zoom, LINE, &green);
                balanceQuotedScale->adaptXY(balanceQuotedAtCloses);
                Chart::Scale::alignXY(*balanceQuotedScale, *balanceQuotedFullScale);
                balanceQuotedScale->project(balanceQuotedAtCloses, false);
            }
            balanceQuotedFullScale->project(balanceQuotedFullAtCloses, false);

            Chart::Scale* balanceBaseFullScale = balanceBaseChart.createScale(zoom, LINE, &yellow);
            balanceBaseFullScale->adaptXY(balanceBaseFullAtCloses);
            Chart::Scale* balanceBaseScale = balanceBaseChart.createScale(zoom, LINE, &orange);
            balanceBaseScale->adaptXY(balanceBaseAtCloses);
            Chart::Scale::alignXY(*balanceBaseScale, *balanceBaseFullScale);
            balanceBaseScale->project(balanceBaseAtCloses, false);
            balanceBaseFullScale->project(balanceBaseFullAtCloses, false);
        };
    };

}
