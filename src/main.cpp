#include "includes/madlib/graph/graph.hpp"

using namespace std;
using namespace madlib::graph;

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

class Scale {

    double xmin, ymin, xmax, ymax, zoomX = 1, zoomY = 1;
    int width = 0, height = 0;

public:

    Scale(int width, int height): width(width), height(height) {
        reset();
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

    void reset() {
        xmin = INFINITY;
        ymin = INFINITY;
        xmax = -INFINITY;
        ymax = -INFINITY;
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

    vector<ProjectedPoint> project(const vector<RealPoint>& realPoints, vector<ProjectedPoint>& resultProjectedPoints) {
        resultProjectedPoints.clear();
        adaptXY(realPoints);
        ProjectedPoint projectedPoint;
        for (const RealPoint& realPoint: realPoints) resultProjectedPoints.push_back(project(realPoint.getX(), realPoint.getY(), projectedPoint));
        return resultProjectedPoints;
    }
};

class Chart {
protected:

    Painter& painter;
    Color borderColor = white;
    
    vector<Scale> scales;

public:

    Chart(Painter& painter, double zoomX = 1, double zoomY = 1): painter(painter) {
        addScale(zoomX, zoomY);
    }

    const vector<Scale>& getScales() const {
        return scales;
    }

    Scale& getScaleAt(size_t at) {
        return scales[at];
    }

    size_t addScale(double zoomX = 1, double zoomY = 1) {
        Scale scale(painter.getWidth(), painter.getHeight());
        scale.setZoomXY(zoomX, zoomY);
        scales.push_back(scale);
        return scales.size() - 1;
    }
    
    void drawPoint(int x, int y) {
        painter.point(x, painter.getHeight() - y);
    }

    void drawPoint(const ProjectedPoint& projectedPoint) {
        painter.point(projectedPoint.getX(), painter.getHeight() - projectedPoint.getY());
    }

    void drawPoints(const vector<ProjectedPoint>& projectedPoints, Color color) {
        painter.color(color);
        int painterHeight = painter.getHeight();
        for (const ProjectedPoint& projectedPoint: projectedPoints) 
            painter.point(projectedPoint.getX(), painterHeight - projectedPoint.getY());
    }

    void drawLine(int x1, int y1, int x2, int y2) {
        painter.line(x1, painter.getHeight() - y1, x2, painter.getHeight() - y2);
    }

    void drawLines(const vector<ProjectedPoint>& projectedPoints, Color color) {
        painter.color(color);
        if (projectedPoints.empty()) return;
        int x1 = projectedPoints[0].getX();
        int y1 = projectedPoints[0].getY();
        int painterHeight = painter.getHeight();
        for (const ProjectedPoint& projectedPoint: projectedPoints) {
            int x2 = projectedPoint.getX();
            int y2 = projectedPoint.getY();
            painter.line(x1, painterHeight - y1, x2, painterHeight - y2);
            x1 = x2;
            y1 = y2;
        }

    }
};

const double scaleXMin = 0;
const double scaleXMax = 1000;
const double zoomX = 2;

GFX gfx;
GUI gui(&gfx, 800, 600);
Frame frame(&gfx, 50, 50, 700, 500);
Chart chart(frame, zoomX);

vector<ProjectedPoint> projectedPoints0;
vector<ProjectedPoint> projectedPoints1;

void draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart.drawLines(projectedPoints0, lightGreen);
    chart.drawPoints(projectedPoints0, white);

    chart.drawLines(projectedPoints1, lightCyan);
}

void generateRealPoints(vector<RealPoint>& realPoints) {
    realPoints.clear();

    double y_ = randd(0, 5);
    for (double x = scaleXMin; x <= scaleXMax; x += 1) {

        double y = y_ + randd(0, 5);
        y_++;
        if (y_ > 100 + randd(0, 5)) y_ = randd(0, 5);

        RealPoint realPoint(x, y);
        // scale.adaptXY(realPoint);
        realPoints.push_back(realPoint);
    }
}

int main()
{
    // set up UI
    frame.fixed = false;
    frame.setBackgroundColor(black);
    gui.child(&frame);
    frame.onDrawHandlers.push_back(draw);
    
    // generate data and show on scales
    vector<RealPoint> realPoints;
    generateRealPoints(realPoints);

    chart.getScaleAt(0).project(realPoints, projectedPoints0);

    chart.addScale();
    chart.getScaleAt(1).project(realPoints, projectedPoints1);
    

    gui.loop();
    
    return 0;
}
