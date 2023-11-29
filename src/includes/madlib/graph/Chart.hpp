#pragma once

#include "graph.hpp"
#include "../madlib.hpp"

using namespace std;
using namespace madlib;

namespace madlib::graph {

    enum Shape { DOT, LINE, BOX, FILLED, CANDLE, LABEL };

    class ShapeName {
    public:
        static const string getName(Shape shape) {
            switch (shape) {
                case DOT: return "DOT";
                case LINE: return "LINE";
                case BOX: return "BOX";
                case FILLED: return "FILLED";
                case CANDLE: return "CANDLE";
                case LABEL: return "LABEL";
                default: throw ERROR("Invalid shape");
            }
        }
    };

    class Chart: public Frame {
    public:

        class LabelStyle {
        protected:
            Color color;
            bool hasBackground;
            Color backgroundColor;
            Color borderColor;
            int padding;
        public:
            LabelStyle(
                const Color color = Theme::defaultChartLabelColor,
                const bool hasBackground = Theme::defaultChartLabelHasBackground,
                const Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
                const Color borderColor = Theme::defaultChartLabelBorderColor,
                const int padding = Theme::defaultChartLabelPadding
            ):
                color(color),
                hasBackground(hasBackground),
                backgroundColor(backgroundColor),
                borderColor(borderColor),
                padding(padding)
            {}

            Color getColor() const {
                return color;
            }

            bool isHasBackground() const {
                return hasBackground;
            }

            Color getBackgroundColor() const {
                return backgroundColor;
            }

            Color getBorderColor() const {
                return borderColor;
            }

            int getPadding() const {
                return padding;
            }
        };


        class CandleStyle {
        protected:
            const Color colorUp;
            const Color colorDown;
        public:
            CandleStyle(
                const Color colorUp = Theme::defaultCandleColorUp,
                const Color colorDown = Theme::defaultCandleColorDown
            ):
                colorUp(colorUp),
                colorDown(colorDown)
            {}

            Color getColorUp() const {
                return colorUp;
            }

            Color getColorDown() const {
                return colorDown;
            }
        };

        static const Shape defaultChartShape = LINE;


        class Scale: public Zoomable {
        public:

            const static CandleStyle defaultCandleStyle;
            const static LabelStyle defaultLabelStyle;

        protected:

            static const void* getDefaultScaleContext(Shape shape) {
                switch (shape)
                {
                    case DOT: return &Theme::defaultChartDotScaleContext;
                    case LINE: return &Theme::defaultChartLineScaleContext;
                    case BOX: return &Theme::defaultChartBoxScaleContext;
                    case FILLED: return &Theme::defaultChartFilledScaleContext;
                    case CANDLE: return &defaultCandleStyle; // TODO;
                    case LABEL: return &defaultLabelStyle; // TODO;
                    default: throw ERROR("Invalid shape");
                }
                return NULL;
            }

            double xmin, ymin, xmax, ymax;
            int width, height;
            Shape shape;
            const void* context = NULL;

            vector<ProjectedPoint> projectedPoints;
            vector<string> texts;

            int projectX(double x, bool adapt = true) {
                if (adapt) adaptX(x);
                return (int)(((x - xmin) * width) / (xmax - xmin));
            }

            int projectY(double y, bool adapt = true) {
                if (adapt) adaptY(y);
                return (int)(((y - ymin) * height) / (ymax - ymin));
            }
            
            ProjectedPoint& project(double x, double y, ProjectedPoint& result, bool adapt = true) {
                if (adapt) adaptXY(x, y);
                // result.setX(projectX(x, adapt));
                // result.setY(projectY(y, adapt));
                result = zoom.apply(
                    projectX(xmin, false), projectY(ymin, false), 
                    projectX(x, adapt), projectY(y, adapt)
                );
                return result;
            }
            
        public:

            Scale(
                Zoom& zoom, int width, int height, 
                Shape shape = LINE, const void* context = NULL
            ):
                Zoomable(zoom),
                width(width), height(height), shape(shape), 
                context(context ? context : getDefaultScaleContext(shape))
            {
                reset();
            }

            void reset() {
                xmin = INFINITY;
                ymin = INFINITY;
                xmax = -INFINITY;
                ymax = -INFINITY;
                projectedPoints.clear();
            }

            Shape getShape() const {
                return shape;
            }

            const void* getContext() const {
                return context;
            }

            void setShape(const Shape& shape, const void* context = NULL) {
                this->shape = shape;
                this->context = context ? context : getDefaultScaleContext(shape);
            }

            const vector<ProjectedPoint>& getProjectedPoints() const {
                return projectedPoints;
            }

            const vector<string>& getTexts() const {
                return texts;
            }

            void adaptX(double x) {
                if (xmin > x) xmin = x;
                if (xmax < x) xmax = x;
            }

            void adaptY(double y) {
                if (ymin > y) ymin = y;
                if (ymax < y) ymax = y;
            }

            void adaptXY(double x, double y) {
                adaptX(x);
                adaptY(y);
            }

            void adaptXY(RealPoint realPoint) {
                adaptXY(realPoint.getX(), realPoint.getY());
            }

            void adaptXY(const vector<RealPoint>& realPoints) {
                reset();
                for (const RealPoint realPoint: realPoints) adaptXY(realPoint);
            }

            vector<ProjectedPoint>& project(const vector<RealPoint>& realPoints, bool adapt = true) {
                projectedPoints.clear();
                if (adapt) adaptXY(realPoints);
                ProjectedPoint projectedPoint;
                // for (const RealPoint& realPoint: realPoints) 
                //     projectedPoints.push_back(
                //         project(realPoint.getX(), realPoint.getY(), projectedPoint, false)
                //     );
                transform(realPoints.begin(), realPoints.end(), back_inserter(projectedPoints),
                    [&](const RealPoint& realPoint) {
                        return project(realPoint.getX(), realPoint.getY(), projectedPoint, false);
                    }
                );
                return projectedPoints;
            }

            void project(const vector<RealPoint>& realPoints, const vector<string> &texts, bool adapt = true) {
                project(realPoints, adapt);
                this->texts = texts;
            }

            static void alignX(Scale& scale1, Scale& scale2) {
                double xmin = scale1.xmin < scale2.xmin ? scale1.xmin : scale2.xmin;
                double xmax = scale1.xmax > scale2.xmax ? scale1.xmax : scale2.xmax;
                scale1.xmin = xmin;
                scale1.xmax = xmax;
                scale2.xmin = xmin;
                scale2.xmax = xmax;
            }

            // static void alignX(vector<Scale>& scales) {
            //     size_t size = scales.size();
            //     if (size < 2) return;
            //     size_t stop = scales.size() - 1;
            //     for (size_t i = 0; i < stop; i++) alignX(scales.at(i), scales.at(i + 1));
            // }

            static void alignY(Scale& scale1, Scale& scale2) {
                double ymin = scale1.ymin < scale2.ymin ? scale1.ymin : scale2.ymin;
                double ymax = scale1.ymax > scale2.ymax ? scale1.ymax : scale2.ymax;
                scale1.ymin = ymin;
                scale1.ymax = ymax;
                scale2.ymin = ymin;
                scale2.ymax = ymax;
            }

            // static void alignY(vector<Scale>& scales) {
            //     size_t size = scales.size();
            //     if (size < 2) return;
            //     size_t stop = scales.size() - 1;
            //     for (size_t i = 0; i < stop; i++) alignY(scales.at(i), scales.at(i + 1));
            // }

            static void alignXY(Scale& scale1, Scale& scale2) {
                alignX(scale1, scale2);
                alignY(scale1, scale2);
            }

            // static void alignXY(vector<Scale>& scales) {
            //     size_t size = scales.size();
            //     if (size < 2) return;
            //     size_t stop = scales.size() - 1;
            //     for (size_t i = 0; i < stop; i++) alignXY(scales.at(i), scales.at(i + 1));
            // }
        };

    protected:

        static const int zoomInScrollButton = 4; // TODO
        static const int zoomOutScrollButton = 5; // TODO
        static constexpr double zoomInRatio = 1.25; // TODO
        static constexpr double zoomOutRatio = .8; // TODO
        static constexpr double zoomRatioMax = INFINITY; // TODO
        static constexpr double zoomRatioMin = 1; // TODO

        static void zoomHandler(void* context, unsigned int button, int, int) {
            Chart* that = (Chart*)context;

            // change zoom ratio

            double ratioX;
            switch (button) {
                case zoomInScrollButton:
                    ratioX = that->getZoom().getRatio().getX() * zoomInRatio;
                    if (ratioX > zoomRatioMax) ratioX = zoomRatioMax;
                    break;
                case zoomOutScrollButton:
                    ratioX = that->getZoom().getRatio().getX() * zoomOutRatio;
                    if (ratioX < zoomRatioMin) ratioX = zoomRatioMin;
                    break;
                default:
                    return; // no scroll
            }
            that->getZoom().setRatioX(ratioX);
            
            // redraw

            that->project();
            that->redraw();
        }

        Color borderColor = white; // TODO
        
        vector<Scale*> scales;
        // Factory<Scale> scales;

        typedef void (*DrawPairFunction)(Chart*, int, int, int, int);

        static void drawPairAsLine(Chart* chart, int x1, int y1, int x2, int y2) {
            chart->line(x1, y1, x2, y2);
        }

        static void drawPairAsBox(Chart* chart, int x1, int y1, int x2, int y2) {
            chart->rect(x1, y1, x2, y2);
        }

        static void drawPairAsFilled(Chart* chart, int x1, int y1, int x2, int y2) {
            chart->fRect(x1, y1, x2, y2);
        }

    public:

        Chart(
            GFX& gfx, Zoom& zoom, 
            const int left, const int top, 
            const int width, const int height,
            const Border border = Theme::chartBorder,
            const Color backgroundColor = Theme::chartBackgroundColor
        ): Frame(
            gfx, zoom, left, top, width, height,
            border, backgroundColor
        ) {
            addTouchHandler(zoomHandler);
        }

        ~Chart() {
            vector_destroy<Scale>(scales);
        }

        virtual void project() {
            // if (onDrawHandlers.empty()) {
            //     throw ERROR("Chart does not show anything because projection is not implemented nor drawHandler added");
            // }
        };

        const vector<Scale*> getScales() const {
            return scales;
        }

        Scale* getScaleAt(size_t at) {
            if (scales.size() <= at) throw ERROR("Invalid scale index: " + to_string(at));
            return scales.at(at);
        }

        Scale* createScale(Zoom& zoom, Shape shape = LINE, const void* context = NULL) {
            Scale* scale = vector_create(scales, zoom, width, height, shape, context);
            return scale;
        }

        void destroyScales() {
            vector_destroy(scales);
        }
        
        void drawPoint(int x, int y) {
            point(x, height - y);
        }

        void drawPoint(const ProjectedPoint& projectedPoint) {
            point(projectedPoint.getX(), height - projectedPoint.getY());
        }

        void drawPoints(const size_t scale, const Color color) {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            brush(color);
            int h = height;
            for (const ProjectedPoint& projectedPoint: projectedPoints) 
                point(projectedPoint.getX(), h - projectedPoint.getY());
        }

        void drawLine(int x1, int y1, int x2, int y2) {
            line(x1, height - y1, x2, height - y2);
        }

        void drawRect(int x1, int y1, int x2, int y2) {
            rect(x1, height - y1, x2, height - y2);
        }

        void fillRect(int x1, int y1, int x2, int y2) {
            fRect(x1, height - y1, x2, height - y2);
        }

        void drawPairs(const size_t scale, const Color color, const Shape shape) {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            brush(color);
            ProjectedPoint prjPoint = projectedPoints.at(0);
            int x1 = prjPoint.getX();
            int y1 = prjPoint.getY();
            int painterHeight = height;
            DrawPairFunction func;
            switch (shape) {
                case LINE:
                    func = drawPairAsLine;
                    break;
                case BOX:
                    func = drawPairAsBox;
                    break;
                case FILLED:
                    func = drawPairAsFilled;
                    break;
                default:
                    throw ERROR("Invalid pair shape: " + ShapeName::getName(shape));
            }
            for (const ProjectedPoint& projectedPoint: projectedPoints) {
                int x2 = projectedPoint.getX();
                int y2 = projectedPoint.getY();
                func(this, x1, painterHeight - y1, x2, painterHeight - y2);
                x1 = x2;
                y1 = y2;
            }
        }

        void drawLines(const size_t scale, const Color color) {
            drawPairs(scale, color, LINE);
        }

        void drawRectangles(const size_t scale, const Color color) {
            drawPairs(scale, color, BOX);
        }

        void fillRectangles(const size_t scale, const Color color) {
            drawPairs(scale, color, FILLED);
        }

        void drawCandle(
            int openX, int openY,
            int closeX, int closeY,
            int lowX, int lowY,
            int highY,
            const Color colorInc = green,
            const Color colorDec = red,
            int painterHeight = 0
        ) {
            Color color = (openY < closeY) ? colorInc : colorDec;
            painterHeight = painterHeight ? painterHeight : height;
            brush(ColorMixer::light(color));
            vLine(lowX, painterHeight - lowY, painterHeight - highY);
            brush(color);
            if (openY == closeY) {
                hLine(openX, painterHeight - openY, closeX);
                return;
            }
            const int width = closeX - openX;
            if (width > 5) { openX++; closeX--; }
            if (width > 10) { openX++; closeX--; }
            if (width > 20) { openX++; closeX--; }
            if (width > 50) { openX++; closeX--; }
            if (width > 100) { openX++; closeX--; }
            fRect(openX, painterHeight - openY, closeX, painterHeight - closeY);
        }

        void drawCandle(
            // FlawFinder: ignore
            ProjectedPoint open,
            ProjectedPoint close,
            ProjectedPoint low,
            ProjectedPoint high,
            const Color colorInc = green, 
            const Color colorDec = red,
            int painterHeight = 0
        ) {
            drawCandle(
                open.getX(), open.getY(), // FlawFinder: ignore
                close.getX(), close.getY(),
                low.getX(), low.getY(),
                high.getY(),
                colorInc, colorDec,
                painterHeight
            );
        }

        void drawCandles(const size_t scale, const CandleStyle& candleStyle) {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            size_t size = projectedPoints.size();
            int painterHeight = height;
            for (size_t i = 0; i < size; i += 4) {
                const ProjectedPoint& open = projectedPoints.at(i);
                const ProjectedPoint& close = projectedPoints.at(i + 1);
                const ProjectedPoint& low = projectedPoints.at(i + 2);
                const ProjectedPoint& high = projectedPoints.at(i + 3);
                drawCandle(
                    open, close, low, high, 
                    candleStyle.getColorUp(), 
                    candleStyle.getColorDown(), 
                    painterHeight
                );
            }
        }

        void putLabel(
            int x, int y, const string &text, 
            Color color = Theme::defaultChartLabelColor,
            bool hasBackground = Theme::defaultChartLabelHasBackground,
            Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
            Color borderColor = Theme::defaultChartLabelBorderColor,
            int padding = Theme::defaultChartLabelPadding,
            int painterHeight = -1
        ) {
            painterHeight = painterHeight == -1 ? height : painterHeight;
            const int painterHeight_y = painterHeight - y;
            if (hasBackground) {
                Painter::TextSize textSize = getTextSize(text);
                const int x1 = x - padding;
                const int y1 = painterHeight_y - textSize.height - padding;
                const int x2 = x + textSize.width + padding;
                const int y2 = painterHeight_y + padding; // + textSize.height;
                brush(backgroundColor);
                fRect(x1, y1, x2, y2);
                brush(borderColor);
                rect(x1, y1, x2, y2);
            }
            brush(color);
            write(x, painterHeight_y - padding, text);
        }

        void putLabels(size_t scale, const LabelStyle& labelStyle) {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            vector<string> texts = scales.at(scale)->getTexts();
            if (texts.empty()) return;
            int painterHeight = height;
            size_t size = projectedPoints.size();
            for (size_t i = 0; i < size; i++) {
                ProjectedPoint projectedPoint = projectedPoints.at(i);
                putLabel(
                    projectedPoint.getX(), projectedPoint.getY(), texts.at(i), 
                    labelStyle.getColor(), labelStyle.isHasBackground(), 
                    labelStyle.getBackgroundColor(), labelStyle.getBorderColor(),
                    labelStyle.getPadding(), painterHeight
                );
            }
        }

        // -------- 

        void redraw() {
            // recalc
            setCalcScrollOnly(true);
            Point<int> scrollXY = getScrollXY();
            resetScrollXYMax();
            drawScales();
            setScrollXY(scrollXY.getX(), scrollXY.getY());

            // actual drawing
            setCalcScrollOnly(false);
            draw();
        }

        virtual void draw() override {
            if (!calcScrollOnly) Frame::draw();
            drawScales();
        }

        void drawScales() {
            size_t at = 0;
            for (const Scale* scale: scales) {
                const Shape shape = scale->getShape();
                const void* context = scale->getContext();

                const Color* contextAsColor = (const Color*)context;
                Color color = *contextAsColor;

                const CandleStyle* contextAsCandleStyle = (const CandleStyle*)context;
                CandleStyle candleStyle(
                    contextAsCandleStyle->getColorUp(), 
                    contextAsCandleStyle->getColorDown()
                );
                
                const LabelStyle* contextAsLabelStyle = (const LabelStyle*)context;
                LabelStyle labelStyle(
                    contextAsLabelStyle->getColor(),
                    contextAsLabelStyle->isHasBackground(),
                    contextAsLabelStyle->getBackgroundColor(),
                    contextAsLabelStyle->getBorderColor()
                );

                switch (shape)
                {
                    case DOT:
                        drawPoints(at, color);
                        break;

                    case LINE:
                        drawLines(at, color);
                        break;

                    case BOX:
                        drawRectangles(at, color);
                        break;

                    case FILLED:
                        fillRectangles(at, color);
                        break;

                    case CANDLE:
                        drawCandles(at, candleStyle);
                        break;

                    case LABEL:
                        putLabels(at, labelStyle);
                        break;
                    
                    default:
                        throw ERROR("invalid shape type: " + to_string((int)shape));
                        break;
                    }
                at++;
            }
        }

    };


    const Chart::CandleStyle Chart::Scale::defaultCandleStyle = CandleStyle();
    const Chart::LabelStyle Chart::Scale::defaultLabelStyle = LabelStyle();

    class MultiChartAccordion: public Accordion {
    protected:

        const int innerBorderSize = 2; // TODO
        vector<Chart*> charts;

        void setupChart(Chart& chart, int frameWidth, int frameHeight) {
            chart.setTop(innerBorderSize);
            chart.setLeft(innerBorderSize);
            chart.setWidth(frameWidth - innerBorderSize * 2);
            chart.setHeight(frameHeight - innerBorderSize * 2);
            chart.setBorder(BUTTON_PUSHED);
            chart.setBackgroundColor(black);
        }

    public:
        using Accordion::Accordion;

        ~MultiChartAccordion() {
            vector_destroy(charts);
        }

        Chart& createChart(const string& title, int frameHeight) { // TODO bubble up params default
            const Accordion::Container& container = createContainer(zoom, title, frameHeight);
            Frame& cntrFrame = container.getFrame();
            cntrFrame.setFixed(true);
            Chart* chart = vector_create(
                charts, gfx, zoom, 0, 0, 0, 0
            );
            setupChart(*chart, cntrFrame.getWidth(), frameHeight);
            cntrFrame.child(*chart);
            return *chart;
        }

        Chart& addChart(const string& title, Chart& chart, int frameHeight) {
            const Accordion::Container& container = 
                createContainer(zoom, title, frameHeight);
            Frame& cntrFrame = container.getFrame();
            cntrFrame.setFixed(true);
            setupChart(chart, cntrFrame.getWidth(), frameHeight);
            cntrFrame.child(chart);
            return chart;
        }
        
    };

}