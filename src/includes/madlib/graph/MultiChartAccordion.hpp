#pragma once

#include "Accordion.hpp"

namespace madlib::graph {
    
    class MultiChartAccordion: public Accordion {
    protected:
        Chart::MultiChart multiChart;
        vector<Chart*> charts;
        ms_t timeRangeBegin;
        ms_t timeRangeEnd;

        void createChartFrame(const string& title, Chart* chart, int frameHeight) {
            Frame* cntrFrame = createContainer(title, frameHeight)->getFrame();
            cntrFrame->setScrollFixed(true);
            int frameWidth = cntrFrame->getWidth();
            chart->setTop(0);
            chart->setLeft(0);
            chart->setWidth(frameWidth - 2);
            chart->setHeight(frameHeight - 1);
            chart->setBorder(PUSHED);
            chart->setBackgroundColor(black);
            cntrFrame->child(chart);
        }

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

        Chart* createChart(const string& title, int frameHeight) { // TODO bubble up params default
            Chart* chart = new Chart(gfx, 0, 0, 0, 0, timeRangeBegin, timeRangeEnd);
            charts.push_back(chart);
            createChartFrame(title, chart, frameHeight);
            multiChart.attach(chart);
            return chart;
        }

        virtual void clearCharts() {
            for (Chart* chart: charts) 
                chart->clearProjectors();
        }
        
    };

}