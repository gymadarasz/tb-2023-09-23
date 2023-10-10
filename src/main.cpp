#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

Chart* chartPtr;

void draw(void*) {
    //chartPtr->drawLines(0, black);

    chartPtr->draw();
}

int main()
{
    GFX gfx;
    GUI gui(gfx, 800, 600, black);
    Chart chart(gui);
    chartPtr = &chart;
    gui.onDrawHandlers.push_back(draw);
    chart.addScale(1, 1, TEXT, black);

    vector<RealPoint> realPoints;
    vector<RealPoint> textRealChoords;
    vector<string> texts;
    int i = 0;
    for (double x = 0; x < 100; x++) {
        double y = randd(0, 100) + x*5;
        RealPoint realPoint(x, y);
        realPoints.push_back(realPoint);
        i++;
        if (i > 10) {
            i = 0;
            textRealChoords.push_back(realPoint);
            texts.push_back(to_string(x));
        }
    }
    

    chart.getScaleAt(0).project(realPoints);
    chart.getScaleAt(1).project(textRealChoords, texts);


    gui.loop();
    
    return 0;
}
