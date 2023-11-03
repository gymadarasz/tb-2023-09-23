#pragma once

#include "graph.hpp"
#include "../madlib.hpp"

using namespace std;
using namespace madlib;

namespace madlib::graph {

    template<typename T>
    class PointTemplate {
    protected:

        T x, y;

    public:

        PointTemplate(T x = 0, T y = 0): x(x), y(y) {}

        void setX(T x) {
            this->x = x; 
        }

        T getX() const {
            return x;
        }

        void setY(T y) {
            this->y = y;
        }

        T getY() const {
            return y;
        }

    };

    class RealPoint: public PointTemplate<double> {
    public:
        using PointTemplate<double>::PointTemplate;
    };

    class ProjectedPoint: public PointTemplate<int> {
    public:
        using PointTemplate<int>::PointTemplate;
    };

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

    class Zoom {
    public:
        const int defaultCenterX = 0, defaultCenterY = 0;
        const double defaultRatioX = 1.0, defaultRatioY = 1.0;
    protected:
        ProjectedPoint center = ProjectedPoint(defaultCenterX, defaultCenterY);
        RealPoint ratio = RealPoint(defaultRatioX, defaultRatioY);

    public:

        Zoom(): center(defaultCenterX, defaultCenterY), ratio(defaultRatioX, defaultRatioY) {}
        Zoom(int centerX, int centerY): center(centerX, centerY), ratio(defaultRatioX, defaultRatioY) {}
        explicit Zoom(const ProjectedPoint& center): center(center), ratio(defaultRatioX, defaultRatioY) {}
        Zoom(double ratioX, double ratioY): center(defaultCenterX, defaultCenterY), ratio(ratioX, ratioY) {}
        explicit Zoom(const RealPoint& ratio): center(defaultCenterX, defaultCenterY), ratio(ratio) {}
        Zoom(int centerX, int centerY, double ratioX, double ratioY): center(centerX, centerY), ratio(ratioX, ratioY) {}
        Zoom(const ProjectedPoint& center, const RealPoint& ratio): center(center), ratio(ratio) {}
        Zoom(double ratioX, double ratioY, int centerX, int centerY): center(centerX, centerY), ratio(ratioX, ratioY) {}
        Zoom(const RealPoint& ratio, const ProjectedPoint& center): center(center), ratio(ratio) {}
        
        Zoom& operator=(const Zoom& other) {
            if (this != &other) { // Check for self-assignment
                this->center = other.center;
                this->ratio = other.ratio;
            }
            return *this;
        }

        ProjectedPoint getCenter() const {
            return center;
        }

        void setCenter(const ProjectedPoint& center) {
            this->center = center;
        }

        void setCenter(int x = 0, int y = 0) {
            setCenter(ProjectedPoint(x, y));
        }

        RealPoint getRatio() const {
            return ratio;
        }

        void setRatio(const RealPoint& ratio) {
            this->ratio = ratio;
        }

        void setRatio(double x = 1.0, double y = 1.0) {
            setRatio(RealPoint(x, y));
        }

        int applyX(int origoX, int pointX) const {
            int origoXAddCenterX = origoX + center.getX();
            int pointXSubCenterX = pointX - origoXAddCenterX;
            double pointXSubCenterXMulRatioX = pointXSubCenterX * ratio.getX();
            return origoXAddCenterX + (int)pointXSubCenterXMulRatioX;
        }

        int applyY(int origoY, int pointY) const {
            int origoYAddCenterY = origoY + center.getY();
            int pointYSubCenterY = pointY - origoYAddCenterY;
            double pointYSubCenterYMulRatioY = pointYSubCenterY * ratio.getY();
            return origoYAddCenterY + (int)pointYSubCenterYMulRatioY;
        }

        ProjectedPoint apply(int origoX, int origoY, int pointX, int pointY) const {
            ProjectedPoint result(
                applyX(origoX, pointX),
                applyY(origoY, pointY)
            );

            return result;
        }
        
        ProjectedPoint apply(const ProjectedPoint& origo, const ProjectedPoint& point) const {
            return apply(origo.getX(), origo.getY(), point.getX(), point.getY());
        }
    };

    class Zoomable {
    protected:
        Zoom zoom;

    public:
        void setZoom(const Zoom& zoom) {
            this->zoom = zoom;
        }

        Zoom& getZoom() {
            return zoom;
        }
    };

    class Chart: public Zoomable {
    public:

        class LabelStyle {
        protected:
            Color color = white; // TODO
            bool hasBackground = true; // TODO
            Color backgroundColor = black; // TODO
            Color borderColor = gray; // TODO
            int padding = 3; // TODO
        public:
            LabelStyle(
                const Color color = white, // TODO
                const bool hasBackground = true, // TODO
                const Color backgroundColor = black, // TODO
                const Color borderColor = gray, // TODO
                const int padding = 3 // TODO
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
            const Color colorUp = green; // TODO
            const Color colorDown = red; // TODO
        public:
            CandleStyle(
                const Color colorUp = green, // TODO
                const Color colorDown = red // TODO
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
        protected:
            const CandleStyle defaultCandleStyle;
            const LabelStyle defaultLabelStyle;

            const void* getDefaultScaleContext(Shape shape) {
                switch (shape)
                {
                    case DOT: return &white; // TODO;
                    case LINE: return &white; // TODO;
                    case BOX: return &white; // TODO;
                    case FILLED: return &white; // TODO;
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

            Scale(int width, int height, Shape shape = LINE, const void* context = NULL): 
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

        Painter& painter;
        Color borderColor = white; // TODO
        
        vector<Scale*> scales;

        typedef void (*DrawPairFunction)(Painter*, int, int, int, int);

        static void drawPairAsLine(Painter* painter, int x1, int y1, int x2, int y2) {
            painter->line(x1, y1, x2, y2);
        }

        static void drawPairAsBox(Painter* painter, int x1, int y1, int x2, int y2) {
            painter->rect(x1, y1, x2, y2);
        }

        static void drawPairAsFilled(Painter* painter, int x1, int y1, int x2, int y2) {
            painter->fillRect(x1, y1, x2, y2);
        }

    public:

        explicit Chart(Painter& painter): painter(painter) {}

        ~Chart() {
            vector_destroy<Scale>(scales);
        }

        const vector<Scale*>& getScales() const {
            return scales;
        }

        Scale& getScaleAt(size_t at) {
            if (scales.size() <= at) throw ERROR("Invalid scale index: " + to_string(at));
            return *scales.at(at);
        }

        Scale& addScale(Shape shape = LINE, const void* context = NULL, const Zoom* zoom = NULL) {
            Scale* scale = new Scale(painter.getWidth(), painter.getHeight(), shape, context);
            scale->setZoom(zoom ? *zoom : this->zoom);
            scales.push_back(scale);
            return *scale;
        }
        
        void drawPoint(int x, int y) const {
            painter.point(x, painter.getHeight() - y);
        }

        void drawPoint(const ProjectedPoint& projectedPoint) {
            painter.point(projectedPoint.getX(), painter.getHeight() - projectedPoint.getY());
        }

        void drawPoints(const size_t scale, const Color color) const {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            painter.color(color);
            int painterHeight = painter.getHeight();
            for (const ProjectedPoint& projectedPoint: projectedPoints) 
                painter.point(projectedPoint.getX(), painterHeight - projectedPoint.getY());
        }

        void drawLine(int x1, int y1, int x2, int y2) const {
            painter.line(x1, painter.getHeight() - y1, x2, painter.getHeight() - y2);
        }

        void drawRect(int x1, int y1, int x2, int y2) const {
            painter.rect(x1, painter.getHeight() - y1, x2, painter.getHeight() - y2);
        }

        void fillRect(int x1, int y1, int x2, int y2) const {
            painter.fillRect(x1, painter.getHeight() - y1, x2, painter.getHeight() - y2);
        }

        void drawPairs(const size_t scale, const Color color, const Shape shape) const {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            painter.color(color);
            ProjectedPoint prjPoint = projectedPoints.at(0);
            int x1 = prjPoint.getX();
            int y1 = prjPoint.getY();
            int painterHeight = painter.getHeight();
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
                func(&painter, x1, painterHeight - y1, x2, painterHeight - y2);
                x1 = x2;
                y1 = y2;
            }
        }

        void drawLines(const size_t scale, const Color color) const {
            drawPairs(scale, color, LINE);
        }

        void drawRectangles(const size_t scale, const Color color) const {
            drawPairs(scale, color, BOX);
        }

        void fillRectangles(const size_t scale, const Color color) const {
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
        ) const {
            Color color = (openY < closeY) ? colorInc : colorDec;
            painterHeight = painterHeight ? painterHeight : painter.getHeight();
            painter.color(ColorMixer::light(color));
            painter.vLine(lowX, painterHeight - lowY, painterHeight - highY);
            painter.color(color);
            if (openY == closeY) {
                painter.hLine(openX, painterHeight - openY, closeX);
                return;
            }
            const int width = closeX - openX;
            if (width > 5) { openX++; closeX--; }
            if (width > 10) { openX++; closeX--; }
            if (width > 20) { openX++; closeX--; }
            if (width > 50) { openX++; closeX--; }
            if (width > 100) { openX++; closeX--; }
            painter.fillRect(openX, painterHeight - openY, closeX, painterHeight - closeY);
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
        ) const {
            drawCandle(
                open.getX(), open.getY(), // FlawFinder: ignore
                close.getX(), close.getY(),
                low.getX(), low.getY(),
                high.getY(),
                colorInc, colorDec,
                painterHeight
            );
        }

        void drawCandles(const size_t scale, const CandleStyle& candleStyle) const {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            size_t size = projectedPoints.size();
            int painterHeight = painter.getHeight();
            for (size_t i = 0; i < size; i += 4) {
                const ProjectedPoint& open = projectedPoints.at(i);
                const ProjectedPoint& close = projectedPoints.at(i + 1);
                const ProjectedPoint& low = projectedPoints.at(i + 2);
                const ProjectedPoint& high = projectedPoints.at(i + 3);
                drawCandle(open, close, low, high, candleStyle.getColorUp(), candleStyle.getColorDown(), painterHeight); // FlawFinder: ignore
                // Color color = open.getY() < close.getY() ? colorInc : colorDec;
                // painter.color(color);
                // painter.fillRect(open.getX(), painterHeight - open.getY(), close.getX(), painterHeight - close.getY());
                // painter.line(low.getX(), painterHeight - low.getY(), high.getX(), painterHeight - high.getY());
            }
        }

        void putLabel(int x, int y, const string &text, Color color = white /* TODO */, bool hasBackground = true /* TODO */, Color backgroundColor = black /* TODO */, Color borderColor = gray /* TODO */, int padding = 3, int painterHeight = -1) const {
            painterHeight = painterHeight == -1 ? painter.getHeight() : painterHeight;
            const int painterHeight_y = painterHeight - y;
            if (hasBackground) {
                Painter::TextSize textSize = painter.getTextSize(text);
                const int x1 = x - padding;
                const int y1 = painterHeight_y - textSize.height - padding;
                const int x2 = x + textSize.width + padding;
                const int y2 = painterHeight_y + padding; // + textSize.height;
                painter.color(backgroundColor);
                painter.fillRect(x1, y1, x2, y2);
                painter.color(borderColor);
                painter.rect(x1, y1, x2, y2);
            }
            painter.color(color);
            painter.write(x, painterHeight_y - 3, text); // -3??
        }

        void putLabels(size_t scale, const LabelStyle& labelStyle) const {
            vector<ProjectedPoint> projectedPoints = scales.at(scale)->getProjectedPoints();
            if (projectedPoints.empty()) return;
            vector<string> texts = scales.at(scale)->getTexts();
            if (texts.empty()) return;
            int painterHeight = painter.getHeight();
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


        void draw() const {
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

    class ChartArea {
    public:
        static void draw(void* context) {
            Area* that = (Area*)context;
            Chart* chart = (Chart*)that->getEventContext();
            if (chart) chart->draw();
        }
    };


    class MultiChart: public ChartArea {
    protected:

        const int innerBorderSize = 2; // TODO

        vector<Chart*> charts;

    public:

        ~MultiChart() {
            vector_destroy<Chart>(charts);
        }

        Chart& getChartAt(size_t chartIndex) const {
            return *charts.at(chartIndex);
        }

        Chart& addChartToArea(Area& area) {
            Chart* chart = new Chart(area);
            area.setEventContext(chart);
            area.onDrawHandlers.push_back(draw);
            charts.push_back(chart);
            return *chart;
        }

        Chart& addChartToAccordion(Accordion& accordion, const string& title = "Chart", int frameHeight = 150) {
            const Accordion::Container& container = accordion.addContainer(title, frameHeight);
            Frame& cntrFrame = container.getFrame();
            Frame* frame = new Frame(
                accordion.getGFX(), 
                innerBorderSize, innerBorderSize, 
                cntrFrame.getWidth() - innerBorderSize*2,  // accordion.getWidth() - innerBorderSize*4, 
                frameHeight - innerBorderSize*2,  // frameHeight - innerBorderSize*2,
                BUTTON_PUSHED, black
            );
            cntrFrame.child(*frame);
            return addChartToArea(*frame);
        }
    };

    class MultiChartAccordion: public MultiChart, public Accordion {
    protected:
    public:
        using Accordion::Accordion;

        Chart& addChart(const string& title = "Chart", int frameHeight = 150) {
            return addChartToAccordion(*this, title, frameHeight);
        }
    };

    // -- VIRTUAL PLUGINS --

    class ChartPlugin {
    public:
        virtual void project(Chart& /*chart*/, const void* /*context*/) const {
            throw ERR_UNIMP;
        };
    };

    class MultiChartPlugin {
    public:
        virtual void project(MultiChart& /*multiChart*/, const void* /*context*/) const {
            throw ERR_UNIMP;
        };
    };

    class MultiChartAccordionPlugin {
    public:
        virtual void project(MultiChartAccordion& /*multiChartAccordion*/, const void* /*context*/) const {
            throw ERR_UNIMP;
        };
    };

}