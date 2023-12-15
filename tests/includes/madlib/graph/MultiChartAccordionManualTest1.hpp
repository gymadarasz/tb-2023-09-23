#pragma once

#include "../../ManualTestApplication.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"


class MultiChartAccordionManualTest1: public ManualTestApplication {
protected:
    const ms_t chartStart = datetime_to_ms("2020-01-01");
    const ms_t chartFinish = datetime_to_ms("2020-01-07");

    Chart::MultiChart multiChart1 = Chart::MultiChart();
    Chart::MultiChart multiChart2 = Chart::MultiChart();
    // Chart::MultiChart multiChart3 = Chart::MultiChart();

    Chart chart1 = Chart(gfx, 10, 100, 500, 180, chartStart, chartFinish);
    Chart chart2 = Chart(gfx, 10, 300, 500, 180, chartStart, chartFinish);
    Chart chart3 = Chart(gfx, 10, 500, 500, 180, chartStart, chartFinish);

    MultiChartAccordion accordion = MultiChartAccordion(
        gfx, 550, 10, 500, 
        chartStart, chartFinish);

    // MultiChart multiChart;

    MultiChartAccordion multiChartAccordion = MultiChartAccordion(
        gfx, 1100, 10, 850, 
        chartStart, chartFinish);

    Chart testChart = Chart(
        gfx, 1100, 400, 850, 300, 
        chartStart, chartFinish);

    void generateRealPoints(Chart& chart, vector<Shape*>& points, double min = 0, double max = 1, ms_t step = hour) {
        for (ms_t i = chart.getTimeRange().begin; i < chart.getTimeRange().end; i+=step) {
            double y = randd(min, max);
            points.push_back(chart.createPointShape(i, y));
        }
    }

public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~MultiChartAccordionManualTest1() {}

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("MultiChartAccordionManualTest1");

        generateRealPoints(chart1, chart1.createPointSeries()->getShapes());
        generateRealPoints(chart2, chart2.createPointSeries()->getShapes());
        generateRealPoints(chart3, chart3.createPointSeries()->getShapes());
        multiChart1.attach(chart1);
        multiChart1.attach(chart2);
        multiChart1.attach(chart3);
        mainFrame.child(chart1);
        mainFrame.child(chart2);
        mainFrame.child(chart3);
        
        Chart* chart21 = accordion.createChart("First Chart", 240);
        generateRealPoints(*chart21, chart21->createPointSeries()->getShapes());
        Chart* chart22 = accordion.createChart("Second Chart", 240);
        generateRealPoints(*chart22, chart22->createPointSeries()->getShapes());
        Chart* chart23 = accordion.createChart("Third Chart", 240);
        generateRealPoints(*chart23, chart23->createPointSeries()->getShapes());        
        accordion.openAll(true);
        multiChart2.attach(*chart21);
        multiChart2.attach(*chart22);
        multiChart2.attach(*chart23);
        mainFrame.child(accordion);

        Chart* chart31 = multiChartAccordion.createChart("Accordion First Chart", 240);
        generateRealPoints(*chart31, chart31->createPointSeries()->getShapes());
        Chart* chart32 = multiChartAccordion.createChart("Accordion Second Chart", 240);
        generateRealPoints(*chart32, chart32->createPointSeries()->getShapes());
        Chart* chart33 = multiChartAccordion.createChart("Accordion Third Chart", 240);
        generateRealPoints(*chart33, chart33->createPointSeries()->getShapes());
        multiChartAccordion.setSingle(true, false);
        multiChartAccordion.openAt(0, true);
        mainFrame.child(multiChartAccordion);

        PointSeries* scale0 = testChart.createPointSeries(nullptr, true, lightBlue);
        PointSeries* scale1 = testChart.createPointSeries(scale0, true, lightGreen);
        generateRealPoints(testChart, scale0->getShapes(), -1, 1);
        generateRealPoints(testChart, scale1->getShapes(), 0, 1);
        multiChartAccordion.getMultiChart().attach(testChart);
        
        mainFrame.child(testChart);
    }
};
