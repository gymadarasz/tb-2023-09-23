#pragma once

#include "PointShape.hpp"

namespace madlib::graph {
    
    class PointSeries: public Projector {
    protected:
    
        const Color color;

        void projectFirstLastValue() {
            string text;
            
            const PointShape* first = (const PointShape*)shapes[shapeIndexFrom];
            text = to_string(first->value());
            TextSize textSize = timeRangeArea.getTextSize(text);
            timeRangeArea.write(
                -textSize.width, 
                chartHeight - translateY(first->value()), 
                text
            );

            const PointShape* last = (const PointShape*)shapes[shapeIndexTo];
            text = to_string(last->value());
            timeRangeArea.write(
                chartWidth, 
                chartHeight - translateY(last->value()), 
                text
            );
        }

    public:

        explicit PointSeries(
            TimeRangeArea& area,
            const Color color = Theme::defaultChartSeriesColor
        ):
            Projector(area),
            color(color)
        {}

        virtual ~PointSeries() {}

        virtual void project() override {
            const PointShape* first = (const PointShape*)shapes[shapeIndexFrom];

            Pixel prev = translate(
                first->time(), 
                first->value()
            );
            timeRangeArea.brush(color);
            
            size_t step = (shapeIndexTo - shapeIndexFrom) / (size_t)chartWidth;
            if (step < 1) step = 1;
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i += step) {
                const PointShape* point = (const PointShape*)shapes[i];
                Pixel pixel = translate(
                    point->time(), 
                    point->value()
                );

                timeRangeArea.line(
                    prev.x, chartHeight - prev.y, 
                    pixel.x, chartHeight - pixel.y
                );
                prev = pixel;
            }

            projectFirstLastValue();
        }
    };

}