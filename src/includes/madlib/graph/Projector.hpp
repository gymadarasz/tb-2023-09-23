#pragma once

#include "Shape.hpp"
#include "Chart.hpp"

namespace madlib::graph {

    class Projector {
    public:

        struct Canvas {
            double shapesValueMin = 0;
            double shapesValueMax = 0;
            double shapesValueDiff = 0;
            size_t shapeIndexFrom = 0;
            size_t shapeIndexTo = 0;
            int chartHeight = 0;
            int chartWidth = 0;
            ms_t chartBegin = 0;
            ms_t chartEnd = 0;
            ms_t chartInterval = 0;
            bool stretched = false;

            bool isAlignedWith(const Canvas& other) {
                return 
                    shapesValueMin == other.shapesValueMin &&
                    shapesValueMax == other.shapesValueMax &&
                    shapesValueDiff == other.shapesValueDiff &&
                    chartHeight == other.chartHeight &&
                    chartWidth == other.chartWidth &&
                    chartBegin == other.chartBegin &&
                    chartEnd == other.chartEnd &&
                    chartInterval == other.chartInterval;

            }
        };

    protected:        

        Canvas canvas;
        
        // bool prepared = false;

        TimeRangeArea* timeRangeArea = nullptr;
        vector<Shape*> shapes;

    public:

        explicit Projector(
            TimeRangeArea* timeRangeArea
        ): 
            timeRangeArea(timeRangeArea)
        {}

        // bool isPrepared() const {
        //     return prepared;
        // }

        // void setPrepared(bool prepared) {
        //     this->prepared = prepared;
        // }

        Canvas& getCanvas() {
            return canvas;
        }

        TimeRangeArea* getTimeRangeArea() const {
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
                ((ms - canvas.chartBegin) * (ms_t)canvas.chartWidth) / 
                canvas.chartInterval 
            );
        }

        int translateY(double value) const {
            return (int)( 
                ((value - canvas.shapesValueMin) * canvas.chartHeight) /
                canvas.shapesValueDiff   
            );
        }

        Pixel translate(ms_t ms, double value) const {
            return Pixel(translateX(ms), translateY(value));
        }

        // bool align(Projector* other, bool extends = true) {
        //     if (prepared) return true;
        //     if (shapes.size() == 0) return prepared = true;
        //     if (!other || !other->prepared) return prepared = false; // && !other->align(nullptr, extends)) other = nullptr;
        //     if (other) other->prepared = false;

        //     chartHeight = timeRangeArea->getHeight() - 
        //         (timeRangeArea->getMargin().top + timeRangeArea->getMargin().bottom);
        //     chartWidth = timeRangeArea->getWidth() - 
        //         (timeRangeArea->getMargin().left + timeRangeArea->getMargin().right);

        //     if (other) {
        //         timeRangeArea->getTimeRange()->begin = min(
        //             timeRangeArea->getTimeRange()->begin,
        //             other->timeRangeArea->getTimeRange()->begin
        //         );
        //         timeRangeArea->getTimeRange()->end = max(
        //             timeRangeArea->getTimeRange()->end,
        //             other->timeRangeArea->getTimeRange()->end
        //         );
        //     }
        //     chartBegin = timeRangeArea->getTimeRange()->begin;
        //     chartEnd = timeRangeArea->getTimeRange()->end;
        //     chartInterval = chartEnd - chartBegin;
        //     if (other) {
        //         other->chartBegin = chartBegin;
        //         other->chartEnd = chartEnd;
        //         other->chartInterval = chartInterval;
        //     }
            
        //     shapesValueMin = INFINITY;
        //     shapesValueMax = -INFINITY;
        //     if (other && extends) {
        //         shapesValueMin = other->shapesValueMin;
        //         shapesValueMax = other->shapesValueMax;
        //     }

        //     // TODO: binary search to get the first index
        //     shapeIndexFrom = __SIZE_MAX__;
        //     const size_t shapesLastIndex = shapes.size() - 1;
        //     for (
        //         shapeIndexTo = guessFirstShapeIndex(shapesLastIndex);
        //         shapeIndexTo < shapesLastIndex;
        //         shapeIndexTo++
        //     ) {
        //         Shape* shape = shapes[shapeIndexTo];

        //         TimeRange shapeTimeRange = shape->getTimeRange();
        //         if (shapeTimeRange.end < chartBegin) continue;
        //         if (shapeTimeRange.begin > chartEnd) break;

        //         if (shapeIndexFrom == __SIZE_MAX__) shapeIndexFrom = shapeIndexTo;

        //         MinMax<double> valueMinMax = shape->getValueMinMax();
        //         if (shapesValueMin > valueMinMax.min) shapesValueMin = valueMinMax.min;
        //         if (shapesValueMax < valueMinMax.max) shapesValueMax = valueMinMax.max;

        //     }
        //     if (shapeIndexFrom == __SIZE_MAX__) return false;

        //     shapesValueDiff = shapesValueMax - shapesValueMin;
        //     if (other && extends) {
        //         other->shapesValueMin = shapesValueMin;
        //         other->shapesValueMax = shapesValueMax;
        //         other->shapesValueDiff = shapesValueDiff;
        //     }
            
        //     return prepared = true;
        // }

        void calculateCanvasEdges() {
            canvas.chartHeight = timeRangeArea->getHeight() - 
                (timeRangeArea->getMargin().top + timeRangeArea->getMargin().bottom);
            canvas.chartWidth = timeRangeArea->getWidth() - 
                (timeRangeArea->getMargin().left + timeRangeArea->getMargin().right);

            canvas.chartBegin = timeRangeArea->getTimeRange()->begin;
            canvas.chartEnd = timeRangeArea->getTimeRange()->end;
            canvas.chartInterval = canvas.chartEnd - canvas.chartBegin;
        }

        bool searchShapeIndexFromToAndValueMinMax() {
            // TODO: binary search to get the first index
            canvas.shapesValueMin = INFINITY;
            canvas.shapesValueMax = -INFINITY;
            canvas.shapeIndexFrom = __SIZE_MAX__;
            canvas.shapeIndexTo = 0;
            canvas.stretched = false;
            if (shapes.size() == 0) return false;
            const size_t shapesLastIndex = shapes.size() - 1;
            for (
                canvas.shapeIndexTo = guessFirstShapeIndex(shapesLastIndex);
                canvas.shapeIndexTo < shapesLastIndex;
                canvas.shapeIndexTo++
            ) {
                Shape* shape = shapes[canvas.shapeIndexTo];

                TimeRange shapeTimeRange = shape->getTimeRange();
                if (shapeTimeRange.end < canvas.chartBegin) continue;
                if (shapeTimeRange.begin > canvas.chartEnd) break;

                if (canvas.shapeIndexFrom == __SIZE_MAX__) canvas.shapeIndexFrom = canvas.shapeIndexTo;

                MinMax<double> valueMinMax = shape->getValueMinMax();
                if (canvas.shapesValueMin > valueMinMax.min) canvas.shapesValueMin = valueMinMax.min;
                if (canvas.shapesValueMax < valueMinMax.max) canvas.shapesValueMax = valueMinMax.max;

            }
            if (canvas.shapeIndexFrom == __SIZE_MAX__) return false; //canvas.shapeIndexTo = 0;

            canvas.shapesValueDiff = canvas.shapesValueMax - canvas.shapesValueMin;
            return canvas.stretched = true;
        }

        // Recursive binary search with linear interpolation, 
        // assuming a uniform distribution for evenly distributed time series data.
        size_t guessFirstShapeIndex(size_t shapesLastIndex) const {
            double shape0Begin = (double)shapes[0]->getTimeRange().begin;
            long guess = (long)(
                (((double)canvas.chartBegin - shape0Begin) * (double)shapesLastIndex) / 
                ((double)shapes[shapesLastIndex]->getTimeRange().end - shape0Begin)
            ) - 1;
            if (guess < 1) return 0;
            size_t guessedFirstShapeIndex = (size_t)guess;
            if (guessedFirstShapeIndex >= shapesLastIndex) return shapesLastIndex;
            if (shapes[guessedFirstShapeIndex]->getTimeRange().end >= canvas.chartBegin) 
                return guessFirstShapeIndex(guessedFirstShapeIndex);
            return guessedFirstShapeIndex;
        }

        virtual void clearShapes() {
            shapes.clear();
            // prepared = false;
        }
    };

}