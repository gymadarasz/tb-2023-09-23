#include "../../../../includes/madlib/trading/CandleStrategy.hpp"
#include "../../../../includes/madlib/trading/Exchange.hpp"

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

        PointSeries* emaProjector = nullptr;
        PointSeries* sellAboveProjector = nullptr;
        Chart* rsiChart = nullptr;
        PointSeries* rsiProjector = nullptr;

        MartingaleCandleStrategy(): CandleStrategy() {}

        virtual ~MartingaleCandleStrategy() {}

        bool first = true;
        double ema = 0;

        virtual void onCandleClose(Exchange*& exchange, const string& symbol, const Candle& candle) override {            
            ms_t closeAt = candle.getEnd();
            double price = candle.getClose();
            double balanceQuotedFull = exchange->getBalanceQuotedFull(symbol);
            double balanceQuoted = exchange->getBalanceQuoted(symbol);
            double balanceBase = exchange->getBalanceBase(symbol);

            // ema
            if (first) {
                // first cycle only
                emaProjector = candleHistoryChart->createPointSeries(
                    candleHistoryChart->getMainProjector(), true, blue
                );
                sellAboveProjector = balanceQuotedChart->createPointSeries(
                    balanceQuotedChart->getProjectorAt(0), true, darkGray
                );

                // multichartAccordion->closeAll(false);
                rsiChart = multichartAccordion->createChart("RSI", 200);
                rsiProjector = rsiChart->createPointSeries();
                // multichartAccordion->openAll(false);
                // candleHistoryChart->setHeight(200);

                ema = price;
                first = false;
                reinit(price, closeAt);
                return;
            }
            const double emaLength = 2000;
            ema = (ema * emaLength + price) / (emaLength + 1);
            emaProjector->getShapes().push_back(
                candleHistoryChart->createPointShape(closeAt, ema)
            );

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
