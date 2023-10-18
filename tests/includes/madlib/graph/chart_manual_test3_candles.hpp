#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

GFX* chart_manual_test3_gfxPtr;
Chart* chart_manual_test3_chartPtr;
Button* chart_manual_test3_closeOkBtnPtr;

void chart_manual_test3_draw(void* /*context*/) {
    // Painter* painter = (Painter*)context;
    chart_manual_test3_chartPtr->draw();
    // chart_manual_test3_chartPtr->drawCandle(10, 10, 20, 20, 15, 25, 15, 5);
    chart_manual_test3_closeOkBtnPtr->draw();
}

void chart_manual_test3_close(void*, unsigned int, int, int) {
    chart_manual_test3_gfxPtr->close = true;
}

int chart_manual_test3_candles()
{
    GFX gfx;
    chart_manual_test3_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600);
    Frame frame(gfx, 10, 10, 780, 580, BUTTON_PUSHED, black);
    gui.child(frame);
    Chart chart(frame);
    chart_manual_test3_chartPtr = &chart;
    frame.onDrawHandlers.push_back(chart_manual_test3_draw);


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test3_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(chart_manual_test3_close);
    gui.child(closeOkBtn);


    vector<RealPoint> candlesRealPoints;
    vector<RealPoint> pricesRealPoints;
    double price = 1000;
    double time = 0;
    for (int i = 0; i < 30; i++) {
        vector<double> rands = randd_norm_dist(0, 1, 10);
        vector<double> cabdlePrices;
        double start = time;
        for (size_t j = 0; j < rands.size(); j++) {
            price += rands[j];
            time++;
            pricesRealPoints.push_back(RealPoint((double)time, price));
            cabdlePrices.push_back(price);
        }
        double end = time;
        double open = cabdlePrices[0];
        double close = cabdlePrices[cabdlePrices.size() - 1];
        double low = min(cabdlePrices);
        double high = max(cabdlePrices);
        
        double middle = start + (end - start) / 2;
        candlesRealPoints.push_back(RealPoint((double)start, open));
        candlesRealPoints.push_back(RealPoint((double)end, close));
        candlesRealPoints.push_back(RealPoint((double)middle, low));
        candlesRealPoints.push_back(RealPoint((double)middle, high));
    }
    chart.getScaleAt(0).setShape(CANDLE);
    chart.getScaleAt(0).project(candlesRealPoints);
    chart.addScale(1, 1, LINE, orange);
    chart.getScaleAt(1).project(pricesRealPoints);

    gui.loop();
    
    return 0;
}
