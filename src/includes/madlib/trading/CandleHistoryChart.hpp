#pragma once

#include "../graph/Chart.hpp"
#include "CandleHistory.hpp"

namespace madlib::trading {
    
    class CandleHistoryChart: public Chart {
    protected:
        CandleHistory*& candleHistory;
        // LabelSeries* labelSeries = nullptr;
        const Color priceColor;
        const Color volumeColor;

        Projector* mainProjector = nullptr;
        CandleSeries* candleSeries = nullptr;
        PointSeries* priceSeries = nullptr;
        PointSeries* volumeSeries = nullptr;
        LabelSeries* labelSeries = nullptr;

    public:

        CandleHistoryChart(
            GFX* gfx, int left, int top, int width, int height,
            // ms_t timeRangeBegin, ms_t timeRangeEnd,
            CandleHistory*& candleHistory,
            // LabelSeries* labelSeries = nullptr, // TODO: add labelTexts
            const bool showCandles = true, // TODO
            const bool showPrices = true, // TODO
            const bool showVolumes = true, // TODO
            const bool showTexts = true, // TODO
            const bool generate = true,
            const Color priceColor = Theme::defaultTradeHistoryChartPriceColor,
            const Color volumeColor = Theme::defaultTradeHistoryChartVolumeColor,
            void* eventContext = nullptr
        ):  
            Chart(
                gfx, left, top, width, height,
                candleHistory->getStartTime(), 
                candleHistory->getEndTime(),
                Theme::defaultChartBorder,
                Theme::defaultChartBackgroundColor,
                Theme::defaultChartBorderColor,
                eventContext
            ),
            candleHistory(candleHistory),
            priceColor(priceColor),
            volumeColor(volumeColor) //,
            // labelSeries(labelSeries)
        {
            init(
                showCandles,
                showPrices,
                showVolumes,
                showTexts,
                generate
            );
        }

        void init(
            const bool showCandles,
            const bool showPrices,
            const bool showVolumes,
            const bool showTexts,
            const bool generate
        ) {
            if (showCandles)
                mainProjector = candleSeries = createCandleSeries();
            if (showPrices) {
                priceSeries = createPointSeries(
                    mainProjector, priceColor
                );
            }
            if (showVolumes)
                volumeSeries = createPointSeries(nullptr, volumeColor);
            if (showTexts)
                labelSeries = createLabelSeries(mainProjector);
            
            if (generate) {
                generateFromHistory();
                fitTimeRangeToHistory();
            }
        }

        virtual ~CandleHistoryChart() {}

        Projector* getMainProjector() {
            return mainProjector;
        }

        LabelSeries* getLabelSeries() const {
            return labelSeries;
        }

        void generateFromHistory() {
            
            if (candleSeries) {
                const vector<Candle>& candles = candleHistory->getCandles();
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
                const vector<Trade>& trades = candleHistory->getTrades();
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

            // if (
            //     mainProjector && labelSeries && labelSeries &&
            //     labelSeries != labelSeries
            // ) {
            //     vector<Shape*>& textShapes = labelSeries->getShapes();
            //     for (Shape* shape: labelSeries->getShapes()) {
            //         LabelShape* tradeText = (LabelShape*)shape;
            //         textShapes.push_back(createLabelShape(
            //             tradeText->time(), 
            //             tradeText->value(),
            //             tradeText->text(),
            //             tradeText->color(),
            //             tradeText->backgroundColor(),
            //             tradeText->padding(),
            //             tradeText->hasBackground()
            //         ));
            //     }
            // }
        }

        void fitTimeRangeToHistory() {
            // apply new full time range aligned to the candle history
            TimeRange newTimeRangeFull(
                candleHistory->getStartTime(),
                candleHistory->getEndTime()
            );
            setTimeRangeFullAndApply(newTimeRangeFull);
        }

        virtual void clearProjectors() override {
            Chart::clearProjectors();
            generateFromHistory();
            fitTimeRangeToHistory();
        }
    };

}