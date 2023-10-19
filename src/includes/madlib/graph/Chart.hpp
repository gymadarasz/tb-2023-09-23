#pragma once

#include "graph.hpp"

using namespace std;

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

    enum Shape {
        DOT, LINE, BOX, FILLED, CANDLE, TEXT
    };

    class Scale {
    protected:

        double xmin, ymin, xmax, ymax, zoomX = 1, zoomY = 1;
        int width, height;
        Color color;
        Shape shape;

        vector<ProjectedPoint> projectedPoints;
        vector<string> texts;

        int projectX(double x) {
            adaptX(x);
            return (int)(((x*zoomX - xmin) * width) / (xmax - xmin));
        }

        int projectY(double y) {
            adaptY(y);
            return (int)(((y*zoomY - ymin) * height) / (ymax - ymin));
        }
        
        ProjectedPoint& project(double x, double y, ProjectedPoint& result) {
            adaptXY(x, y);
            result.setX(projectX(x));
            result.setY(projectY(y));
            return result;
        }
        
    public:

        Scale(int width, int height, Color color = lightGreen, Shape shape = LINE): 
            width(width), height(height), color(color), shape(shape)
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

        Color getColor() const {
            return color;
        }

        Shape getShape() const {
            return shape;
        }

        void setShape(Shape shape) {
            this->shape = shape;
        }

        const vector<ProjectedPoint>& getProjectedPoints() const {
            return projectedPoints;
        }

        const vector<string>& getTexts() const {
            return texts;
        }

        void setZoomX(double zoomX) {
            this->zoomX = zoomX;
        }

        void setZoomY(double zoomY) {
            this->zoomY = zoomY;
        }

        void setZoomXY(double zoomX, double zoomY) {
            this->zoomX = zoomX;
            this->zoomY = zoomY;
        }

        void setXMinMax(double min, double max) {
            xmin = min;
            xmax = max;
        }

        void setYMinMax(double min, double max) {
            ymin = min;
            ymax = max;
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

        RealPoint& zoomToX(const RealPoint realPoint, RealPoint& resultRealPoint) {
            resultRealPoint.setX(realPoint.getX() * zoomX);
            return resultRealPoint;
        }

        RealPoint& zoomToY(const RealPoint realPoint, RealPoint& resultRealPoint) {
            resultRealPoint.setY(realPoint.getY() * zoomY);
            return resultRealPoint;
        }

        RealPoint& zoomToXY(const RealPoint realPoint, RealPoint& resultRealPoint) {
            resultRealPoint.setX(realPoint.getX() * zoomX);
            resultRealPoint.setY(realPoint.getY() * zoomY);
            return resultRealPoint;
        }

        vector<RealPoint>& zoomToX(const vector<RealPoint> realPoints, vector<RealPoint>& resultRealPoints) {
            resultRealPoints.clear();
            for (const RealPoint& realPoint: realPoints) {
                RealPoint resultRealPoint(realPoint.getX() * zoomX, realPoint.getY());
                resultRealPoints.push_back(resultRealPoint);
            }
            return resultRealPoints;
        }

        vector<RealPoint>& zoomToY(const vector<RealPoint> realPoints, vector<RealPoint>& resultRealPoints) {
            resultRealPoints.clear();
            for (const RealPoint& realPoint: realPoints) {
                RealPoint resultRealPoint(realPoint.getY(), realPoint.getY() * zoomY);
                resultRealPoints.push_back(resultRealPoint);
            }
            return resultRealPoints;
        }

        vector<RealPoint>& zoomToXY(const vector<RealPoint> realPoints, vector<RealPoint>& resultRealPoints) {
            resultRealPoints.clear();
            for (const RealPoint& realPoint: realPoints) {
                RealPoint resultRealPoint(realPoint.getX() * zoomX, realPoint.getY() * zoomY);
                resultRealPoints.push_back(resultRealPoint);
            }
            return resultRealPoints;
        }

        vector<ProjectedPoint>& project(const vector<RealPoint>& realPoints) {
            projectedPoints.clear();
            adaptXY(realPoints);
            ProjectedPoint projectedPoint;
            for (const RealPoint& realPoint: realPoints) projectedPoints.push_back(project(realPoint.getX(), realPoint.getY(), projectedPoint));
            return projectedPoints;
        }

        void project(const vector<RealPoint>& realPoints, const vector<string> texts) {
            project(realPoints);
            this->texts = texts;
        }
    };

    class Chart {
    protected:

        Painter& painter;
        Color borderColor = white;
        
        vector<Scale> scales;

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

        Chart(Painter& painter, double zoomX = 1, double zoomY = 1): painter(painter) {
            addScale(zoomX, zoomY);
        }

        const vector<Scale>& getScales() const {
            return scales;
        }

        Scale& getScaleAt(size_t at) {
            if (scales.size() <= at) throw runtime_error("Invalid index: " + to_string(at));
            return scales[at];
        }

        size_t addScale(double zoomX = 1, double zoomY = 1, Shape shape = LINE, Color color = white) {
            Scale scale(painter.getWidth(), painter.getHeight(), color, shape);
            scale.setZoomXY(zoomX, zoomY);

            scales.push_back(scale);
            return scales.size() - 1;
        }
        
        void drawPoint(int x, int y) const {
            painter.point(x, painter.getHeight() - y);
        }

        void drawPoint(const ProjectedPoint& projectedPoint) {
            painter.point(projectedPoint.getX(), painter.getHeight() - projectedPoint.getY());
        }

        void drawPoints(const size_t scale, const Color color) const {
            vector<ProjectedPoint> projectedPoints = scales[scale].getProjectedPoints();
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
            vector<ProjectedPoint> projectedPoints = scales[scale].getProjectedPoints();
            if (projectedPoints.empty()) return;
            painter.color(color);
            int x1 = projectedPoints[0].getX();
            int y1 = projectedPoints[0].getY();
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
                    throw runtime_error("Invalid pair shape: " + shape);
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
            painter.color(color);
            painter.vLine(lowX, painterHeight - lowY, painterHeight - highY);
            if (openY == closeY) {
                painter.hLine(openX, painterHeight - openY, closeX);
                return;
            }
            painter.fillRect(openX, painterHeight - openY, closeX, painterHeight - closeY);
        }

        void drawCandle(
            ProjectedPoint open,
            ProjectedPoint close,
            ProjectedPoint low,
            ProjectedPoint high,
            const Color colorInc = green, 
            const Color colorDec = red,
            int painterHeight = 0
        ) const {
            drawCandle(
                open.getX(), open.getY(),
                close.getX(), close.getY(),
                low.getX(), low.getY(),
                high.getY(),
                colorInc, colorDec,
                painterHeight
            );
        }

        void drawCandles(const size_t scale, const Color colorInc = green, const Color colorDec = red) const {
            vector<ProjectedPoint> projectedPoints = scales[scale].getProjectedPoints();
            if (projectedPoints.empty()) return;
            size_t size = projectedPoints.size();
            int painterHeight = painter.getHeight();
            for (size_t i = 0; i < size; i += 4) {
                const ProjectedPoint& open = projectedPoints[i];
                const ProjectedPoint& close = projectedPoints[i + 1];
                const ProjectedPoint& low = projectedPoints[i + 2];
                const ProjectedPoint& high = projectedPoints[i + 3];
                drawCandle(open, close, low, high, colorInc, colorDec, painterHeight);
                // Color color = open.getY() < close.getY() ? colorInc : colorDec;
                // painter.color(color);
                // painter.fillRect(open.getX(), painterHeight - open.getY(), close.getX(), painterHeight - close.getY());
                // painter.line(low.getX(), painterHeight - low.getY(), high.getX(), painterHeight - high.getY());
            }
        }

        void writeText(int x, int y, const string text) const {
            int painterHeight = painter.getHeight();
            painter.write(x, painterHeight - y, text);
        }

        void writeTexts(size_t scale, Color color) const {
            vector<ProjectedPoint> projectedPoints = scales[scale].getProjectedPoints();
            if (projectedPoints.empty()) return;
            vector<string> texts = scales[scale].getTexts();
            if (texts.empty()) return;
            int painterHeight = painter.getHeight();
            painter.color(color);
            size_t size = projectedPoints.size();
            for (size_t i = 0; i < size; i++) {
                painter.write(projectedPoints[i].getX(), painterHeight - projectedPoints[i].getY(), texts[i]);
            }
        }

        // -------- 


        void draw() const {
            size_t at = 0;
            for (const Scale& scale: scales) {
                const Shape shape = scale.getShape();
                const Color color = scale.getColor();
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
                        drawCandles(at);
                        break;

                    case TEXT:
                        writeTexts(at, color);
                        break;
                    
                    default:
                        throw runtime_error("invalid shape type: " + to_string((int)shape));
                        break;
                    }
                at++;
            }
        }

    };
    
}