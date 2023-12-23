#pragma once

#include "../../graph/PointSeries.hpp"
#include "../../graph/Color.hpp"
#include "../CandleHistoryChart.hpp"

namespace madlib::trading {

    class EmaIndicator {
    protected:
        CandleHistoryChart* candleHistoryChart = nullptr;
        PointSeries* emaProjector = nullptr;
        double ema = 0;
        double length = 0;
    public:
        EmaIndicator(
            CandleHistoryChart* candleHistoryChart,
            double ema,
            double length,
            Color color
        ):
            candleHistoryChart(candleHistoryChart),
            ema(ema),
            length(length)
        { // TODO: bug: alignment doesn't works on candle main chart
            Projector* mainProjector = candleHistoryChart->getMainProjector();
            if (!mainProjector) mainProjector = candleHistoryChart->getProjectorAt(0);
            emaProjector = candleHistoryChart->createPointSeries(
                mainProjector, color
            );
            DBG("emaProjector (" + to_string((unsigned long long)emaProjector) + ") should align to: " + to_string((unsigned long long)mainProjector));
        }

        virtual ~EmaIndicator() {}

        void calc(ms_t time, double value) {
            ema = (ema * length + value) / (length + 1);

            emaProjector->getShapes().push_back(
                candleHistoryChart->createPointShape(time, ema)
            );
        }

    };

}