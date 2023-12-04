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

    // TODO: Add margin and show value labels to chart (at first / last and pointed place), also add legend
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
                const Color colorUp = Theme::defaultChartCandleColorUp,
                const Color colorDown = Theme::defaultChartCandleColorDown
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

        class Scale: public Zoomable {
        public:

            const static CandleStyle defaultChartCandleStyle;
            const static LabelStyle defaultChartLabelStyle;

            static const void* getDefaultScaleContext(Shape shape) {
                switch (shape)
                {
                    case DOT: return &Theme::defaultChartDotScaleContext;
                    case LINE: return &Theme::defaultChartLineScaleContext;
                    case BOX: return &Theme::defaultChartBoxScaleContext;
                    case FILLED: return &Theme::defaultChartFilledScaleContext;
                    case CANDLE: return &defaultChartCandleStyle;
                    case LABEL: return &defaultChartLabelStyle;
                    default: throw ERROR("Invalid shape");
                }
                return NULL;
            }


        protected:

            double xmin, ymin, xmax, ymax;
            const Chart& chart;
            Shape shape;
            bool adapt;
            const void* context = NULL;

            vector<Pixel> pixels;

            vector<Coord> coords;
            vector<string> texts;

            int projectX(double x, bool adapt) {
                if (adapt) adaptX(x);
                return (int)(((x - xmin) * chart.getWidth()) / (xmax - xmin));
            }

            int projectY(double y, bool adapt) {
                if (adapt) adaptY(y); // TODO: BUG: I think here is the issue with vertical scroll after zoom in and out
                return (int)(((y - ymin) * chart.getHeight()) / (ymax - ymin));
            }
            
            Pixel& project(double x, double y, Pixel& result) {
                if (adapt) adaptXY(x, y);
                result = applyZoom(
                    projectX(xmin, false), projectY(ymin, false), 
                    projectX(x, adapt), projectY(y, adapt)
                );
                return result;
            }
            
        public:

            Scale(
                const Chart& chart,
                Shape shape = LINE, bool adapt = true, const void* context = NULL
            ):
                Zoomable(false),
                chart(chart),
                shape(shape), adapt(adapt),
                context(context ? context : getDefaultScaleContext(shape))
            {
                reset();
            }

            virtual ~Scale() final {}

            Scale& setAdapt(bool adapt) {
                this->adapt = adapt;
                return *this;
            }

            void reset() {
                xmin = INFINITY;
                ymin = INFINITY;
                xmax = -INFINITY;
                ymax = -INFINITY;
                pixels.clear();
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

            const vector<Pixel>& getPixels() const {
                return pixels;
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

            void adaptXY(const Coord& coord) {
                adaptXY(coord.x, coord.y);
            }

            void adaptXY(const vector<Coord>& coords) {
                reset();
                for (const Coord& coord: coords) adaptXY(coord);
            }

            void setRealPoints(const vector<Coord>& coords) {
                this->coords = coords;
            }

            void project(const vector<Coord>& coords) {
                setRealPoints(coords);
                project();
            }

            void project() {                
                if (adapt) adaptXY(coords);
                else pixels.clear();
                Pixel pixel;
                bool _adapt = adapt;
                adapt = false;
                // Rectangle bounds(
                //     - chart.getScrollX(), 
                //     - chart.getScrollY(), 
                //     - chart.getScrollX() + chart.getWidth(), 
                //     - chart.getScrollY() + chart.getHeight()
                // );
                // DBG("bounds:", bounds.x1, ":", bounds.x2);
                // DBG("scale:", xmin, ":", xmax);
                for (const Coord& coord: coords) { // NOTE: it may can be sped up by projecting only those points that inside the chart view (just calculate where are the chart edges converted to real-coordinates)
                    project(coord.x, coord.y, pixel);
                    // DBG("px:", pixel.x, " s:", chart.getScrollX());
                    // if (bounds.contains(pixel))
                    pixels.push_back(pixel);
                }
                adapt = _adapt;
            }

            void setTexts(const vector<string>& texts) {
                this->texts = texts;
            }

            void project(const vector<Coord>& coords, const vector<string>& texts) {
                setRealPoints(coords);
                setTexts(texts);
                project();
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
    
        static void zoomHandler(void* context, unsigned int /*button*/, int x, int y) {
            Chart* that = (Chart*)context;
            DBG(x, ":", y);
            that->projectScales();
            that->redraw();
        }
        
        vector<Scale*> scales;

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
            GFX& gfx,
            const int left, const int top, 
            const int width, const int height,
            const Border border = Theme::defaultChartBorder,
            const Color backgroundColor = Theme::defaultChartBackgroundColor,
            void* eventContext = NULL
        ): Frame(
            gfx, left, top, width, height, false, false,
            border, backgroundColor, eventContext
        ) {
            scrollFixedY = true;
            zoomFixedY = true;
            addTouchHandler(zoomHandler);
        }

        virtual ~Chart() {
            vector_destroy<Scale>(scales);
        }

        virtual void projectScales() {
            for (Scale* scale: scales) {
                scale->setZoomFrom(*this);
                scale->project();
            }
        };

        const vector<Scale*> getScales() const {
            return scales;
        }

        Scale& getScaleAt(size_t at) {
            if (scales.size() <= at) throw ERROR("Invalid scale index: " + to_string(at));
            return *scales.at(at);
        }

        Scale& createScale(Shape shape/* = LINE*/, bool adapt, const void* context = NULL) {
            return *vector_create(scales, *this, shape, adapt, context ? context : Scale::getDefaultScaleContext(shape));
        }
        
        void drawPoint(int x, int y) {
            point(x, height - y);
        }

        void drawPoint(const Pixel& pixel) {
            point(pixel.x, height - pixel.y);
        }

        void drawPoints(const size_t scale, const Color color) {
            vector<Pixel> pixels = scales.at(scale)->getPixels();
            if (pixels.empty()) return;
            brush(color);
            int h = height;
            for (const Pixel& pixel: pixels) 
                point(pixel.x, h - pixel.y);
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
            vector<Pixel> pixels = scales.at(scale)->getPixels();
            if (pixels.empty()) return;
            brush(color);
            Pixel prjPoint = pixels.at(0);
            int x1 = prjPoint.x;
            int y1 = prjPoint.y;
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
            for (const Pixel& pixel: pixels) {
                int x2 = pixel.x;
                int y2 = pixel.y;
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
            // TODO: FlawFinder: ignore
            Pixel open,
            Pixel close,
            Pixel low,
            Pixel high,
            const Color colorInc = green, 
            const Color colorDec = red,
            int painterHeight = 0
        ) {
            drawCandle(
                open.x, open.y, // FlawFinder: ignore
                close.x, close.y,
                low.x, low.y,
                high.y,
                colorInc, colorDec,
                painterHeight
            );
        }

        void drawCandles(const size_t scale, const CandleStyle& candleStyle) {
            vector<Pixel> pixels = scales.at(scale)->getPixels();
            if (pixels.empty()) return;
            size_t size = pixels.size();
            int painterHeight = height;
            for (size_t i = 0; i < size; i += 4) {
                const Pixel& open = pixels.at(i);
                const Pixel& close = pixels.at(i + 1);
                const Pixel& low = pixels.at(i + 2);
                const Pixel& high = pixels.at(i + 3);
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
            vector<Pixel> pixels = scales.at(scale)->getPixels();
            if (pixels.empty()) return;
            vector<string> texts = scales.at(scale)->getTexts();
            if (texts.empty()) return;
            int painterHeight = height;
            size_t size = pixels.size();
            for (size_t i = 0; i < size; i++) {
                Pixel pixel = pixels.at(i);
                putLabel(
                    pixel.x, pixel.y, texts.at(i), 
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
            setScrollXY(scrollXY.x, scrollXY.y);

            // actual drawing
            setCalcScrollOnly(false);
            draw();
        }

        virtual void draw() override final {
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

    const Chart::CandleStyle Chart::Scale::defaultChartCandleStyle = CandleStyle();
    const Chart::LabelStyle Chart::Scale::defaultChartLabelStyle = LabelStyle();

    class MultiChartAccordion: public Accordion {
    protected:

        const int innerBorderSize = Theme::defaultMultiChartAccordionInnerBorderSize;
        vector<Chart*> charts;

    public:
        using Accordion::Accordion;

        virtual ~MultiChartAccordion() {
            vector_destroy(charts);
        }

        Chart& createChart(const string& title, int frameHeight) { // TODO bubble up params default
            Chart& chart = *vector_create(charts, gfx, 0, 0, 0, 0);
            return addChart(title, chart, frameHeight);
        }

        Chart& addChart(const string& title, Chart& chart, int frameHeight) {
            Frame& cntrFrame = createContainer(title, frameHeight).getFrame();
            cntrFrame.setScrollFixed(true);
            int frameWidth = cntrFrame.getWidth();
            chart.setTop(innerBorderSize);
            chart.setLeft(innerBorderSize);
            chart.setWidth(frameWidth - innerBorderSize * 2);
            chart.setHeight(frameHeight - innerBorderSize * 2);
            chart.setBorder(PUSHED);
            chart.setBackgroundColor(black);
            cntrFrame.child(chart);
            return chart;
        }
        
        virtual void projectCharts() {
            for (Chart* chart: charts) {
                chart->projectScales();
            }
        }
    };

}