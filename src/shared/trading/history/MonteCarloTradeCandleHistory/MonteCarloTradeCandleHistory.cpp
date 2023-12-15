#include "../../../../includes/madlib/trading/trading.hpp"

namespace madlib::trading::history {

    class MonteCarloTradeCandleHistory: public TradeCandleHistory {
    protected:
        // TODO: load config somehow..??
        // const string symbol = "MONTECARLO";
        const double volumeMean = 50;  // Initial volume
        const double volumeStdDeviation = 5;
        const double priceMean = 10000;  // Initial price
        const double priceStdDeviation = 50;
        const double timeLambda = MS_PER_SEC * 20;  // Mean time in milliseconds (60 seconds)
        const unsigned int seed = random_device()(); // 12312334
        
        mt19937 gen = mt19937(seed);

        // Function to init events within a specified time range
        void generateTrades() {
            ms_t previousTime = startTime;
            double previousPrice = priceMean;
            double previousVolume = volumeMean;
            
            trades.clear();
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
        // TODO !@# make it module
        // struct Args: public TradeCandleHistory::Args {
        //     double volumeMean;
        //     double volumeStdDeviation;
        //     double priceMean;
        //     double priceStdDeviation;
        //     double timeLambda;
        //     unsigned int seed = random_device()(); // TODO: Add a seed parameter with a default value
        // };
        // explicit MonteCarloTradeCandleHistory(void* context): 
        //     TradeCandleHistory(context),
        //     volumeMean(((Args*)context)->volumeMean),
        //     volumeStdDeviation(((Args*)context)->volumeStdDeviation),
        //     priceMean(((Args*)context)->priceMean),
        //     priceStdDeviation(((Args*)context)->priceStdDeviation),
        //     timeLambda(((Args*)context)->timeLambda),
        //     gen(((Args*)context)->seed)
        // {}

        using TradeCandleHistory::TradeCandleHistory;
        
        virtual ~MonteCarloTradeCandleHistory() {}

        // virtual void init(void* = nullptr) override {}

        virtual void load(Progress&) override {
            generateTrades();
            convertToCandles();
        }

        // virtual void clear() override {
        //     trades.clear();
        //     candles.clear();
        // }

        // Function to print the generated events
        void PrintEvents() {
            for (const Trade& trade : trades) {
                cout << "Event: Volume=" << trade.volume 
                    << ", Price=" << trade.price 
                    << ", Timestamp=" << trade.timestamp;
            }
        }
    };

    extern "C" MonteCarloTradeCandleHistory* createMonteCarloTradeCandleHistory(
            const string& symbol,
            const ms_t startTime,
            const ms_t endTime,
            const ms_t period
    ) {
        return new MonteCarloTradeCandleHistory(symbol, startTime, endTime, period);
    }

}