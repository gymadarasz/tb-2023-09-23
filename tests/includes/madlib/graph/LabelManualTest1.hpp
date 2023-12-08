#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/chart/Chart.hpp"
// #include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
using namespace madlib::graph::chart;
// using namespace madlib::trading;

class LabelManualTest1: public ManualTestApplication {
protected:
        
    Label label1 = Label(
        gfx, 10, 50, 200, 30, 
        "Hello World!"
    );

    Chart chart1 = Chart(
        gfx, 10, 100, 1500, 300,
        datetime_to_ms("2020-01-01"), 
        datetime_to_ms("2020-01-06")
    );

public:

    void init() override {
        ManualTestApplication::init();
        gui.setTitle("LabelManualTest1");

        rand_init_seed(42);

        CandleSeries* candleSeries1 = chart1.createCandleSeries(/*candles1*/);
        PointSeries* pointSeries1 = chart1.createPointSeries(/*points1, */CANDLE, candleSeries1, true, yellow);
        PointSeries* pointSeries2 = chart1.createPointSeries(/*points2, */CANDLE, candleSeries1, true, blue);
        PointSeries* pointSeries3 = chart1.createPointSeries(/*points3, */CANDLE, candleSeries1, true, green);
        LabelSeries* labelSeries1 = chart1.createLabelSeries(/*labels1, */CANDLE, candleSeries1);

        vector<CandleShape>& candles1 = candleSeries1->getShapes();
        vector<PointShape>& points1 = pointSeries1->getShapes();
        vector<PointShape>& points2 = pointSeries2->getShapes();
        vector<PointShape>& points3 = pointSeries3->getShapes();
        vector<LabelShape>& labels1 = labelSeries1->getShapes();

        double close = randd(0, 100);
        const ms_t step = hour;
        for (ms_t at = chart1.getTimeRange().begin; at <= chart1.getTimeRange().end; at += step) {
            double open = close + randd(-3, 3);
            double low = open - randd(0, 50);
            double high = open + randd(-50, 100);
            close = high - randd(0, 50);
            ms_t end = at + step - 1;
            candles1.push_back(CandleShape(at, end, open, low, high, close));

            if (randi(0, 10) == 0) {
                bool side = randi(0, 1) == 0;
                labels1.push_back(
                    LabelShape(
                        end, close, 
                        side ? "Buy" : "Sell", 
                        side ? red : green
                    )
                );
            }

            points1.push_back(PointShape(at, randd(0, 1000)));
            points2.push_back(PointShape(at, randd(500, 800)));
            points3.push_back(PointShape(at, randd(100, 200)));
        }

        mainFrame.child(label1);
        mainFrame.child(chart1);
    }

    virtual ~LabelManualTest1() {}
};
