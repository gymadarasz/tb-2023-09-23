#pragma once

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

using namespace std;
using namespace madlib::graph;

GFX* chart_manual_test3_gfxPtr;
Chart* chart_manual_test3_chartPtr;
Button* chart_manual_test3_closeOkBtnPtr;

void chart_manual_test3_draw(void*) {
    chart_manual_test3_closeOkBtnPtr->draw();
}

void chart_manual_test3_close(void*, unsigned int, int, int) {
    chart_manual_test3_gfxPtr->close = true;
}

int chart_manual_test3_candles()
{
    const ms_t chartStart = datetime_to_ms("2020-01-01");
    const ms_t chartFinish = datetime_to_ms("2020-01-07");
    GFX gfx;
    chart_manual_test3_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test3_candles");
    Chart chart(gfx, 10, 10, 780, 580, chartStart, chartFinish);
    gui.child(chart);
    chart_manual_test3_chartPtr = &chart;
    chart.addDrawHandler(chart_manual_test3_draw);


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test3_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test3_close);
    gui.child(closeOkBtn);

    CandleSeries* candleSeries = chart.createCandleSeries();
    vector<Shape*>& candlesRealPoints = candleSeries->getShapes();
    vector<Shape*>& pricesRealPoints = chart.createPointSeries(candleSeries)->getShapes();
    double price = 1000;
    ms_t time = chartStart;
    for (int i = 0; i < 30; i++) {
        vector<double> rands = rand_norm_dist<double>(0.0, 1.0, 10);
        vector<double> candlePrices;
        ms_t start = time;
        for (size_t j = 0; j < rands.size(); j++) {
            price += rands[j];
            time+=hour;
            pricesRealPoints.push_back(chart.createPointShape(time, price));
            candlePrices.push_back(price);
        }
        ms_t end = time;
        double open = candlePrices[0];
        double close = candlePrices[candlePrices.size() - 1];
        double low = vector_min(candlePrices);
        double high = vector_max(candlePrices);
        
        candlesRealPoints.push_back(chart.createCandleShape(start, end, open, low, high, close));
    }

    gui.loop();
    
    return 0;
}
