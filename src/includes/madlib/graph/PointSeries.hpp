#pragma once

#include "defs.hpp"
#include "TimeRangeArea.hpp"
#include "Color.hpp"
#include "Projector.hpp"
#include "PointShape.hpp"

namespace madlib::graph {
    
    class PointSeries: public Projector {
    protected:
    
        const Color color;

        void projectFirstLastValue() {
            string text;
            
            const PointShape* first = (const PointShape*)shapes[canvas.shapeIndexFrom];
            text = to_string(first->value());
            TextSize textSize = timeRangeArea->getTextSize(text);
            timeRangeArea->write(
                -textSize.width, 
                canvas.chartHeight - translateY(first->value()), 
                text
            );

            const PointShape* last = (const PointShape*)shapes[canvas.shapeIndexTo];
            text = to_string(last->value());
            timeRangeArea->write(
                canvas.chartWidth, 
                canvas.chartHeight - translateY(last->value()), 
                text
            );
        }

    public:

        explicit PointSeries(
            TimeRangeArea* timeRangeArea,
            const Color color = Theme::defaultChartSeriesColor
        ):
            Projector(timeRangeArea),
            color(color)
        {}

        virtual ~PointSeries() {}

        virtual void project() override {
            const PointShape* first = (const PointShape*)shapes[canvas.shapeIndexFrom];

            Pixel prev = translate(
                first->time(), 
                first->value()
            );
            timeRangeArea->brush(color);
            
            size_t step = (canvas.shapeIndexTo - canvas.shapeIndexFrom) / (size_t)canvas.chartWidth;
            if (step < 1) step = 1;
            for (size_t i = canvas.shapeIndexFrom; i < canvas.shapeIndexTo; i += step) {
                const PointShape* point = (const PointShape*)shapes[i];
                Pixel pixel = translate(
                    point->time(), 
                    point->value()
                );

                timeRangeArea->line(
                    prev.x, canvas.chartHeight - prev.y, 
                    pixel.x, canvas.chartHeight - pixel.y
                );
                prev = pixel;
            }

            projectFirstLastValue();
        }
    };

}