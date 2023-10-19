#pragma once

#include <vector>
#include <string>
// #include <../Vector.hpp>
// #include <../Tools.hpp>

using namespace std;
using namespace chrono;
using namespace madlib;

namespace madlib::trading {

    ms_t period_to_ms(const string period) {
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
        };
        return periods[period];
    }

    class Candle {
    protected:
        double open, close, low, high, volume;
        ms_t start, end; // ms
    public:
        Candle(
            double open, double close, double low, double high, double volume, 
            ms_t start, ms_t end
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

    // class History {
    // public:
    //     virtual vector<Candle> fetch(
    //         const string symbol, 
    //         const ms_t interval, 
    //         const ms_t from, 
    //         const ms_t to) const;
        
    //     void save(const string filename, vector<Candle> candles) const {
    //         Vector::save<Candle>(filename, candles);
    //     }
        
    //     vector<Candle> load(const string filename) const {
    //         return Vector::load<Candle>(filename);
    //     }
        
    //     vector<Candle>& load(const string filename, vector<Candle>& data) const {
    //         return Vector::load<Candle>(filename, data);
    //     }
    // };

    // Define a struct for trade event data
    struct TradeEvent {
        double volume;
        double price;
        ms_t timestamp; // ms
    };

    class MonteCarloHistory {
    protected:
        double volumeMean;
        double volumeStdDeviation;
        double priceMean;
        double priceStdDeviation;
        double timeMean;
        double timeStdDeviation;
        ms_t startTime; // ms
        ms_t endTime; // ms
        ms_t period;
        mt19937 gen;
        vector<TradeEvent> tradeEvents;
        vector<Candle> candles;

        void generateCandles() {            
            auto tradeEventIter = tradeEvents.begin();
            
            ms_t currentStart = startTime;
            ms_t currentEnd = currentStart + period;
            while (tradeEventIter != tradeEvents.end()) {
                cout << ms_to_datetime(currentStart) << " - " << ms_to_datetime(currentEnd) << endl;
                
                double open = tradeEventIter->price;
                double close = open;
                double low = open;
                double high = open;
                double volume = 0.0;
                
                while (tradeEventIter != tradeEvents.end() && tradeEventIter->timestamp >= currentStart && tradeEventIter->timestamp < currentEnd) {
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

        // Function to generate events within a specified time range
        void generateTradeEvents() {
            ms_t previousTime = startTime;
            double previousPrice = priceMean;
            double previousVolume = volumeMean;

            while (previousTime < endTime) {
                TradeEvent tradeEvent;

                // Generate elapsed time ensuring it's always greater than 1 ms
                ms_t elapsed_time = 0;
                while (elapsed_time < 1) {
                    normal_distribution<double> timeDistribution(timeMean, timeStdDeviation);
                    elapsed_time = (ms_t)abs(timeDistribution(gen));
                }

                // Accumulate elapsed time to calculate the timestamp
                previousTime += elapsed_time;

                // Generate price movement
                normal_distribution<double> priceDistribution(0, priceStdDeviation);
                double priceMovement = priceDistribution(gen);

                // Calculate the new price based on the previous price and movement
                tradeEvent.price = previousPrice + priceMovement;

                // Generate volume movement
                normal_distribution<double> volumeDistribution(0, volumeStdDeviation);
                double volumeMovement = volumeDistribution(gen);

                // Calculate the new volume based on the previous volume and movement
                tradeEvent.volume = previousVolume + volumeMovement;

                if (previousTime >= endTime) {
                    break;  // Stop generating events if we've reached or passed the end time
                }

                tradeEvent.timestamp = previousTime;
                tradeEvents.push_back(tradeEvent);

                previousPrice = tradeEvent.price;  // Update the previous_price
                previousVolume = tradeEvent.volume;  // Update the previous_volume
            }
        }

    public:
        MonteCarloHistory(
            double volumeMean, double volumeStdDeviation, 
            double priceMean, double priceStdDeviation,
            double timeMean, double timeStdDeviation,
            ms_t startTime, ms_t endTime, ms_t period,
            unsigned int seed = std::random_device()() // Add a seed parameter with a default value
        ):
            volumeMean(volumeMean), volumeStdDeviation(volumeStdDeviation),
            priceMean(priceMean), priceStdDeviation(priceStdDeviation),
            timeMean(timeMean), timeStdDeviation(timeStdDeviation),
            startTime(startTime), endTime(endTime), period(period),
            gen(seed)
        {
            // Initialize the random number generator
            // random_device rd;
            // gen = mt19937(rd());
            
            generateTradeEvents();
            generateCandles();
        }

        vector<TradeEvent> getTradeEvents() const {
            return tradeEvents;
        }

        vector<Candle> getCandles() const {
            return candles;
        }
        

        // Function to print the generated events
        void PrintEvents() {
            for (const TradeEvent& tradeEvent : tradeEvents) {
                cout << "Event: Volume=" << tradeEvent.volume 
                    << ", Price=" << tradeEvent.price 
                    << ", Timestamp=" << tradeEvent.timestamp;
            }
        }
    };

}
