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

        bool apply(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                changed = begin != other.begin || end != other.end;
                begin = other.begin;
                end = other.end;
            }
            return changed;
        }

        bool expand(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                T _begin = begin, _end = end;
                begin = min(other.begin, begin);
                end = max(other.end, end);
                changed = begin != _begin || end != _end;
            }
            return changed;
        }

        bool limit(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                T _begin = begin, _end = end;
                begin = max(other.begin, begin);
                end = min(other.end, end);
                changed = begin != _begin || end != _end;
            }
            return changed;
        }

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
        const int _padding;
        const bool _hasBackground;

    public:

        LabelShape(
            ms_t time,
            double value,
            const string& text,
            const Color color = Theme::defaultChartLabelColor,
            const Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
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

        int padding() const {
            return _padding;
        }

        bool hasBackground() const {
            return _hasBackground;
        }
    };

    class Margin {
    public:

        int top, 
            left, 
            right,
            bottom; 

        Margin(
            int top = Theme::defaultTimeRangeAreMarginTop, 
            int left = Theme::defaultTimeRangeAreMarginLeft, 
            int right = Theme::defaultTimeRangeAreMarginRight,
            int bottom = Theme::defaultTimeRangeAreMarginBottom
        ):
            top(top), 
            left(left), 
            right(right), 
            bottom(bottom)
        {}
    };


    class TimeRangeArea: public Area {
    protected:
        Margin margin;

        TimeRange* timeRangeFull = NULL;
        TimeRange* timeRange = NULL;

        // convert x to ms_t
        ms_t retractX(int x) const {
            double diff = (double)timeRange->end - (double)timeRange->begin;
            double ms = (x * diff) / width;
            return (ms_t)ms;
        }

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
            timeRangeFull(new TimeRange(timeRangeBegin, timeRangeEnd)),
            timeRange(new TimeRange(timeRangeBegin, timeRangeEnd))
        {}

        virtual ~TimeRangeArea() {
            delete timeRangeFull;
            delete timeRange;
        }

        TimeRange& getTimeRange() const {
            return *timeRange;
        }

        TimeRange& getTimeRangeFull() const {
            return *timeRangeFull;
        }

        Margin* getMargin() {
            return &margin;
        }

        void point(int x, int y) override { 
            Area::point(x + margin.left, y + margin.top);
        }

        void rect(int x1, int y1, int x2, int y2) override {
            Area::rect(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        void fRect(int x1, int y1, int x2, int y2) override {
            Area::fRect(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        void line(int x1, int y1, int x2, int y2) override {
            Area::line(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        void hLine(int x1, int y1, int x2) override {
            Area::hLine(x1 + margin.left, y1 + margin.top, x2 + margin.left);
        }

        void vLine(int x1, int y1, int y2) override {
            Area::vLine(x1 + margin.left, y1 + margin.top, y2 + margin.top);
        }

        void write(int x, int y, const string& text) override {
            Area::write(x + margin.left, y + margin.top, text);
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

        bool align(Projector* other = NULL, bool extends = true) {
            if (prepared) return true;
            if (shapes.size() == 0) return prepared = false;
            if (other && !other->prepared) other->align(NULL, extends);

            chartHeight = timeRangeArea.getHeight() - 
                (timeRangeArea.getMargin()->top + timeRangeArea.getMargin()->bottom);
            chartWidth = timeRangeArea.getWidth() - 
                (timeRangeArea.getMargin()->left + timeRangeArea.getMargin()->right);

            if (other && other->prepared) {
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
            if (other && other->prepared) {
                other->chartBegin = chartBegin;
                other->chartEnd = chartEnd;
                other->chartInterval = chartInterval;
            }
            
            shapesValueMin = INFINITY;
            shapesValueMax = -INFINITY;
            if (other && other->prepared && extends) {
                shapesValueMin = other->shapesValueMin;
                shapesValueMax = other->shapesValueMax;
            }
            
            shapeIndexFrom = __SIZE_MAX__;
            shapeIndexTo = __SIZE_MAX__;

            for (const Shape* shape: shapes) {
                shapeIndexTo++;

                TimeRange shapeTimeRange = shape->getTimeRange();
                if (shapeTimeRange.end < chartBegin) continue;
                if (shapeTimeRange.begin > chartEnd) break;

                if (shapeIndexFrom == __SIZE_MAX__) shapeIndexFrom = shapeIndexTo;

                MinMax<double> valueMinMax = shape->getValueMinMax();
                if (shapesValueMin > valueMinMax.min) shapesValueMin = valueMinMax.min;
                if (shapesValueMax < valueMinMax.max) shapesValueMax = valueMinMax.max;

            }
            shapesValueDiff = shapesValueMax - shapesValueMin;
            if (other && other->prepared && extends) {
                other->shapesValueMin = shapesValueMin;
                other->shapesValueMax = shapesValueMax;
                other->shapesValueDiff = shapesValueDiff;
            }
            
            if (shapeIndexFrom == __SIZE_MAX__) return prepared = false;


            return prepared = true;
        }
    };

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

        void project() override {
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


    class CandleSeries: public Projector {
    protected:
    
        const Color colorUp;
        const Color colorDown;

        void projectFirstLastValue() {
            string text;

            const CandleShape* first = (const CandleShape*)shapes[shapeIndexFrom];
            timeRangeArea.brush(
                first->open() > first->close() ? colorDown : colorUp
            );
            text = to_string(first->open());
            TextSize textSize = timeRangeArea.getTextSize(text);
            timeRangeArea.write(
                -textSize.width, 
                chartHeight - translateY(first->open()), 
                text
            );

            const CandleShape* last = (const CandleShape*)shapes[shapeIndexTo];
            timeRangeArea.brush(
                last->open() > last->close() ? colorDown : colorUp
            );
            text = to_string(last->close());
            timeRangeArea.write(
                chartWidth, 
                chartHeight - translateY(last->close()), 
                text
            );
        }

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
            size_t step = (shapeIndexTo - shapeIndexFrom) / (size_t)chartWidth;
            if (step < 1) step = 1;
            for (size_t i = shapeIndexFrom; i < shapeIndexTo; i += step) {
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

            projectFirstLastValue();
        }
    };

    class LabelSeries: public Projector {
    protected:
        const size_t maxLabelsToShow = 50; // TODO
    public:

        using Projector::Projector;

        virtual ~LabelSeries() {}

        void project() override {
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
    public:

        class MultiChart {
        protected:

            vector<Chart*> charts;

        public:

            void attach(Chart& chart) {
                charts.push_back(&chart);
                chart.join(this);
                follow(chart);
            }

            void follow(const Chart& that) {
                for (Chart* chart: charts)
                    if (chart != &that) {
                        bool changed = false;
                        if (chart->getTimeRangeFull().expand(that.getTimeRangeFull())) changed = true;
                        if (chart->getTimeRange().apply(that.getTimeRange())) changed = true;
                        if (chart->getTimeRange().limit(chart->getTimeRangeFull())) changed = true;
                        if (changed) {
                            chart->resetProjectorsPrepared();
                            chart->draw();
                        }
                    }
            }

        };

    protected:

        MultiChart* multiChart = NULL;

        bool dragTimeRange = false;
        int dragTimeRangeStartedX = 0;

        static void zoomHandler(void* context, unsigned int button, int x, int) {
            if (
                button != Theme::zoomInScrollButton &&
                button != Theme::zoomOutScrollButton
            ) return;

            Chart* chart = (Chart*)context;
            const int partialWidth = chart->width / 5;
            bool zoomAtLeft = true;
            bool zoomAtRight = true;
            if (x <= partialWidth * 2) zoomAtRight = false;
            if (x >= partialWidth * 3) zoomAtLeft = false;
            if (chart->timeRange->begin < chart->timeRangeFull->begin) zoomAtLeft = true;
            if (chart->timeRange->end > chart->timeRangeFull->end) zoomAtRight = true;
            
            double origDiff = (double)(chart->timeRange->end - chart->timeRange->begin);
            double interval = origDiff;
            if (button == Theme::zoomInScrollButton) interval *= Theme::zoomInRatio;
            if (button == Theme::zoomOutScrollButton) interval *= Theme::zoomOutRatio;
            double diff = origDiff - interval;

            if (zoomAtLeft) chart->timeRange->end += (ms_t)diff;                
            if (zoomAtRight) chart->timeRange->begin -= (ms_t)diff;

            chart->getTimeRange().limit(chart->getTimeRangeFull());
            chart->resetProjectorsPrepared();
            chart->draw();
            if (chart->multiChart) chart->multiChart->follow(*chart);
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

            chart->getTimeRange().limit(chart->getTimeRangeFull());
            chart->resetProjectorsPrepared();
            chart->draw();
            if (chart->multiChart) chart->multiChart->follow(*chart);
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
            GFX &gfx, 
            int left, int top, int width, int height,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            const Border border = Theme::defaultChartBorder,
            const Color backgroundColor = Theme::defaultChartBackgroundColor,
            const Color borderColor = Theme::defaultChartBorderColor,
            void *eventContext = NULL
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

        void join(MultiChart* multiChart) {
            this->multiChart = multiChart;
        }

        void draw() override {
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

        PointSeries* createPointSeries(
            Projector* alignToProjector = NULL,
            bool alignExtends = true,
            Color color = Theme::defaultChartSeriesColor
        ) {
            PointSeries* pointSeries = new PointSeries(
                *this, color
            );
            pointSeriesProjectors.push_back(pointSeries);
            // alignToProjector = alignToProjector
            //     ? alignToProjector
            //     : projectors.size() > 0 ? projectors[0] : NULL;
            projectors.push_back(pointSeries);
            alignments.push_back(Alignment(
                pointSeries, 
                alignToProjector, 
                alignExtends
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


    class MultiChartAccordion: public Chart::MultiChart, public Accordion {
    protected:

        // vector<Chart*> charts;
        ms_t timeRangeBegin;
        ms_t timeRangeEnd;

        void createChartFrame(const string& title, Chart& chart, int frameHeight) {
            Frame* cntrFrame = createContainer(title, frameHeight)->getFrame();
            cntrFrame->setScrollFixed(true);
            int frameWidth = cntrFrame->getWidth();
            chart.setTop(0);
            chart.setLeft(0);
            chart.setWidth(frameWidth - 2);
            chart.setHeight(frameHeight - 1);
            chart.setBorder(PUSHED);
            chart.setBackgroundColor(black);
            cntrFrame->child(chart);
        }

    public:
    
        MultiChartAccordion(
            GFX& gfx, int left, int top, int width,
            ms_t timeRangeBegin,
            ms_t timeRangeEnd,
            bool single = false,
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor,
            void* eventContext = NULL
        ):
            Accordion(
                gfx, left, top, width, 
                single, border, backgroundColor, 
                eventContext
            ),
            timeRangeBegin(timeRangeBegin),
            timeRangeEnd(timeRangeEnd)
        {}

        virtual ~MultiChartAccordion() {
            vector_destroy(charts);
        }

        Chart* createChart(const string& title, int frameHeight) { // TODO bubble up params default
            Chart* chart = new Chart(gfx, 0, 0, 0, 0, timeRangeBegin, timeRangeEnd);
            charts.push_back(chart);
            createChartFrame(title, *chart, frameHeight);
            attach(*chart);
            return chart;
        }
    };
}