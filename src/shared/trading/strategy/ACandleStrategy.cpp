#include "../../../includes/madlib/trading/trading.hpp"

namespace madlib::trading::strategy {

    class ACandleStrategy: public CandleStrategy {
    protected:
        const double buyAmount = 1;
        Candle prevCandle;
        int stage = 0;
        long tick = 0;
        double exitAt = 0;
    public:

        using CandleStrategy::CandleStrategy;

        virtual ~ACandleStrategy() {}

        void onCandleClose(const Candle& candle) override {
            const string symbol = parameters.at("symbol").getString();
            // LOG(
            //     ms_to_datetime(candle.getEnd()), 
            //     " ", candle.getClose(), 
            //     " QF:", exchange.getBalanceQuotedFull(symbol), 
            //     " Q:", exchange.getBalanceQuoted(symbol),
            //     " B:", exchange.getBalanceBase(symbol)
            // );  

            double orderAmount;
            double balanceBase09;
            double balanceQuoted09;
            double orderAmountPrice;
            switch (stage)
            {
                case 0: // buy
                    balanceBase09 = exchange.getBalanceBase(symbol) * 0.9;
                    orderAmount = buyAmount > balanceBase09 ? balanceBase09 : buyAmount;
                    balanceQuoted09 = exchange.getBalanceQuoted(symbol) * 0.9;
                    orderAmountPrice = orderAmount * exchange.getPairAt(symbol).getPrice();
                    if (orderAmountPrice < balanceQuoted09) {
                        marketBuy(symbol, orderAmount);
                    }
                    exitAt = exchange.getBalanceQuotedFull(symbol) * 1.25;
                    stage = 1;
                    break;

                case 1: // sell
                    
                    if (tick > 50 /*|| exitAt > exchange.getBalanceQuotedFull(symbol)*/) {
                        balanceBase09 = exchange.getBalanceBase(symbol) * 0.9;
                        marketSell(symbol, buyAmount > balanceBase09 ? balanceBase09 : buyAmount);
                        stage = 2;
                    }
                    break;
                    
                case 2: // wait  
                    if (tick > 30) {
                        tick = -1;
                        stage = 0;
                    }
                    break;
                
                default:
                    break;
            }

            tick++;
            prevCandle = candle;

            // cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
        }
    };

    EXPORT_CLASS(ACandleStrategy)
}
