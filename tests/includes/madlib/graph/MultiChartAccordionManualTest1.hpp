#pragma once

#include "../../ManualTestApplication.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"

class MultiChartAccordionManualTest1: public ManualTestApplication {
protected:

    Frame frame1 = Frame(gfx, 10, 100, 300, 180, BUTTON_PUSHED, black);
    Frame frame2 = Frame(gfx, 10, 300, 300, 180, BUTTON_PUSHED, black);
    Frame frame3 = Frame(gfx, 10, 500, 300, 180, BUTTON_PUSHED, black);

    Accordion accordion = Accordion(gfx, 350, 10, 300);

    MultiChart multiChart;

    MultiChartAccordion multiChartAccordion = MultiChartAccordion(gfx, 700, 10, 850);

    Frame testFrame = Frame(gfx, 700, 400, 850, 300, BUTTON_PUSHED, black);
    Chart testChart = Chart(testFrame);

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

        multiChart.addChartToArea(frame1).addScale().project(generateRealPoints(100));
        multiChart.addChartToArea(frame2).addScale().project(generateRealPoints(100));
        multiChart.addChartToArea(frame3).addScale().project(generateRealPoints(100));
        gui.child(frame1);
        gui.child(frame2);
        gui.child(frame3);
        
        multiChart.addChartToAccordion(accordion, "First Chart", 240).addScale().project(generateRealPoints(100));
        multiChart.addChartToAccordion(accordion, "Second Chart", 240).addScale().project(generateRealPoints(100));
        multiChart.addChartToAccordion(accordion, "Third Chart", 240).addScale().project(generateRealPoints(100));        
        accordion.openAll();
        gui.child(accordion);

        multiChartAccordion.addChart("Accordion First Chart", 240).addScale().project(generateRealPoints(100));
        multiChartAccordion.addChart("Accordion Second Chart", 240).addScale().project(generateRealPoints(100));
        multiChartAccordion.addChart("Accordion Third Chart", 240).addScale().project(generateRealPoints(100));
        multiChartAccordion.setOne(true);
        multiChartAccordion.openAt(0);
        gui.child(multiChartAccordion);

        Scale& scale0 = testChart.addScale(LINE, lightBlue);
        Scale& scale1 = testChart.addScale(LINE, lightGreen);
        vector<RealPoint> realPoints0 = generateRealPoints(100, -1, 1);
        vector<RealPoint> realPoints1 = generateRealPoints(100, 0, 1);
        scale0.adaptXY(realPoints0);
        scale1.adaptXY(realPoints1);
        Scale::alignXY(scale0, scale1);
        scale0.project(realPoints0, false);
        scale1.project(realPoints1, false);
        
        testFrame.setEventContext(&testChart);
        testFrame.onDrawHandlers.push_back(ChartArea::draw);
        gui.child(testFrame);
    }
};
