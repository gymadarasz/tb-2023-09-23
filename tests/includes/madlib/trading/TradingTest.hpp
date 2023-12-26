#pragma once

#include <cassert>

#include "../../../../src/includes/madlib/trading/Balance.hpp"

using namespace madlib::trading;

class TradingTest {
public:

    // Balance

    static void testBalance_DefaultInitialization() {
        Balance balance;
        assert(balance.getAmount() == 0.0);
    }

    static void testBalance_CustomInitialization() {
        Balance balance(100.0, true);
        assert(balance.getAmount() == 100.0);
    }

    static void testBalance_SetAndGetAmount() {
        Balance balance;
        balance.setAmount(50.0);
        assert(balance.getAmount() == 50.0);
    }

    static void testBalance_IncrementPositiveAmount() {
        Balance balance;
        bool success = balance.increment(30.0);
        assert(success);
        assert(balance.getAmount() == 30.0);
    }

    static void testBalance_IncrementNegativeAmount() {
        Balance balance(50.0, false);
        bool success = balance.decrement(-20.0, false);
        assert(success);
        assert(balance.getAmount() == 70.0);
    }

    static void testBalance_DecrementPositiveAmount() {
        Balance balance(50.0, false);
        bool success = balance.decrement(20.0, false);
        assert(success);
        assert(balance.getAmount() == 30.0);
    }

    static void testBalance_DecrementNegativeAmount() {
        Balance balance(50.0, false);
        bool success = balance.decrement(-20.0, false);
        assert(success);
        assert(balance.getAmount() == 70.0);
    }

    static void testBalance_DecrementToNegative() {
        Balance balance(30.0, false);
        bool success = balance.decrement(40.0, false);
        assert(!success);
    }

    static void testBalance_CheckIfNegativeThroughIncrement() {
        Balance balance(20.0, false);
        bool success = balance.increment(-30.0, false);
        assert(!success);
    }
    
    static void testBalance_IncrementDecrementZeroAmount() {
        Balance balance;
        bool successIncrement = balance.increment(0.0);
        bool successDecrement = balance.decrement(0.0);
        assert(successIncrement);
        assert(successDecrement);
        assert(balance.getAmount() == 0.0);
    }
    
    static void testBalance_IncrementDecrementNegativeBalanceAllowed() {
        Balance balance(30.0, true);
        bool successIncrement = balance.increment(-10.0);
        bool successDecrement = balance.decrement(-20.0);
        assert(successIncrement);
        assert(successDecrement);
        assert(balance.getAmount() == 0.0);
    }

    static void testBalance_IncrementDecrementWithErrorHandling() {
        Balance balance(20.0, false);
        bool successIncrement = balance.increment(-30.0, false);
        bool successDecrement = balance.decrement(30.0, false);
        assert(!successIncrement);
        assert(!successDecrement);
        assert(balance.getAmount() == 20.0);
    }

    static void testBalance_CheckIfNegativeViaIncrement() {
        Balance balance(20.0, false);
        bool successIncrement = balance.increment(-30.0, false);
        assert(!successIncrement);
    }

    static void testBalance_CheckIfNegativeViaDecrement() {
        Balance balance(30.0, false);
        bool successDecrement = balance.decrement(40.0, false);
        assert(!successDecrement);
    }

    // Candle

    static void testCandle_DefaultInitialization() {
        Candle candle;
        assert(candle.getOpen() == 0.0);
        assert(candle.getClose() == 0.0);
        assert(candle.getLow() == 0.0);
        assert(candle.getHigh() == 0.0);
        assert(candle.getVolume() == 0.0);
        assert(candle.getStart() == 0);
        assert(candle.getEnd() == 0);
    }

    static void testCandle_CustomInitialization() {
        ms_t startTime = 1000;
        ms_t endTime = 2000;

        Candle candle(10.0, 20.0, 5.0, 25.0, 1000.0, startTime, endTime);

        assert(candle.getOpen() == 10.0);
        assert(candle.getClose() == 20.0);
        assert(candle.getLow() == 5.0);
        assert(candle.getHigh() == 25.0);
        assert(candle.getVolume() == 1000.0);
        assert(candle.getStart() == startTime);
        assert(candle.getEnd() == endTime);
    }

    static void testCandle_SettersAndGetters() {
        Candle candle;
        candle.setOpen(15.0);
        candle.setClose(25.0);
        candle.setLow(10.0);
        candle.setHigh(30.0);
        candle.setVolume(1500.0);
        candle.setStart(500);
        candle.setEnd(1000);

        assert(candle.getOpen() == 15.0);
        assert(candle.getClose() == 25.0);
        assert(candle.getLow() == 10.0);
        assert(candle.getHigh() == 30.0);
        assert(candle.getVolume() == 1500.0);
        assert(candle.getStart() == 500);
        assert(candle.getEnd() == 1000);
    }

    static void testCandle_InvalidInitialization() {

        // Test that candle start time cannot be greater than end time
        {
            // Tolerant candle should pass with incorrect values
            Candle candleTolerant(10.0, 20.0, 5.0, 25.0, 1000.0, 2000, 1000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(10.0, 20.0, 5.0, 25.0, 1000.0, 2000, 1000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Candle start time cannot be greater than end time.", e.what()));
            }
        }

        // Test that candle volume cannot be negative
        {
            // Tolerant candle should pass with negative volume
            Candle candleTolerant(10.0, 20.0, 5.0, 25.0, -100.0, 1000, 2000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(10.0, 20.0, 5.0, 25.0, -100.0, 1000, 2000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Candle volume cannot be negative.", e.what()));
            }
        }

        // Test that candle low should be the lowest
        {
            // Tolerant candle should pass with incorrect values
            Candle candleTolerant(10.0, 20.0, 50.0, 25.0, 1000.0, 1000, 2000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(10.0, 20.0, 50.0, 25.0, 1000.0, 1000, 2000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Candle low should be the lowest.", e.what()));
            }
        }

        // Test that candle high should be the highest
        {
            // Tolerant candle should pass with incorrect values
            Candle candleTolerant(10.0, 20.0, 5.0, 15.0, 1000.0, 2000, 3000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(10.0, 20.0, 5.0, 15.0, 1000.0, 2000, 3000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Candle high should be the highest.", e.what()));
            }
        }

        // Test that candle prices cannot be negative
        {
            // Tolerant candle should pass with negative prices
            Candle candleTolerant(-10.0, -20.0, -5.0, -25.0, 1000.0, 1000, 2000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(-10.0, -20.0, -5.0, -25.0, 1000.0, 1000, 2000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Candle prices cannot be negative.", e.what()));
            }
        }

        // Test that invalid NaN values are detected
        {
            // Tolerant candle should pass with NaN values
            Candle candleTolerant(10.0, 20.0, numeric_limits<double>::quiet_NaN(), 25.0, 1000.0, 1000, 2000);
            assert(true);

            // Strict candle should throw an exception
            try {
                Candle candleStrict(10.0, 20.0, numeric_limits<double>::quiet_NaN(), 25.0, 1000.0, 1000, 2000, true);
                assert(false);
            } catch (const exception& e) {
                assert(str_ends_with("Invalid NaN value(s).", e.what()));
            }
        }
    }


    static void testCandle_ExtremeValues() {
        Candle candle(
            numeric_limits<double>::max(),       // Maximum positive double value
            numeric_limits<double>::min(),       // Minimum positive double value
            numeric_limits<double>::infinity(),  // Positive infinity
            -numeric_limits<double>::infinity(), // Negative infinity
            numeric_limits<double>::quiet_NaN(), // Quiet NaN
            numeric_limits<ms_t>::min(),         // Minimum value for ms_t
            numeric_limits<ms_t>::max()          // Maximum value for ms_t
        );

        // Assertions for the expected behavior with extreme values
        assert(candle.getOpen() == numeric_limits<double>::max());
        assert(candle.getClose() == numeric_limits<double>::min());
        assert(isinf(candle.getLow()));  // Check if low is infinity
        assert(isinf(candle.getHigh())); // Check if high is negative infinity
        assert(isnan(candle.getVolume())); // Check if volume is NaN
        assert(candle.getStart() == numeric_limits<ms_t>::min());
        assert(candle.getEnd() == numeric_limits<ms_t>::max());
    }

    static void testCandle_ConsistencyInGetterSetter() {
        Candle candle;

        candle.setOpen(15.0);
        assert(candle.getOpen() == 15.0);

        candle.setClose(25.0);
        assert(candle.getClose() == 25.0);

        candle.setLow(10.0);
        assert(candle.getLow() == 10.0);

        candle.setHigh(30.0);
        assert(candle.getHigh() == 30.0);

        candle.setVolume(1500.0);
        assert(candle.getVolume() == 1500.0);

        ms_t startTime = 500;
        candle.setStart(startTime);
        assert(candle.getStart() == startTime);

        ms_t endTime = 1000;
        candle.setEnd(endTime);
        assert(candle.getEnd() == endTime);
    }


    static void testCandle_VolumeAndPriceRelationship() {
        Candle candle(10.0, 20.0, 5.0, 25.0, 1000.0);

        // Assert that volume is non-negative
        assert(candle.getVolume() >= 0.0);

        // Assert that volume and price are positively correlated
        assert(candle.getVolume() > 0.0 && candle.getClose() > 0.0);

        // Volume should be zero when there is no price change
        Candle noPriceChangeCandle(10.0, 10.0, 10.0, 10.0, 0.0);
        assert(noPriceChangeCandle.getVolume() == 0.0);

        // Assert that high and low prices are equal when there is no price change
        assert(noPriceChangeCandle.getHigh() == noPriceChangeCandle.getLow());

        // Assert that close price is equal to open price when there is no price change
        assert(noPriceChangeCandle.getClose() == noPriceChangeCandle.getOpen());


        // Volume and price should be positively correlated during an uptrend
        Candle uptrendCandle(10.0, 20.0, 5.0, 25.0, 1000.0);
        assert(uptrendCandle.getVolume() > 0.0);
        assert(uptrendCandle.getClose() > uptrendCandle.getOpen());

        // Volume and price should be positively correlated during a downtrend
        Candle downtrendCandle(20.0, 10.0, 5.0, 25.0, 1200.0);
        assert(downtrendCandle.getVolume() > 0.0);
        assert(downtrendCandle.getOpen() > downtrendCandle.getClose());

        // Volume should be significant during a price gap
        Candle priceGapCandle(10.0, 15.0, 5.0, 25.0, 2000.0);
        double priceGap = priceGapCandle.getOpen() - priceGapCandle.getClose();
        assert(priceGapCandle.getVolume() / abs(priceGap) > 100.0);

    }


    // History

    class TestableHistory: public History {
    public:
        using History::History;
        virtual ~TestableHistory() {}
        const string& getSymbol() const {
            return symbol;
        }
        ms_t getPeriod() const {
            return period;
        }
    };


    static void testHistory_DefaultInitialization() {
        TestableHistory history("AAPL", 0, 0, 0);
        assert(history.getSymbol() == "AAPL");
        assert(history.getStartTime() == 0);
        assert(history.getEndTime() == 0);
        assert(history.getPeriod() == 0);
    }

    static void testHistory_SetAndGetSymbol() {
        TestableHistory history("AAPL", 0, 0, 0);
        history.setSymbol("GOOGL");
        assert(history.getSymbol() == "GOOGL");
    }

    static void testHistory_SetAndGetStartTime() {
        History history("AAPL", 0, 0, 0);
        history.setStartTime(1000);
        assert(history.getStartTime() == 1000);
    }

    static void testHistory_SetAndGetEndTime() {
        History history("AAPL", 0, 0, 0);
        history.setEndTime(2000);
        assert(history.getEndTime() == 2000);
    }

    static void testHistory_SetAndGetPeriod() {
        TestableHistory history("AAPL", 0, 0, 0);
        history.setPeriod(500);
        assert(history.getPeriod() == 500);
    }

    static void testHistory_Load() {
        History history("AAPL", 0, 0, 0);
        Progress progress;
        try {
            history.load(progress);
            assert(false); // Should not reach here
        } catch (const exception& e) {
            assert(str_ends_with("Unimplemented", e.what()));
        }
    }

    static void testHistory_Reload() {
        History history("AAPL", 0, 0, 0);
        Progress progress;
        try {
            history.reload(progress);
            assert(false); // Should not reach here
        } catch (const exception& e) {
            assert(str_ends_with("Unimplemented", e.what()));
        }
    }

    // CandleHistory

    class TestableCandleHistory: public CandleHistory {
    public:
        using CandleHistory::CandleHistory;
        virtual ~TestableCandleHistory() {}
        const string& getSymbol() const {
            return symbol;
        }
        ms_t getPeriod() const {
            return period;
        }
        void addCandle(Candle candle) {
            candles.push_back(candle);
        }
    };

    static void testCandleHistory_DefaultInitialization() {
        TestableCandleHistory candleHistory("AAPL", 0, 0, 0);
        assert(candleHistory.getSymbol() == "AAPL");
        assert(candleHistory.getStartTime() == 0);
        assert(candleHistory.getEndTime() == 0);
        assert(candleHistory.getPeriod() == 0);
        assert(candleHistory.getCandles().empty());
    }

    static void testCandleHistory_AddCandle() {
        TestableCandleHistory candleHistory("AAPL", 0, 0, 0);
        Candle candle(10.0, 20.0, 5.0, 25.0, 1000.0);
        candleHistory.addCandle(candle);
        assert(candleHistory.getCandles().size() == 1);
        assert(candleHistory.getCandles()[0].getOpen() == 10.0);
    }

    static void testCandleHistory_GetTrades() {
        CandleHistory candleHistory("AAPL", 0, 0, 0);
        Progress progress;
        try {
            candleHistory.getTrades();
            assert(false); // Should not reach here
        } catch (const exception& e) {
            assert(str_ends_with("Unimplemented", e.what()));
        }
    }
};
