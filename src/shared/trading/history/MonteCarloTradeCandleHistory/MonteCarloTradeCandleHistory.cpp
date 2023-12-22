#include "../../../../includes/madlib/rand.hpp"
#include "../../../../includes/madlib/time.hpp"
#include "../../../../includes/madlib/Progress.hpp"
#include "../../../../includes/madlib/graph/Mixed.hpp"
#include "../../../../includes/madlib/trading/TradeCandleHistory.hpp"


namespace madlib::trading::history {

    class MonteCarloTradeCandleHistory: public TradeCandleHistory {
    protected:
        // TODO: load config somehow..??
        // const string symbol = "MONTECARLO";
        const double defaultVolumeMean = 100;  // Initial volume
        const double defaultVolumeStdDeviation = 10;
        const double defaultPriceMean = 100;  // Initial price
        const double defaultPriceStdDeviation = 10;
        const double defaultTimeLambda = MS_PER_SEC;  // Mean time in milliseconds (60 seconds)
        const bool defaultUseRandomDevice = true;
        const unsigned int defaultSeed = 1;
        // const unsigned int defaultSeedRandomDevice = random_device()(); // 12312334
        

        // Function to init events within a specified time range
        void generateTrades() {
            const double priceMean = getDouble("priceMean");
            const double volumeMean = getDouble("volumeMean");
            const double timeLambda = getDouble("timeLambda");
            const double priceStdDeviation = getDouble("priceStdDeviation");
            const double volumeStdDeviation = getDouble("volumeStdDeviation");
            const unsigned int seed = getBool("useRandomDevice") 
                ? random_device()() 
                : (unsigned int)getLong("seed");


            mt19937 gen = mt19937(seed);

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

        MonteCarloTradeCandleHistory(
            const string& symbol,
            const ms_t startTime,
            const ms_t endTime,
            const ms_t period
        ): 
            TradeCandleHistory(symbol, startTime, endTime, period)
        {
            list.add("volumeMean", "Volume Mean", defaultVolumeMean);
            list.add("volumeStdDeviation", "Volume Standard Deviation", defaultVolumeStdDeviation);
            list.add("priceMean", "Price Mean", defaultPriceMean);
            list.add("priceStdDeviation", "Price Standard Deviation", defaultPriceStdDeviation);
            list.add("timeLambda", "Time Lambda (ms)", defaultTimeLambda);
            list.add("useRandomDevice", "Use random device", defaultUseRandomDevice);
            list.add("seed", "Use seed number", (long)defaultSeed);
        }
        
        virtual ~MonteCarloTradeCandleHistory() {}

        // virtual void init(void* = nullptr) override {}

        virtual void load(Progress&) override {
            generateTrades();
            convertToCandles();
        }

        virtual void reload(Progress& progress) override {
            load(progress);
        }

        // virtual bool (MixedInputList& inputs) override {
        //     DBG("TODO: currents:");
        //     list.dump(true);
        //     DBG("TODO: validate settings inputs:");
        //     inputs.dump(true);
        //     // dumpSettings();
        //     // throw ERR_UNIMP;
        //     return true;
        // }

        // virtual bool setList(MixedInputList& list) override {
        //     DBG("TODO: store settings");
        //     list.dump(true);
        //     // throw ERR_UNIMP;
        //     this->list = list;
        //     return true;
        // }

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