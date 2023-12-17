#pragma once

#include "Candle.hpp"
#include "CandleHistory.hpp"
#include "TestExchange.hpp"
#include "CandleStrategy.hpp"

namespace madlib::trading {
    
    class CandleStrategyBacktester {
    public:

        class ProgressContext {
        public:
            
            void* context = nullptr;
            const Candle* candle = nullptr;

            ProgressContext() {}

            virtual ~ProgressContext() {}
        };

        typedef bool (*ProgressCallback)(ProgressContext&);

    protected:

        void* context;
        CandleHistory*& candleHistory;
        TestExchange& testExchange;
        CandleStrategy& candleStrategy;
        const string& symbol;
        ProgressCallback onProgressStart = nullptr;
        ProgressCallback onProgressStep = nullptr;
        ProgressCallback onProgressFinish = nullptr;
        
    public:

        CandleStrategyBacktester(
            void* context,
            CandleHistory*& candleHistory,
            TestExchange& testExchange,
            CandleStrategy& candleStrategy,
            const string& symbol,
            const ProgressCallback onProgressStart = nullptr,
            const ProgressCallback onProgressStep = nullptr,
            const ProgressCallback onProgressFinish = nullptr
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

        // void setCandleHistory(const CandleHistory& candleHistory) {
        //     this->candleHistory = candleHistory;
        // }

        bool backtest() {

            // **** backtest ****

            ProgressContext progressContext;
            progressContext.context = context;

            if (onProgressStart && !onProgressStart(progressContext)) return false;

            vector<Candle> candles = candleHistory->getCandles();
            Pair& pair = testExchange.getPairAt(symbol);
            
            for (const Candle& candle: candles) {
                progressContext.candle = &candle;

                testExchange.setCurrentTime(candle.getEnd());
                pair.setPrice(candle.getClose()); // TODO: set the price to a later price (perhaps next open price) so that, we can emulate some exchange communication latency

                if (onProgressStep && !onProgressStep(progressContext)) 
                    return false;
                
                candleStrategy.onCandleClose(testExchange, symbol, candle);
            }

            if (onProgressFinish) return onProgressFinish(progressContext);

            return true;
        }
    };

}