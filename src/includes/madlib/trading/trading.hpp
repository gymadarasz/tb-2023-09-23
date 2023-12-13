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
        ms_t timestamp;
    };

    class History: public Shared {
    protected:

        const string& symbol;
        const ms_t startTime;
        const ms_t endTime;
        const ms_t period;

    public:
        struct Args {
            const string symbol;
            const ms_t startTime; 
            const ms_t endTime;
            const ms_t period;
        };
        explicit History(void* context): 
            symbol(((Args*)context)->symbol),
            startTime(((Args*)context)->startTime),
            endTime(((Args*)context)->endTime),
            period(((Args*)context)->period)
        {}

        virtual ~History() {}

        ms_t getStartTime() const {
            return startTime;
        }

        ms_t getEndTime() const {
            return endTime;
        }
    };

    class CandleHistory: public History {
    protected:

        vector<Candle> candles;

    public:
        using History::History;

        virtual ~CandleHistory() {}

        virtual vector<Trade> getTrades() const {
            throw ERR_UNIMP;
        }

        virtual vector<Candle> getCandles() const {
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

    class TradeCandleHistory: public CandleHistory {
    protected:
        vector<Trade> trades;

        // TODO: yagni??
        void convertToCandles() {

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

    public:

        using CandleHistory::CandleHistory;

        virtual ~TradeCandleHistory() {}

        virtual vector<Trade> getTrades() const override {
            return trades;
        }
    };


    class MonteCarloHistory: public TradeCandleHistory {
    protected:
        double volumeMean;
        double volumeStdDeviation;
        double priceMean;
        double priceStdDeviation;
        double timeLambda;
        mt19937 gen;

        // Function to init events within a specified time range
        void generateTrades() {
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
        struct Args: public TradeCandleHistory::Args {
            double volumeMean;
            double volumeStdDeviation;
            double priceMean;
            double priceStdDeviation;
            double timeLambda;
            unsigned int seed = random_device()(); // TODO: Add a seed parameter with a default value
        };
        explicit MonteCarloHistory(void* context): 
            TradeCandleHistory(context),
            volumeMean(((Args*)context)->volumeMean),
            volumeStdDeviation(((Args*)context)->volumeStdDeviation),
            priceMean(((Args*)context)->priceMean),
            priceStdDeviation(((Args*)context)->priceStdDeviation),
            timeLambda(((Args*)context)->timeLambda),
            gen(((Args*)context)->seed)
        {}
        
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

    class Exchange: public Shared {
    protected:
        
        map<string, Pair> pairs;
        map<string, Balance> balances;

    public:

        Exchange() {}

        virtual ~Exchange() {}

        virtual vector<string> getSymbols() const {
            throw ERR_UNIMP;
        }

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

        vector<string> symbols;

        ms_t currentTime = 0;
        double currentPrice = 0;

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
        struct Args {
            const vector<string>& symbols;
            const map<string, Pair>& pairs;
            const map<string, Balance>& balances;
        };
        
        explicit TestExchange(void* context): 
            Exchange(), 
            symbols(((Args*)context)->symbols)
        {
            pairs = ((Args*)context)->pairs;
            balances = ((Args*)context)->balances;
        }

        virtual void init(void*) override {

        }

        virtual vector<string> getSymbols() const override {
            return symbols;
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

    class Strategy: public Shared {
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

        Exchange& exchange;
        map<string, Parameter> parameters;
        LabelSeries* labelSeries;

    public:
        struct Args {
            Exchange& exchange;
            const map<string, Parameter>& parameters;
            LabelSeries* labelSeries = NULL;
        };

        explicit Strategy(void* context):
            exchange(((Args*)context)->exchange),
            parameters(((Args*)context)->parameters),
            labelSeries(((Args*)context)->labelSeries)
        {}
        
        virtual ~Strategy() {}

        LabelSeries* getLabelSeries() const {
            return labelSeries;
        }

        void setLabelSeries(LabelSeries* labelSeries) {
            this->labelSeries = labelSeries;
        }

        void addBuyText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "BUY", Color color = Theme::defaultTradeLabelBuyColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getBuyTextRealChoords(), labelSeries.getBuyTexts(), currentTime, currentPrice, text);
        }

        void addSellText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "SELL", Color color = Theme::defaultTradeLabelSellColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getSellTextRealChoords(), labelSeries.getSellTexts(), currentTime, currentPrice, text);
        }

        void addErrorText(const string& symbol, ms_t currentTime = 0, double currentPrice = 0, const string& text = "ERROR", Color color = Theme::defaultTradeLabelErrorColor) {
            if (!labelSeries) return;
            if (!currentTime) currentTime = exchange.getCurrentTime();
            if (!currentPrice) currentPrice = exchange.getPairAt(symbol).getPrice();
            Chart& chart = (Chart&)labelSeries->getTimeRangeArea();
            labelSeries->getShapes().push_back(chart.createLabelShape(currentTime, currentPrice, text, color));
            // addText(labelSeries.getErrorTextRealChoords(), labelSeries.getErrorTexts(), currentTime, currentPrice, text);
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

        virtual ~CandleStrategy() {}

        virtual void onCandleClose(const Candle&) {
            throw ERR_UNIMP;
        }
    };


    class CandleHistoryChart: public Chart {
    protected:
        const CandleHistory& candleHistory;
        LabelSeries* labelSeries = NULL;
        const Color& priceColor;
        const Color& volumeColor;

        Projector* mainProjector = NULL;
        CandleSeries* candleSeries = NULL;
        PointSeries* priceSeries = NULL;
        PointSeries* volumeSeries = NULL;
        LabelSeries* textSeries = NULL;

    public:

        CandleHistoryChart(
            GFX& gfx, int left, int top, int width, int height,
            // ms_t timeRangeBegin, ms_t timeRangeEnd,
            const CandleHistory& candleHistory,
            LabelSeries* labelSeries = NULL,
            const bool showCandles = true, // TODO
            const bool showPrices = true, // TODO
            const bool showVolumes = true, // TODO
            const bool showTexts = true, // TODO
            const bool generate = true,
            const Color& priceColor = Theme::defaultTradeHistoryChartPriceColor,
            const Color& volumeColor = Theme::defaultTradeHistoryChartVolumeColor,
            void* eventContext = NULL
        ):  
            Chart(
                gfx, left, top, width, height,
                candleHistory.getStartTime(), 
                candleHistory.getEndTime(),
                Theme::defaultChartBorder,
                Theme::defaultChartBackgroundColor,
                Theme::defaultChartBorderColor,
                eventContext
            ),
            candleHistory(candleHistory),
            labelSeries(labelSeries),
            priceColor(priceColor),
            volumeColor(volumeColor)
        {
            if (showCandles)
                mainProjector = candleSeries = createCandleSeries();
            if (showPrices) {
                priceSeries = createPointSeries(
                    mainProjector, true, priceColor
                );
            }
            if (showVolumes)
                volumeSeries = createPointSeries(NULL, true, volumeColor);
            if (showTexts)
                textSeries = createLabelSeries(mainProjector, true);
            
            if (generate) this->generate();
        }

        virtual ~CandleHistoryChart() {}

        LabelSeries* getTextSeries() const {
            return textSeries;
        }

        void generate() {
            
            if (candleSeries) {
                vector<Candle> candles = candleHistory.getCandles();
                vector<Shape*>& candleShapes = candleSeries->getShapes();
                for (const Candle& candle: candles) {
                    candleShapes.push_back(createCandleShape(
                        candle.getStart(),
                        candle.getEnd(),
                        candle.getOpen(),
                        candle.getLow(),
                        candle.getHigh(),
                        candle.getClose()
                    ));
                }
            }

            if (priceSeries || volumeSeries) {
                vector<Trade> trades = candleHistory.getTrades();
                if (trades.size()) {
                    mainProjector = priceSeries;
                    for (const Trade& trade: trades) {
                        if (priceSeries) priceSeries->getShapes().push_back(
                            createPointShape(trade.timestamp, trade.price)
                        );
                        if (volumeSeries) volumeSeries->getShapes().push_back(
                            createPointShape(trade.timestamp, trade.volume)
                        );
                    }
                }
            }

            if (
                mainProjector && textSeries && labelSeries &&
                textSeries != labelSeries
            ) {
                vector<Shape*>& textShapes = textSeries->getShapes();
                for (Shape* shape: labelSeries->getShapes()) {
                    LabelShape* tradeText = (LabelShape*)shape;
                    textShapes.push_back(createLabelShape(
                        tradeText->time(), 
                        tradeText->value(),
                        tradeText->text(),
                        tradeText->color(),
                        tradeText->backgroundColor(),
                        tradeText->padding(),
                        tradeText->hasBackground()
                    ));
                }
            }
        }
    };

    class CandleStrategyBacktester {
    public:

        class ProgressContext {
        public:
            
            void* context = NULL;
            const Candle* candle = NULL;

            ProgressContext() {}

            virtual ~ProgressContext() {}
        };

        typedef bool (*ProgressCallback)(ProgressContext&);

    protected:

        void* context;
        const CandleHistory& candleHistory;
        TestExchange& testExchange;
        CandleStrategy& candleStrategy;
        const string& symbol;
        ProgressCallback onProgressStart = NULL;
        ProgressCallback onProgressStep = NULL;
        ProgressCallback onProgressFinish = NULL;
        
    public:

        CandleStrategyBacktester(
            void* context,
            const CandleHistory& candleHistory,
            TestExchange& testExchange,
            CandleStrategy& candleStrategy,
            const string& symbol,
            const ProgressCallback onProgressStart = NULL,
            const ProgressCallback onProgressStep = NULL,
            const ProgressCallback onProgressFinish = NULL
        ):
            context(context),
            candleHistory(candleHistory),
            testExchange(testExchange),
            candleStrategy(candleStrategy),
            symbol(symbol),
            onProgressStart(onProgressStart),
            onProgressStep(onProgressStep),
            onProgressFinish(onProgressFinish)
        {}

        virtual ~CandleStrategyBacktester() {}

        bool backtest() {

            // **** backtest ****

            ProgressContext progressContext;
            progressContext.context = context;

            if (onProgressStart && !onProgressStart(progressContext)) return false;

            vector<Candle> candles = candleHistory.getCandles();
            Pair& pair = testExchange.getPairAt(symbol);
            for (const Candle& candle: candles) {
                progressContext.candle = &candle;

                testExchange.setCurrentTime(candle.getEnd());
                pair.setPrice(candle.getClose()); // TODO: set the price to a later price (perhaps next open price) so that, we can emulate some exchange communication latency

                if (onProgressStep && !onProgressStep(progressContext)) 
                    return false;
                
                candleStrategy.onCandleClose(candle);
            }

            if (onProgressFinish) return onProgressFinish(progressContext);

            return true;
        }
    };

    class CandleStrategyBacktesterMultiChart: public MultiChartAccordion {
    protected:

        CandleStrategyBacktester* backtester;

        const CandleHistory& candleHistory;
        TestExchange& testExchange;
        Strategy& candleStrategy;
        const string& symbol;
        const bool showBalanceQuotedScale;
        const bool logProgress; // TODO: progress callbacks 
        const bool showProgress;
        const ms_t showProgressAfterMs;
        const ms_t showProgressFreqMs;

        // **** tradeHistoryChart ****
        
        CandleHistoryChart candleHistoryChart = CandleHistoryChart(
            gfx, 0, 0, 0, 0, candleHistory, candleStrategy.getLabelSeries(),
            true, false, false
        );

        // **** balanceQuotedChart ****

        Chart* balanceQuotedChart = NULL;
        PointSeries* balanceQuotedFullScale = NULL;
        PointSeries* balanceQuotedScale = NULL;

        // **** balanceBaseChart ****

        Chart* balanceBaseChart = NULL;
        PointSeries* balanceBaseFullScale = NULL;
        PointSeries* balanceBaseScale = NULL;

        struct ProgressState {
            // show log and zenity
            size_t candlesRemaining = 0, candlesSize = 0;
            ms_t progressUpdatedAt = 0;
            FILE* progressZenityPipe = NULL;

            // show results on charts:
            vector<Shape*>* balanceQuotedAtCloses = NULL;
            vector<Shape*>* balanceQuotedFullAtCloses = NULL;
            vector<Shape*>* balanceBaseAtCloses = NULL;
            vector<Shape*>* balanceBaseFullAtCloses = NULL;
            Pair* pair = NULL;

            bool showProgressStarted = false;
        } progressState;
        
        static bool onProgressStart(CandleStrategyBacktester::ProgressContext& progressContext) {
            CandleStrategyBacktesterMultiChart* that = (CandleStrategyBacktesterMultiChart*)progressContext.context;

            that->clearCharts();
            that->progressState.balanceQuotedAtCloses = &that->balanceQuotedScale->getShapes();
            that->progressState.balanceQuotedFullAtCloses = &that->balanceQuotedFullScale->getShapes();
            that->progressState.balanceBaseAtCloses = &that->balanceBaseScale->getShapes();
            that->progressState.balanceBaseFullAtCloses = &that->balanceBaseFullScale->getShapes();

            if (that->showProgress || that->logProgress) {
                that->progressState.candlesSize = that->progressState.candlesRemaining = that->candleHistory.getCandles().size();
                that->progressState.progressUpdatedAt = now() + that->showProgressAfterMs;
                if (that->logProgress) 
                    LOG("Backtest starts with ", that->progressState.candlesRemaining, " candles...");
                if (that->showProgress)
                    that->progressState.showProgressStarted = false;
            }

            that->progressState.pair = &that->testExchange.getPairAt(that->symbol);

            return true;
        }
        
        static bool onProgressStep(CandleStrategyBacktester::ProgressContext& progressContext) {
            CandleStrategyBacktesterMultiChart* that = (CandleStrategyBacktesterMultiChart*)progressContext.context;

            // show zenity and log

            if (that->showProgress || that->logProgress) {
                if (now() - that->progressState.progressUpdatedAt >= that->showProgressFreqMs) {
                    that->progressState.progressUpdatedAt = now();
                    int pc100 = (int)((1 - ((double)that->progressState.candlesRemaining / (double)that->progressState.candlesSize)) * 100);
                    if (that->logProgress) LOG("Backtest in progress: ", that->progressState.candlesRemaining, " candles remaining... (" + to_string(pc100) + "% done)");
                    if (that->showProgress) {

                        if (!that->progressState.showProgressStarted) {
                            that->progressState.progressZenityPipe = zenity_progress("Backtest");
                            that->progressState.showProgressStarted = true;
                        }

                        if (
                            !zenity_progress_update(that->progressState.progressZenityPipe, pc100) ||
                            !zenity_progress_update(that->progressState.progressZenityPipe, "Remaining candles: " + to_string(that->progressState.candlesRemaining))
                        ) {
                            LOG("User canceled.");
                            return false;
                        }
                    }
                }
                that->progressState.candlesRemaining--; 
            }
            
            // collect data to charts

            ms_t currentTime = progressContext.candle->getEnd();
            Pair* pair = that->progressState.pair;

            // **** balanceQuotedChart ****

            if (that->showBalanceQuotedScale) {
                that->progressState.balanceQuotedAtCloses->push_back(that->balanceQuotedChart->createPointShape(
                    currentTime,
                    that->testExchange.getBalanceQuoted(*pair)
                ));
            }
            
            that->progressState.balanceQuotedFullAtCloses->push_back(that->balanceQuotedChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceQuotedFull(*pair)
            ));

            // **** balanceBaseChart ****
        
            that->progressState.balanceBaseAtCloses->push_back(that->balanceBaseChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceBase(*pair)
            ));
            
            that->progressState.balanceBaseFullAtCloses->push_back(that->balanceBaseChart->createPointShape(
                currentTime,
                that->testExchange.getBalanceBaseFull(*pair)
            ));

            return true;
        }
        
        static bool onProgressFinish(CandleStrategyBacktester::ProgressContext& progressContext) {
            CandleStrategyBacktesterMultiChart* that = (CandleStrategyBacktesterMultiChart*)progressContext.context;

            if (that->logProgress) LOG("Backtest done.");
            if (that->showProgress) zenity_progress_close(that->progressState.progressZenityPipe);

            return true;
        }

    public:

        CandleStrategyBacktesterMultiChart(
            GFX& gfx, int left, int top, int width,
            const int multiChartAccordionFramesHeight,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            const CandleHistory& candleHistory,
            TestExchange& testExchange,
            CandleStrategy& candleStrategy,
            const string& symbol,
            const bool showBalanceQuotedScale = true, // TODO

            const bool logProgress = true, // TODO
            const bool showProgress = true,
            const ms_t showProgressAfterMs = 0,
            const ms_t showProgressFreqMs = second,

            bool single = false,
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor,
            void* eventContext = NULL
        ):
            MultiChartAccordion(
                gfx, left, top, width,
                timeRangeBegin, timeRangeEnd,
                single, border, backgroundColor, 
                eventContext
            ),
            candleHistory(candleHistory),
            testExchange(testExchange),
            candleStrategy(candleStrategy),
            symbol(symbol),
            showBalanceQuotedScale(showBalanceQuotedScale),
            logProgress(logProgress),
            showProgress(showProgress),
            showProgressAfterMs(showProgressAfterMs),
            showProgressFreqMs(showProgressFreqMs)
        {

            backtester = new CandleStrategyBacktester(
                this, candleHistory, testExchange, candleStrategy, symbol, 
                onProgressStart, onProgressStep, onProgressFinish
            );

            // **** tradeCandleHistoryChart ****

            createChartFrame(
                "History", candleHistoryChart, multiChartAccordionFramesHeight
            );

            if (!candleStrategy.getLabelSeries())
                candleStrategy.setLabelSeries(
                    candleHistoryChart.getTextSeries()
                );

            multiChart.attach(candleHistoryChart);

            // **** balanceQuotedChart ****

            balanceQuotedChart = createChart(
                "Balance (quoted)", multiChartAccordionFramesHeight
            );
            balanceQuotedFullScale = balanceQuotedChart->createPointSeries(NULL, true, lightGreen);
            balanceQuotedScale = balanceQuotedChart->createPointSeries(balanceQuotedFullScale, true, green);

            // **** balanceBaseChart ****

            balanceBaseChart = createChart(
                "Balance (base)", multiChartAccordionFramesHeight
            );
            balanceBaseFullScale = balanceBaseChart->createPointSeries(NULL, true, yellow);
            balanceBaseScale = balanceBaseChart->createPointSeries(balanceBaseFullScale, true, orange);

            openAll(false);
        }

        virtual ~CandleStrategyBacktesterMultiChart() {
            delete backtester;
        }

        // TODO: separated backtester class that can be reused from command line
        void backtest() {

            if (!backtester->backtest()) {
                LOG("Backtest failed"); // TODO
            }

        }
    };

}
