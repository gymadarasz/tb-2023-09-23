#pragma once

#include "../../../../src/includes/madlib/madlib.hpp"
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
    GFX gfx;
    chart_manual_test3_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test3_candles");
    Chart chart(gfx, 10, 10, 780, 580, PUSHED, black);
    gui.child(chart);
    chart_manual_test3_chartPtr = &chart;
    chart.addDrawHandler(chart_manual_test3_draw);


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test3_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test3_close);
    gui.child(closeOkBtn);


    vector<Coord> candlesRealPoints;
    vector<Coord> pricesRealPoints;
    double price = 1000;
    double time = 0;
    for (int i = 0; i < 30; i++) {
        vector<double> rands = rand_norm_dist<double>(0.0, 1.0, 10);
        vector<double> candlePrices;
        double start = time;
        for (size_t j = 0; j < rands.size(); j++) {
            price += rands[j];
            time++;
            pricesRealPoints.push_back(Coord((double)time, price));
            candlePrices.push_back(price);
        }
        double end = time;
        double open = candlePrices[0];
        double close = candlePrices[candlePrices.size() - 1];
        double low = min(candlePrices);
        double high = max(candlePrices);
        
        double middle = start + (end - start) / 2;
        candlesRealPoints.push_back(Coord((double)start, open));
        candlesRealPoints.push_back(Coord((double)end, close));
        candlesRealPoints.push_back(Coord((double)middle, low));
        candlesRealPoints.push_back(Coord((double)middle, high));
    }
    chart.createScale(LINE);
    chart.getScaleAt(0).setShape(CANDLE);
    chart.getScaleAt(0).project(candlesRealPoints);
    chart.createScale(LINE, &orange);
    chart.getScaleAt(1).project(pricesRealPoints);

    gui.loop();
    
    return 0;
}
