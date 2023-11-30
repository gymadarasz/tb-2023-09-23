#pragma once

#include "../../ManualTestApplication.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

class MultiChartAccordionManualTest1: public ManualTestApplication {
protected:

    Chart chart1 = Chart(gfx, zoom, 10, 100, 300, 180, PUSHED, black);
    Chart chart2 = Chart(gfx, zoom, 10, 300, 300, 180, PUSHED, black);
    Chart chart3 = Chart(gfx, zoom, 10, 500, 300, 180, PUSHED, black);

    MultiChartAccordion accordion = MultiChartAccordion(gfx, zoom, 350, 10, 300);

    // MultiChart multiChart;

    MultiChartAccordion multiChartAccordion = MultiChartAccordion(gfx, zoom, 700, 10, 850);

    Chart testChart = Chart(gfx, zoom, 700, 400, 850, 300, PUSHED, black);

    vector<RealPoint> generateRealPoints(size_t n, double min = 0, double max = 1) {
        vector<RealPoint> realPoints;
        for (size_t i = 0; i < n; i++) {
            double x = (double)i, y = randd(min, max);
            RealPoint realPoint(x, y);
            realPoints.push_back(realPoint);
        }
        return realPoints;
    }

public:
    void init() override {
        ManualTestApplication::init();
        gui.setTitle("MultiChartAccordionManualTest1");

        chart1.createScale(zoom)->project(generateRealPoints(100));
        chart2.createScale(zoom)->project(generateRealPoints(100));
        chart3.createScale(zoom)->project(generateRealPoints(100));
        mainFrame.child(chart1);
        mainFrame.child(chart2);
        mainFrame.child(chart3);
        
        accordion.createChart("First Chart", 240).createScale(zoom)->project(generateRealPoints(100));
        accordion.createChart("Second Chart", 240).createScale(zoom)->project(generateRealPoints(100));
        accordion.createChart("Third Chart", 240).createScale(zoom)->project(generateRealPoints(100));        
        accordion.openAll();
        mainFrame.child(accordion);

        multiChartAccordion.createChart("Accordion First Chart", 240).createScale(zoom)->project(generateRealPoints(100));
        multiChartAccordion.createChart("Accordion Second Chart", 240).createScale(zoom)->project(generateRealPoints(100));
        multiChartAccordion.createChart("Accordion Third Chart", 240).createScale(zoom)->project(generateRealPoints(100));
        multiChartAccordion.setOne(true);
        multiChartAccordion.openAt(0);
        mainFrame.child(multiChartAccordion);

        Chart::Scale* scale0 = testChart.createScale(zoom, LINE, &lightBlue);
        Chart::Scale* scale1 = testChart.createScale(zoom, LINE, &lightGreen);
        vector<RealPoint> realPoints0 = generateRealPoints(100, -1, 1);
        vector<RealPoint> realPoints1 = generateRealPoints(100, 0, 1);
        scale0->adaptXY(realPoints0);
        scale1->adaptXY(realPoints1);
        Chart::Scale::alignXY(*scale0, *scale1);
        scale0->project(realPoints0, false);
        scale1->project(realPoints1, false);
        
        // testFrame.setEventContext(&testChart);
        // testFrame.addDrawHandler(Chart::drawHandler);
        mainFrame.child(testChart);
    }
};
