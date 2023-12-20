#include "../../../../includes/madlib/trading/CandleStrategy.hpp"
#include "../../../../includes/madlib/trading/Exchange.hpp"
#include "../../../../includes/madlib/trading/inicators/EmaIndicator.hpp"

namespace madlib::trading::strategy {


    class MartingaleCandleStrategy: public CandleStrategy {
    protected:

        // params:
        double initialBuyPc = 0.01;
        double buyIncPc = 1.8;
        double profitPc = 1.09;
        double sellPc = 0.9;
        double buyBellowPc = 0.9;
        ms_t waitBeforeBuyAgain = 120 * minute;

        //
        ms_t dontBuyUntil = 0;
        double sellAbove = INFINITY;
        double buyPc = initialBuyPc;
        double buyBellow = INFINITY;

        void reinit(double price, ms_t closeAt) {
            dontBuyUntil = closeAt + waitBeforeBuyAgain;
            sellAbove = INFINITY;
            buyPc = initialBuyPc;
            buyBellow = price * buyBellowPc;
        }

    public:

        PointSeries* sellAboveProjector = nullptr;
        Chart* rsiChart = nullptr;
        PointSeries* rsiProjector = nullptr;

        EmaIndicator* emaIndicator1 = nullptr;
        EmaIndicator* emaIndicator2 = nullptr;
        EmaIndicator* emaIndicator3 = nullptr;

        MartingaleCandleStrategy(): CandleStrategy() {}

        virtual ~MartingaleCandleStrategy() {
            delete emaIndicator1;
            delete emaIndicator2;
            delete emaIndicator3;
        }

        bool first = true;

        virtual void onFirstCandleClose(Exchange*&, const string&, const Candle& candle) override {            
            ms_t closeAt = candle.getEnd();
            double price = candle.getClose();

            sellAboveProjector = balanceQuotedChart->createPointSeries(
                balanceQuotedChart->getProjectorAt(0), true, darkGray
            );

            emaIndicator1 = new EmaIndicator(candleHistoryChart, price, 2000, blue);
            emaIndicator2 = new EmaIndicator(candleHistoryChart, price, 4000, green);
            emaIndicator3 = new EmaIndicator(candleHistoryChart, price, 16000, orange);

            rsiChart = multichartAccordion->createChart("RSI", 200);
            rsiProjector = rsiChart->createPointSeries();

            reinit(price, closeAt);
        }

        virtual void onCandleClose(Exchange*& exchange, const string& symbol, const Candle& candle) override {            
            ms_t closeAt = candle.getEnd();
            double price = candle.getClose();
            double balanceQuotedFull = exchange->getBalanceQuotedFull(symbol);
            double balanceQuoted = exchange->getBalanceQuoted(symbol);
            double balanceBase = exchange->getBalanceBase(symbol);

            emaIndicator1->calc(closeAt, price);
            emaIndicator2->calc(closeAt, price);
            emaIndicator3->calc(closeAt, price);

            rsiProjector->getShapes().push_back(
                rsiChart->createPointShape(closeAt, price)
            );


            // sell
            if (balanceQuotedFull > sellAbove) {
                double amount = balanceBase * sellPc;
                marketSell(exchange, symbol, amount); // TODO couldn't sell?
                reinit(price, closeAt);
                return;
            }

            // buy            
            if (dontBuyUntil <= closeAt && price < buyBellow) {
                double amount = (balanceQuoted * buyPc) / price;
                marketBuy(exchange, symbol, amount); // TODO: can't buy?
                double _sellAbove = balanceQuotedFull * profitPc; 
                sellAbove = 
                    // sellAbove > _sellAbove ? 
                    // sellAbove : 
                    _sellAbove;
                sellAboveProjector->getShapes().push_back(
                    balanceQuotedChart->createPointShape(closeAt, sellAbove)
                );
                buyPc *= buyIncPc;
                dontBuyUntil = closeAt + waitBeforeBuyAgain;
                buyBellow = price * buyBellowPc;
            }
        }
    };

    extern "C" MartingaleCandleStrategy* createMartingaleCandleStrategy() {
        return new MartingaleCandleStrategy();
    }
}
