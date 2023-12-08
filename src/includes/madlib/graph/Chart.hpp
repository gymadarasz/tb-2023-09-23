#pragma once

#include "graph.hpp"
#include "../madlib.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;

namespace madlib::graph {

    template<typename T>
    class Range {
    public:
        T begin, end;
        Range(T begin, T end): begin(begin), end(end) {}
    };

    template<typename T>
    class MinMax {
    public:
        T min, max;
        MinMax(T min, T max): min(min), max(max) {}
    };

    class TimeRange: public Range<ms_t> {
    public:
        TimeRange(
            ms_t begin, 
            ms_t end
        ):
            Range(begin, end)
        {}
    };

    class Shape {
    protected:

        const TimeRange timeRange;
        const MinMax<double> valueMinMax;

    public:

        Shape(
            const TimeRange timeRange,
            const MinMax<double> valueMinMax
        ):
            timeRange(timeRange),
            valueMinMax(valueMinMax)
        {}

        const TimeRange& getTimeRange() const {
            return timeRange;
        }

        const MinMax<double> getValueMinMax() const {
            return valueMinMax;
        }
    };

    class PointShape: public Shape {
    public:

        PointShape(
            ms_t time, 
            double value
        ):
            Shape(
                TimeRange(time, time), 
                MinMax<double>(value, value)
            )
        {}

        ms_t time() const {
            return timeRange.begin;
        }

        double value() const {
            return valueMinMax.min;
        }
    };

    class CandleShape: public Shape {
    protected:

        double _open;
        double _close;

    public:

        CandleShape(
            ms_t begin,
            ms_t end,
            double open,
            double low,
            double high,
            double close
        ): 
            Shape(
                TimeRange(begin, end), 
                MinMax<double>(low, high)
            ),
            _open(open),
            _close(close)
        {}

        ms_t begin() const {
            return timeRange.begin;
        }

        ms_t end() const {
            return timeRange.end;
        }

        double open() const {
            return _open;
        }

        double low() const {
            return valueMinMax.min;
        }

        double high() const {
            return valueMinMax.max;
        }

        double close() const {
            return _close;
        }

    };


    class LabelShape: public PointShape {
    protected:

        const string _text;
        const Color _color;
        const Color _backgroundColor;
        const Color _borderColor;
        const int _padding;
        const bool _hasBackground;

    public:

        LabelShape(
            ms_t time,
            double value,
            const string& text,
            const Color color = Theme::defaultChartLabelColor,
            const Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
            const Color borderColor = Theme::defaultChartLabelBorderColor,
            const int padding = Theme::defaultChartLabelPadding,
            const bool hasBackground = Theme::defaultChartLabelHasBackground
        ): 
            PointShape(
                time,
                value
            ),
            _text(text),
            _color(color),
            _backgroundColor(backgroundColor),
            _borderColor(borderColor),
            _padding(padding),
            _hasBackground(hasBackground)
        {}

        const string text() const {
            return _text;
        }

        Color color() const {
            return _color;
        }

        Color backgroundColor() const {
            return _backgroundColor;
        }

        Color borderColor() const {
            return _borderColor;
        }

        int padding() const {
            return _padding;
        }

        bool hasBackground() const {
            return _hasBackground;
        }
    };    


    class TimeRangeArea: public Area {
    protected:

        TimeRange* timeRange = NULL;

    public:
        
        TimeRangeArea(
            GFX &gfx, 
            int left, int top, int width, int height,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            const Border border = Theme::defaultChartBorder,
            const Color backgroundColor = Theme::defaultChartBackgroundColor,
            const Color borderColor = Theme::defaultChartBorderColor,
            void *eventContext = NULL
        ): 
            Area(
                gfx, left, top, width, height, 
                true, true, "", 
                Theme::defaultAreaTextAlign, 
                border, backgroundColor, 
                Theme::defaultAreaMargin,
                0, borderColor, 
                Theme::defaultAreaTextColor, 
                eventContext
            ),
            timeRange(new TimeRange(timeRangeBegin, timeRangeEnd))
        {}

        virtual ~TimeRangeArea() {
            delete timeRange;
        }

        TimeRange& getTimeRange() {
            return *timeRange;
        }
    };


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

        bool align(Projector* other = NULL, bool extends = true) {
            if (!other && shapes.size() == 0) return prepared = false;

            if (other && !other->prepared && !other->align(NULL, extends)) 
                return align(NULL, extends);

            chartHeight = timeRangeArea.getHeight();
            chartWidth = timeRangeArea.getWidth();
            chartBegin = timeRangeArea.getTimeRange().begin;
            chartEnd = timeRangeArea.getTimeRange().end;

            shapesValueMin = INFINITY;
            shapesValueMax = -INFINITY;
            if (other) {
                shapesValueMin = other->shapesValueMin;
                shapesValueMax = other->shapesValueMax;
            }

            shapeIndexFrom = __SIZE_MAX__;
            shapeIndexTo = 0;
            for (const Shape* shape: shapes) {
                TimeRange shapeTimeRange = shape->getTimeRange();
                if (shapeTimeRange.end < chartBegin) continue;
                if (shapeTimeRange.begin > chartEnd) break;

                if (!other || extends) {
                    MinMax<double> valueMinMax = shape->getValueMinMax();
                    if (shapesValueMin > valueMinMax.min) shapesValueMin = valueMinMax.min;
                    if (shapesValueMax < valueMinMax.max) shapesValueMax = valueMinMax.max;
                }

                if (shapeIndexFrom == __SIZE_MAX__) shapeIndexFrom = shapeIndexTo;
                shapeIndexTo++;
            }
            if (shapeIndexFrom == __SIZE_MAX__) return prepared = false;

            chartInterval = chartEnd - chartBegin;
            shapesValueDiff = shapesValueMax - shapesValueMin;

            if (other && extends) {
                other->shapesValueMin = shapesValueMin;
                other->shapesValueMax = shapesValueMax;
                other->getTimeRangeArea().getTimeRange().begin = chartBegin;
                other->getTimeRangeArea().getTimeRange().end = chartEnd;
                other->chartInterval = chartInterval;
                other->shapesValueDiff = shapesValueDiff;
            }


            return prepared = true;
        }
    };

    class Retractor { // TODO: chart to scales
    public:
        virtual void retract(Shape&) {
            throw ERR_UNIMP;
        }
    };

    class PointSeries: public Projector {
    protected:
    
        const Color color;

    public:

        explicit PointSeries(
            TimeRangeArea& area,
            const Color color = Theme::defaultChartSeriesColor
        ):
            Projector(area),
            color(color)
        {}

        virtual ~PointSeries() {}

        void project() override {
            const PointShape* first = (const PointShape*)shapes[shapeIndexFrom];

            Pixel prev = translate(
                first->time(), 
                first->value()
            );
            timeRangeArea.brush(color);
            
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i++) {
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
        }
    };


    class CandleSeries: public Projector {
    protected:
    
        const Color colorUp;
        const Color colorDown;

    public:

        explicit CandleSeries(
            TimeRangeArea& area,
            const Color colorUp = Theme::defaultChartCandleColorUp,
            const Color colorDown = Theme::defaultChartCandleColorDown
        ):
            Projector(area),
            colorUp(colorUp),
            colorDown(colorDown)
        {}

        virtual ~CandleSeries() {}

        void project() override {
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i++) {
                const CandleShape* candle = (const CandleShape*)shapes[i];
                Color color = candle->open() > candle->close() ? colorDown : colorUp;
                timeRangeArea.brush(color);

                const ms_t mid = candle->begin() + (candle->end() - candle->begin()) / 2;
                int wick = translateX(mid);
                int high = translateY(candle->high());
                int low = translateY(candle->low());
                timeRangeArea.vLine(
                    wick, 
                    chartHeight - high, 
                    chartHeight - low
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
                timeRangeArea.fRect(
                    left, chartHeight - open, 
                    right, chartHeight - close
                );
            }
        }
    };

    class LabelSeries: public Projector {
    public:

        using Projector::Projector;

        virtual ~LabelSeries() {}

        void project() override {
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i++) {
                const LabelShape* label = (const LabelShape*)shapes[i];
                int x = translateX(label->time());
                int y = translateY(label->value());
                Painter::TextSize textSize = timeRangeArea.getTextSize(label->text());
                if (label->hasBackground()) {
                    int padding = label->padding();
                    int x1 = x - padding;
                    int y1 = chartHeight - y - padding;
                    int x2 = x + textSize.width + padding;
                    int y2 = chartHeight - (y - textSize.height) + padding;
                    timeRangeArea.brush(label->backgroundColor());
                    timeRangeArea.fRect(x1, y1, x2, y2);
                    timeRangeArea.brush(label->borderColor());
                    timeRangeArea.rect(x1, y1, x2, y2);
                }
                timeRangeArea.brush(label->color());
                timeRangeArea.write(x, chartHeight - y, label->text());
            }
        }
    };

    class Alignment {
    protected:
        Projector* projector;
        Projector* alignToProjector;
        bool extends;
    public:
        explicit Alignment(
            Projector* projector,
            Projector* alignToProjector = NULL,
            bool extends = true
        ):
            projector(projector),
            alignToProjector(alignToProjector),
            extends(extends)
        {
            if (!projector) throw ERR_MISSING;
        }

        Projector* getProjector() const {
            return projector;
        }

        Projector* getAlignToProjector() const {
            return alignToProjector;
        }

        bool isExtends() const {
            return extends;
        }
    };

    class Chart: public TimeRangeArea {
    protected:
    
        vector<Projector*> projectors;
        vector<Alignment> alignments;

        vector<PointSeries*> pointSeriesProjectors;
        vector<CandleSeries*> candleSeriesProjectors;
        vector<LabelSeries*> labelSeriesProjectors;

        vector<PointShape*> pointShapes;
        vector<CandleShape*> candleShapes;
        vector<LabelShape*> labelShapes;
        
    public:
        using TimeRangeArea::TimeRangeArea;

        virtual ~Chart() {
            clear();
        }

        void clear() {
            vector_destroy(pointSeriesProjectors);
            vector_destroy(candleSeriesProjectors);
            vector_destroy(labelSeriesProjectors);
            projectors.clear();

            vector_destroy(pointShapes);
            vector_destroy(candleShapes);
            vector_destroy(labelShapes);
            alignments.clear();
        };

        void draw() override {
            TimeRangeArea::draw();
        
            for (const Alignment& alignment: alignments) {
                alignment.getProjector()->align(
                    alignment.getAlignToProjector(), 
                    alignment.isExtends()
                );                
            }

            for (Projector* projector: projectors) {
                if (!projector) continue;
                if (!projector->isPrepared()) continue;
                projector->project();
            }

        }

        PointSeries* createPointSeries(
            Projector* alignToProjector = NULL,
            bool alignExtends = true,
            Color color = Theme::defaultChartSeriesColor
        ) {
            PointSeries* pointSeries = new PointSeries(
                *this, color
            );
            pointSeriesProjectors.push_back(pointSeries);
            projectors.push_back(pointSeries);
            alignments.push_back(Alignment(
                pointSeries, alignToProjector, alignExtends
            ));
            return pointSeries;
        }

        CandleSeries* createCandleSeries(
            Projector* alignToProjector = NULL,
            bool alignExtends = true,
            Color colorUp = Theme::defaultChartCandleColorUp, 
            Color colorDown = Theme::defaultChartCandleColorDown
        ) {
            CandleSeries* candleSeries = new CandleSeries(
                *this, colorUp, colorDown
            );
            candleSeriesProjectors.push_back(candleSeries);
            projectors.push_back(candleSeries);
            alignments.push_back(Alignment(candleSeries, alignToProjector, alignExtends));
            return candleSeries;
        }

        LabelSeries* createLabelSeries(
            Projector* alignToProjector = NULL,
            bool alignExtends = true
        ) {
            LabelSeries* labelSeries = new LabelSeries(*this);
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
            const Color borderColor = Theme::defaultChartLabelBorderColor,
            const int padding = Theme::defaultChartLabelPadding,
            const bool hasBackground = Theme::defaultChartLabelHasBackground
        ) {
            LabelShape* labelShape = new LabelShape(
                time, value, text, 
                color, backgroundColor, borderColor, 
                padding, hasBackground
            );
            labelShapes.push_back(labelShape);
            return labelShape;
        }
    };

}