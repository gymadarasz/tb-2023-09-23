#pragma once

#include "MultiChartAccordion.hpp"

namespace madlib::graph {

    class ChartInjector { // TODO: YAGNI?
    protected:

        // vector<Chart*> charts;

        MultiChartAccordion* multiChartAccordion = nullptr;
    public:
        explicit ChartInjector(
            MultiChartAccordion* multiChartAccordion
        ):
            multiChartAccordion(multiChartAccordion)
        {}

        virtual ~ChartInjector() {
            // vector_destroy(charts);
        }

        Chart* createChart(const string& title, int frameHeight) {
            return multiChartAccordion->createChart(title, frameHeight);
            // const vector<Accordion::Container*>& containers = multiChartAccordion.getContainers();
            // for (const Accordion::Container* container: containers) {
            //     const string& containerTitle = container->getToggler()->getText();
            //     if (containerTitle == title) {
            //         DBG("title", chartHeight, timeRange.begin, timeRange.end);
            //         Frame* frame = container->getFrame();
            //         frame->setScrollFixedY(false);
            //         Chart* chart = new Chart(
            //             frame->getGFX(), 0, frame->getHeight() - 100, frame->getWidth(), 
            //             chartHeight, timeRange.begin, timeRange.end
            //         );
                    // frame->child(*chart);
                    // multiChartAccordion.getMultiChart().attach(*chart);
            //         charts.push_back(chart);
            //         return chart;
            //     }
            // }
            // return nullptr;
        }
    };

}