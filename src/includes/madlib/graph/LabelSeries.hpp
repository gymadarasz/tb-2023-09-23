#pragma once

#include "LabelShape.hpp"

namespace madlib::graph {
    
    class LabelSeries: public Projector {
    protected:
        const size_t maxLabelsToShow = 50; // TODO
    public:

        using Projector::Projector;

        virtual ~LabelSeries() {}

        virtual void project() override {
            if (shapeIndexTo - shapeIndexFrom > maxLabelsToShow) return;
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i++) {
                const LabelShape* label = (const LabelShape*)shapes[i];
                int x = translateX(label->time());
                int y = translateY(label->value());
                TextSize textSize = timeRangeArea.getTextSize(label->text());
                if (label->hasBackground()) {
                    int padding = label->padding();
                    int x1 = x - padding;
                    int y1 = chartHeight - y - padding;
                    int x2 = x + textSize.width + padding;
                    int y2 = chartHeight - (y - textSize.height) + padding;
                    timeRangeArea.brush(label->backgroundColor());
                    timeRangeArea.fRect(x1, y1, x2, y2);
                    timeRangeArea.brush(label->color());
                    timeRangeArea.rect(x1, y1, x2, y2);
                }
                timeRangeArea.brush(label->color());
                timeRangeArea.write(x, chartHeight - y, label->text());
            }
        }
    };


}