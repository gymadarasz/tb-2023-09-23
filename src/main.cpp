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

    double xmin, ymin, xmax, ymax; 
    int width = 0, height = 0;

public:

    Scale(int width, int height): width(width), height(height) {
        reset();
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

    int projectX(double x) const {
        return (int)(((x - xmin) * width) / (xmax - xmin));
    }

    int projectY(double y) const {
        return (int)(((y - ymin) * height) / (ymax - ymin));
    }
    
    ProjectedPoint& project(double x, double y, ProjectedPoint& result) const {
        result.setX(projectX(x));
        result.setY(projectY(y));
        return result;
    }

    vector<ProjectedPoint> project(vector<RealPoint> realPoints, vector<ProjectedPoint>& resultProjectedPoints) const {
        resultProjectedPoints.clear();
        ProjectedPoint projectedPoint;
        for (const RealPoint& realPoint: realPoints) resultProjectedPoints.push_back(project(realPoint.getX(), realPoint.getY(), projectedPoint));
        return resultProjectedPoints;
    }
};

class Chart {
protected:

    const Painter& painter;
    Color borderColor = white;
    
public:

    Chart(const Painter& painter): painter(painter) {}
    
    void drawPoint(int x, int y) {
        painter.point(x, y);
    }

    void drawPoint(const ProjectedPoint& projectedPoint) {
        painter.point(projectedPoint.getX(), projectedPoint.getY());
    }

    void drawPoints(vector<ProjectedPoint> projectedPoints, Color color) {
        painter.color(color);
        for (const ProjectedPoint& projectedPoint: projectedPoints) drawPoint(projectedPoint);
    }

    void drawLine(int x1, int y1, int x2, int y2) {
        painter.line(x1, y1, x2, y2);
    }

    void drawLines(vector<ProjectedPoint> projectedPoints, Color color) {
        painter.color(color);
        if (projectedPoints.empty()) return;
        int x1 = projectedPoints[0].getX();
        int y1 = projectedPoints[0].getY();
        for (const ProjectedPoint& projectedPoint: projectedPoints) {
            int x2 = projectedPoint.getX();
            int y2 = projectedPoint.getY();
            drawLine(x1, y1, x2, y2);
            x1 = x2;
            y1 = y2;
        }

    }
};

const double scaleXMin = 0;
const double scaleXMax = 1000;

GFX gfx;
GUI gui(&gfx, 800, 600);
Frame frame(&gfx, 50, 50, 700, 500);
Chart chart(frame);

vector<ProjectedPoint> projectedPoints;

void draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart.drawLines(projectedPoints, lightGreen);
    // chart.drawPoints(projectedPoints, white);
}

int main()
{
    frame.fixed = false;
    frame.setBackgroundColor(black);

    gui.child(&frame);
    frame.onDrawHandlers.push_back(draw);

    Scale scale(frame.getWidth(), frame.getHeight());
    scale.setXMinMax(scaleXMin, scaleXMax);
    // scale.setYMinMax(0, 100);
    
    vector<RealPoint> realPoints;
    const double zoom = 20;
    for (double x = scaleXMin; x <= scaleXMax; x += 1) {
        double y = randd(0, 100);
        RealPoint realPoint(x*zoom, y);
        realPoints.push_back(realPoint);
        scale.adaptY(y);
    }
    frame.setScrollXYMax(scale.projectX(scaleXMax*zoom), 500);

    scale.project(realPoints, projectedPoints);

    gui.loop();
    
    return 0;
}
