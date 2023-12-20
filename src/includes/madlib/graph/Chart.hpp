#pragma once

#include "../vectors.hpp"

#include "TimeRangeArea.hpp"
#include "Projector.hpp"
#include "Alignment.hpp"
#include "PointSeries.hpp"
#include "CandleSeries.hpp"
#include "LabelSeries.hpp"
#include "PointShape.hpp"
#include "CandleShape.hpp"
#include "LabelShape.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;

namespace madlib::graph {

    class Chart: public TimeRangeArea {
    public:

        class MultiChart {
        protected:

            vector<Chart*> charts;

        public:

            MultiChart() {}

            virtual ~MultiChart() {}

            void attach(Chart* chart) {
                charts.push_back(chart);
                chart->join(this);
                follow(chart);
            }

            void follow(Chart* that) {
                for (Chart* chart: charts)
                    if (chart != that) {
                        bool changed = false;
                        if (chart->getTimeRangeFull()->expand(*that->getTimeRangeFull())) changed = true;
                        if (chart->getTimeRange()->apply(*that->getTimeRange())) changed = true;
                        if (chart->getTimeRange()->limit(*chart->getTimeRangeFull())) changed = true;
                        if (changed) {
                            chart->resetProjectorsPrepared();
                            chart->draw();
                        }
                    }
            }

        };

    protected:

        MultiChart* multiChart = nullptr;

        bool dragTimeRange = false;
        int dragTimeRangeStartedX = 0;

        static void zoomHandler(void* context, unsigned int button, int x, int) {
            if (
                button != Theme::zoomInScrollButton &&
                button != Theme::zoomOutScrollButton
            ) return;

            Chart* chart = (Chart*)context;
            bool zoomAtLeft = true;
            bool zoomAtRight = true;
            if (x <= chart->width * .2) zoomAtRight = false;
            if (x >= chart->width * .8) zoomAtLeft = false;
            if (chart->timeRange->begin < chart->timeRangeFull->begin) zoomAtLeft = true;
            if (chart->timeRange->end > chart->timeRangeFull->end) zoomAtRight = true;
            if (button == Theme::zoomOutScrollButton) {
                if (chart->timeRange->begin == chart->timeRangeFull->begin) zoomAtLeft = true;
                if (chart->timeRange->end == chart->timeRangeFull->end) zoomAtRight = true;
            }
            
            double origDiff = (double)(chart->timeRange->end - chart->timeRange->begin);
            double interval = origDiff;
            if (button == Theme::zoomInScrollButton) interval *= Theme::zoomInRatio;
            if (button == Theme::zoomOutScrollButton) interval *= Theme::zoomOutRatio;
            double diff = origDiff - interval;

            if (zoomAtLeft) chart->timeRange->end += (ms_t)diff;                
            if (zoomAtRight) chart->timeRange->begin -= (ms_t)diff;

            chart->timeRange->limit(*chart->getTimeRangeFull());
            chart->resetProjectorsPrepared();
            chart->draw();
            if (chart->multiChart) chart->multiChart->follow(chart);
        }

        void resetProjectorsPrepared() const {
            for (Projector* projector: projectors) 
                projector->setPrepared(false);
        }

        static void dragStartHandler(void* context, unsigned int button, int x, int) {
            if (button != Theme::touchButton) return;
            Chart* chart = (Chart*)context;
            chart->dragTimeRange = true;
            chart->dragTimeRangeStartedX = x;
        }

        static void dragMoveHandler(void* context, int x, int) {
            Chart* chart = (Chart*)context;
            if (!chart->dragTimeRange) return;
            ms_t diff = chart->retractX(x - chart->dragTimeRangeStartedX);
            chart->timeRange->begin -= diff;
            chart->timeRange->end -= diff;
            chart->dragTimeRangeStartedX = x;

            chart->getTimeRange()->limit(*chart->getTimeRangeFull());
            chart->resetProjectorsPrepared();
            chart->draw();
            if (chart->multiChart) chart->multiChart->follow(chart);
        }

        static void dragStopHandler(void* context, unsigned int button, int, int) {
            if (button != Theme::touchButton) return;
            Chart* chart = (Chart*)context;
            chart->dragTimeRange = false;
        }

    
        vector<Projector*> projectors;
        vector<Alignment> alignments;

        vector<PointSeries*> pointSeriesProjectors;
        vector<CandleSeries*> candleSeriesProjectors;
        vector<LabelSeries*> labelSeriesProjectors;

        vector<PointShape*> pointShapes;
        vector<CandleShape*> candleShapes;
        vector<LabelShape*> labelShapes;
        
        void drawTimeRange() {
            TextSize textSize;
            brush(gray);
            const string begin = ms_to_datetime(timeRange->begin);
            textSize = getTextSize(begin);
            write(
                -textSize.width/2, 
                height - textSize.height, 
                begin
            );
            const string end = ms_to_datetime(timeRange->end);
            textSize = getTextSize(end);
            write(
                width - margin.left - margin.right -textSize.width/2, 
                height - textSize.height, 
                end
            );
        }

    public:

        Chart(
            GFX* gfx, 
            int left, int top, int width, int height,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            const Border border = Theme::defaultChartBorder,
            const Color backgroundColor = Theme::defaultChartBackgroundColor,
            const Color borderColor = Theme::defaultChartBorderColor,
            void* eventContext = nullptr
        ):
            TimeRangeArea(
                gfx, left, top, width, height,
                timeRangeBegin, timeRangeEnd,
                border, backgroundColor, borderColor,
                eventContext
            )
        {
            addTouchHandler(Chart::zoomHandler);
            addTouchHandler(Chart::dragStartHandler);
            addMoveHandler(Chart::dragMoveHandler);
            addReleaseHandler(Chart::dragStopHandler);
        }

        virtual ~Chart() {
            vector_destroy(pointSeriesProjectors);
            vector_destroy(candleSeriesProjectors);
            vector_destroy(labelSeriesProjectors);
            projectors.clear();

            vector_destroy(pointShapes);
            vector_destroy(candleShapes);
            vector_destroy(labelShapes);
            alignments.clear();
        }

        Projector* getProjectorAt(size_t at) {
            return projectors.size() > at ? projectors.at(at) : nullptr;
        }

        void setTimeRangeFullAndApply(const TimeRange& newTimeRangeFull) {
            timeRangeFull->apply(newTimeRangeFull);
            timeRange->apply(*timeRangeFull);
        }

        virtual void clearProjectors() {
            vector_destroy(pointShapes);
            for (PointSeries* pointSeriesProjector: pointSeriesProjectors)
                pointSeriesProjector->clearShapes();
            vector_destroy(candleShapes);
            for (CandleSeries* candleSeriesProjector: candleSeriesProjectors)
                candleSeriesProjector->clearShapes();
            vector_destroy(labelShapes);
            for (LabelSeries* labelSeriesProjector: labelSeriesProjectors)
                labelSeriesProjector->clearShapes();
        };

        void join(MultiChart* multiChart) {
            this->multiChart = multiChart;
        }

        virtual void draw() override {
            TimeRangeArea::draw();
        
            for (const Alignment& alignment: alignments)
                alignment.getProjector()->align(
                    alignment.getAlignToProjector(), 
                    alignment.isExtends()
                );

            for (Projector* projector: projectors)
                if (!projector) continue;
                else if (!projector->isPrepared()) continue;
                else projector->project();

            drawTimeRange();
        }

        virtual PointSeries* createPointSeries(
            Projector* alignToProjector = nullptr,
            bool alignExtends = true,
            Color color = Theme::defaultChartSeriesColor
        ) {
            PointSeries* pointSeries = new PointSeries(
                this, color
            );
            pointSeriesProjectors.push_back(pointSeries);
            // alignToProjector = alignToProjector
            //     ? alignToProjector
            //     : projectors.size() > 0 ? projectors[0] : nullptr;
            projectors.push_back(pointSeries);
            alignments.push_back(Alignment(
                pointSeries, 
                alignToProjector, 
                alignExtends
            ));
            return pointSeries;
        }

        virtual CandleSeries* createCandleSeries(
            Projector* alignToProjector = nullptr,
            bool alignExtends = true,
            Color colorUp = Theme::defaultChartCandleColorUp, 
            Color colorDown = Theme::defaultChartCandleColorDown
        ) {
            CandleSeries* candleSeries = new CandleSeries(
                this, colorUp, colorDown
            );
            candleSeriesProjectors.push_back(candleSeries);
            projectors.push_back(candleSeries);
            alignments.push_back(Alignment(candleSeries, alignToProjector, alignExtends));
            return candleSeries;
        }

        virtual LabelSeries* createLabelSeries(
            Projector* alignToProjector = nullptr,
            bool alignExtends = true
        ) {
            LabelSeries* labelSeries = new LabelSeries(this);
            labelSeriesProjectors.push_back(labelSeries);
            projectors.push_back(labelSeries);
            alignments.push_back(Alignment(labelSeries, alignToProjector, alignExtends));
            return labelSeries;
        }

        PointShape* createPointShape(
            ms_t time, 
            double value
        ) {
            PointShape* pointShape = new PointShape(
                time, value
            );
            pointShapes.push_back(pointShape);
            return pointShape;
        }

        CandleShape* createCandleShape(
            ms_t begin,
            ms_t end,
            double open,
            double low,
            double high,
            double close
        ) {
            CandleShape* candleShape = new CandleShape(
                begin, end, open, low, high, close
            );
            candleShapes.push_back(candleShape);
            return candleShape;
        }

        LabelShape* createLabelShape(
            ms_t time,
            double value,
            const string& text,
            const Color color = Theme::defaultChartLabelColor,
            const Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
            const int padding = Theme::defaultChartLabelPadding,
            const bool hasBackground = Theme::defaultChartLabelHasBackground
        ) {
            LabelShape* labelShape = new LabelShape(
                time, value, text, 
                color, backgroundColor,
                padding, hasBackground
            );
            labelShapes.push_back(labelShape);
            return labelShape;
        }
    };

}