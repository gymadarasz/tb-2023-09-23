#pragma once

#include "CandleShape.hpp"

namespace madlib::graph {
    
    class CandleSeries: public Projector {
    protected:
    
        const Color colorUp;
        const Color colorDown;

        void projectFirstLastValue() {
            string text;

            const CandleShape* first = (const CandleShape*)shapes[canvas.shapeIndexFrom];
            timeRangeArea->brush(
                first->open() > first->close() ? colorDown : colorUp
            );
            text = to_string(first->open());
            TextSize textSize = timeRangeArea->getTextSize(text);
            timeRangeArea->write(
                -textSize.width, 
                canvas.chartHeight - translateY(first->open()), 
                text
            );

            const CandleShape* last = (const CandleShape*)shapes[canvas.shapeIndexTo];
            timeRangeArea->brush(
                last->open() > last->close() ? colorDown : colorUp
            );
            text = to_string(last->close());
            timeRangeArea->write(
                canvas.chartWidth,
                canvas.chartHeight - translateY(last->close()), 
                text
            );
        }

    public:

        explicit CandleSeries(
            TimeRangeArea* timeRangeArea,
            const Color colorUp = Theme::defaultChartCandleColorUp,
            const Color colorDown = Theme::defaultChartCandleColorDown
        ):
            Projector(timeRangeArea),
            colorUp(colorUp),
            colorDown(colorDown)
        {}

        virtual ~CandleSeries() {}

        virtual void project() override {
            size_t step = (canvas.shapeIndexTo - canvas.shapeIndexFrom) / (size_t)canvas.chartWidth;
            double prevClose = 0;
            if (step < 1) step = 1;
            for (size_t i = canvas.shapeIndexFrom; i < canvas.shapeIndexTo; i += step) {
                const CandleShape* candle = (const CandleShape*)shapes[i];

                if (step > 1) { 
                    double candleClose = candle->close();
                    if (!prevClose) prevClose = candleClose;
                    Color color = prevClose > candleClose ? colorDown : colorUp;
                    timeRangeArea->brush(color);
                    int x = translateX(candle->end());
                    int y1 = canvas.chartHeight - translateY(prevClose);
                    int y2 = canvas.chartHeight - translateY(candleClose);
                    timeRangeArea->vLine(x, y1, y2);
                    prevClose = candleClose;
                    continue;
                }

                Color color = candle->open() > candle->close() ? colorDown : colorUp;
                timeRangeArea->brush(color);

                const ms_t mid = candle->begin() + (candle->end() - candle->begin()) / 2;
                int wick = translateX(mid);
                int high = translateY(candle->high());
                int low = translateY(candle->low());

                timeRangeArea->vLine(
                    wick, 
                    canvas.chartHeight - high, 
                    canvas.chartHeight - low
                );
                
                int left = translateX(candle->begin());
                int right = translateX(candle->end());
                int diff = right - left;
                if (diff > 3) {
                    left += 1;
                    right -= 1;
                }
                int open = translateY(candle->open());
                int close = translateY(candle->close());
                timeRangeArea->fRect(
                    left, canvas.chartHeight - open, 
                    right, canvas.chartHeight - close
                );
            }

            projectFirstLastValue();
        }
    };

}