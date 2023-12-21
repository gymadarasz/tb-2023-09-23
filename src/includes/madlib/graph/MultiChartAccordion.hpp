#pragma once

#include "Accordion.hpp"

namespace madlib::graph {
    
    class MultiChartAccordion: public Accordion {
    protected:
        Chart::MultiChart multiChart;
        vector<Chart*> charts;
        ms_t timeRangeBegin;
        ms_t timeRangeEnd;

    public:
    
        MultiChartAccordion(
            GFX* gfx, int left, int top, int width,
            ms_t timeRangeBegin,
            ms_t timeRangeEnd,
            bool single = false,
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor,
            void* eventContext = nullptr
        ):
            Accordion(
                gfx, left, top, width, 
                single, border, backgroundColor, 
                eventContext
            ),
            timeRangeBegin(timeRangeBegin),
            timeRangeEnd(timeRangeEnd)
        {}

        virtual ~MultiChartAccordion() {
            vector_destroy(charts);
        }

        Chart::MultiChart& getMultiChart() {
            return multiChart;
        }

        Container* createChartFrame(const string& title, Chart* chart, int frameHeight) {
            vector<size_t> wasOpens = closeAll(false); // TODO: it's a hack to fix sizing (BUG: when strategy adds new charts the screen scroll size is not updated)
            Container* container = createContainer(title, frameHeight);
            Frame* frame = container->getFrame();
            openAll(false);
            closeAllExcept(wasOpens, false);            
            frame->setScrollFixed(true);
            int frameWidth = frame->getWidth();
            chart->setTop(0);
            chart->setLeft(0);
            chart->setWidth(frameWidth - 2);
            chart->setHeight(frameHeight - 1);
            chart->setBorder(PUSHED);
            chart->setBackgroundColor(black);
            frame->child(chart);
            Area* parent = frame->getParent();
            if (parent) parent->adaptScrollSize(frame);
            return container;
        }

        Chart* createChart(const string& title, int frameHeight) { // TODO bubble up params default
            Chart* chart = new Chart(gfx, 0, 0, 0, 0, timeRangeBegin, timeRangeEnd);
            charts.push_back(chart);
            createChartFrame(title, chart, frameHeight);
            multiChart.attach(chart);
            return chart;
        }

        Chart* getLastChart() {
            size_t size = charts.size();
            return size ? charts[size - 1] : nullptr;
        }

        virtual void clearCharts() {
            for (Chart* chart: charts) 
                chart->clearProjectors();
        }
        
    };

}