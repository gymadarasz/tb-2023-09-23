#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
// #include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace madlib::graph;
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

        CandleSeries* candleSeries1 = chart1.createCandleSeries();
        PointSeries* pointSeries1 = chart1.createPointSeries(candleSeries1, true, yellow);
        PointSeries* pointSeries2 = chart1.createPointSeries(candleSeries1, true, blue);
        PointSeries* pointSeries3 = chart1.createPointSeries(candleSeries1, true, green);
        LabelSeries* labelSeries1 = chart1.createLabelSeries(candleSeries1);

        vector<Shape*>& candles1 = candleSeries1->getShapes();
        vector<Shape*>& points1 = pointSeries1->getShapes();
        vector<Shape*>& points2 = pointSeries2->getShapes();
        vector<Shape*>& points3 = pointSeries3->getShapes();
        vector<Shape*>& labels1 = labelSeries1->getShapes();

        double close = randd(0, 100);
        const ms_t step = hour;
        for (ms_t at = chart1.getTimeRange().begin; at <= chart1.getTimeRange().end; at += step) {
            double open = close + randd(-3, 3);
            double low = open - randd(0, 50);
            double high = open + randd(-50, 100);
            close = high - randd(0, 50);
            ms_t end = at + step - 1;
            candles1.push_back(chart1.createCandleShape(at, end, open, low, high, close));

            if (randi(0, 10) == 0) {
                bool side = randi(0, 1) == 0;
                labels1.push_back(
                    chart1.createLabelShape(
                        end, close, 
                        side ? "Buy" : "Sell", 
                        side ? red : green
                    )
                );
            }

            points1.push_back(chart1.createPointShape(at, randd(0, 1000)));
            points2.push_back(chart1.createPointShape(at, randd(500, 800)));
            points3.push_back(chart1.createPointShape(at, randd(100, 200)));
        }

        mainFrame.child(label1);
        mainFrame.child(chart1);
    }

    virtual ~LabelManualTest1() {}
};
