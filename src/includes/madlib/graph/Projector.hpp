#pragma once

#include "Shape.hpp"

namespace madlib::graph {
    
    class Projector {
    protected:

        ms_t chartBegin = 0;
        ms_t chartEnd = 0;
        int chartHeight = 0;
        int chartWidth = 0;
        double shapesValueMin = 0;
        double shapesValueMax = 0;
        ms_t chartInterval = 0;
        double shapesValueDiff = 0;
        size_t shapeIndexFrom = 0;
        size_t shapeIndexTo = 0;
        
        bool prepared = false;

        TimeRangeArea& timeRangeArea;
        vector<Shape*> shapes;

    public:

        explicit Projector(
            TimeRangeArea& timeRangeArea
        ): 
            timeRangeArea(timeRangeArea)
        {}

        bool isPrepared() const {
            return prepared;
        }

        void setPrepared(bool prepared) {
            this->prepared = prepared;
        }

        TimeRangeArea& getTimeRangeArea() const {
            return timeRangeArea;
        }

        vector<Shape*>& getShapes() {
            return shapes;
        }

        virtual void project() {
            throw ERR_UNIMP;
        }
    

        int translateX(ms_t ms) const {
            return (int)( 
                ((ms - chartBegin) * (ms_t)chartWidth) / 
                chartInterval 
            );
        }

        int translateY(double value) const {
            return (int)( 
                ((value - shapesValueMin) * chartHeight) /
                shapesValueDiff   
            );
        }

        Pixel translate(ms_t ms, double value) const {
            return Pixel(translateX(ms), translateY(value));
        }

        bool align(Projector* other = nullptr, bool extends = true) {
            if (prepared) return true;
            if (shapes.size() == 0) return false;
            if (other && !other->prepared && !other->align(nullptr, extends)) other = nullptr;

            chartHeight = timeRangeArea.getHeight() - 
                (timeRangeArea.getMargin()->top + timeRangeArea.getMargin()->bottom);
            chartWidth = timeRangeArea.getWidth() - 
                (timeRangeArea.getMargin()->left + timeRangeArea.getMargin()->right);

            if (other) {
                timeRangeArea.getTimeRange().begin = min(
                    timeRangeArea.getTimeRange().begin,
                    other->timeRangeArea.getTimeRange().begin
                );
                timeRangeArea.getTimeRange().end = max(
                    timeRangeArea.getTimeRange().end,
                    other->timeRangeArea.getTimeRange().end
                );
            }
            chartBegin = timeRangeArea.getTimeRange().begin;
            chartEnd = timeRangeArea.getTimeRange().end;
            chartInterval = chartEnd - chartBegin;
            if (other) {
                other->chartBegin = chartBegin;
                other->chartEnd = chartEnd;
                other->chartInterval = chartInterval;
            }
            
            shapesValueMin = INFINITY;
            shapesValueMax = -INFINITY;
            if (other && extends) {
                shapesValueMin = other->shapesValueMin;
                shapesValueMax = other->shapesValueMax;
            }

            // TODO: binary search to get the first index
            shapeIndexFrom = __SIZE_MAX__;
            const size_t shapesLastIndex = shapes.size() - 1;
            for (
                shapeIndexTo = guessFirstShapeIndex(shapesLastIndex);
                shapeIndexTo < shapesLastIndex;
                shapeIndexTo++
            ) {
                Shape* shape = shapes[shapeIndexTo];

                TimeRange shapeTimeRange = shape->getTimeRange();
                if (shapeTimeRange.end < chartBegin) continue;
                if (shapeTimeRange.begin > chartEnd) break;

                if (shapeIndexFrom == __SIZE_MAX__) shapeIndexFrom = shapeIndexTo;

                MinMax<double> valueMinMax = shape->getValueMinMax();
                if (shapesValueMin > valueMinMax.min) shapesValueMin = valueMinMax.min;
                if (shapesValueMax < valueMinMax.max) shapesValueMax = valueMinMax.max;

            }
            if (shapeIndexFrom == __SIZE_MAX__) return false;

            shapesValueDiff = shapesValueMax - shapesValueMin;
            if (other && extends) {
                other->shapesValueMin = shapesValueMin;
                other->shapesValueMax = shapesValueMax;
                other->shapesValueDiff = shapesValueDiff;
            }
            
            return prepared = true;
        }

        // Recursive binary search with linear interpolation, 
        // assuming a uniform distribution for evenly distributed time series data.
        size_t guessFirstShapeIndex(size_t shapesLastIndex) const {
            double shape0Begin = (double)shapes[0]->getTimeRange().begin;
            long guess = (long)(
                (((double)chartBegin - shape0Begin) * (double)shapesLastIndex) / 
                ((double)shapes[shapesLastIndex]->getTimeRange().end - shape0Begin)
            ) - 1;
            if (guess < 1) return 0;
            size_t guessedFirstShapeIndex = (size_t)guess;
            if (guessedFirstShapeIndex >= shapesLastIndex) return shapesLastIndex;
            if (shapes[guessedFirstShapeIndex]->getTimeRange().end >= chartBegin) 
                return guessFirstShapeIndex(guessedFirstShapeIndex);
            return guessedFirstShapeIndex;
        }

        virtual void clearShapes() {
            shapes.clear();
            prepared = false;
        }
    };

}