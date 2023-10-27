#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

class CandleStrategy {
public:
    virtual void onCandleClose(const Candle&) {
        throw ERR_UNIMP;
    }
};

class ACandleStrategy: public CandleStrategy {
public:
    void onCandleClose(const Candle& candle) override {
        cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
    }
};


class ChartFrame: public Frame {
protected:

    Chart* chart = NULL;

public:
    ChartFrame(
        GFX& gfx, int left, int top, int width, int height,
        const Border border = defaultFrameBorder,
        const Color backgroundColor = defaultFrameBackgroundColor
    ): Frame(gfx, left, top, width, height, border, backgroundColor) {}

    void setChart(Chart& chart) {
        this->chart = &chart;
    }

    static void draw(void* context) {
        ChartFrame* that = (ChartFrame*)context;
        if (that->chart) that->chart->draw();
    }
};

class MultiChart {
protected:

    static const int innerPadding = 1; // TODO

    vector<Chart*> charts;

public:

    Chart& addChartToFrame(ChartFrame& frame) {
        Chart* chart = new Chart(frame);
        frame.setChart(*chart);
        frame.onDrawHandlers.push_back(ChartFrame::draw);
        charts.push_back(chart);
        return *chart;
    }

    Chart& addChartToAccordion(Accordion& accordion, const string& title = "Chart", int frameHeight = 150) {
        Accordion::Container& container = accordion.addContainer(title, frameHeight);
        const int innerPadding2 = innerPadding * 2;
        ChartFrame* frame = new ChartFrame(
            accordion.getGFX(), 
            innerPadding, innerPadding, 
            accordion.getWidth() - innerPadding2, 
            frameHeight - innerPadding2, NONE, black
        );
        container.getFrame().child(*frame);
        return addChartToFrame(*frame);
    }
};

class MultiChartAccordion: public MultiChart, public Accordion {
protected:
public:
    using Accordion::Accordion;

    Chart& addChart(const string& title = "Chart", int frameHeight = 150) {
        return addChartToAccordion(*this, title, frameHeight);
    }
};

class MainApplication: public Application {
protected:

    ChartFrame frame1 = ChartFrame(gfx, 10, 100, 300, 180, BUTTON_PUSHED, black);
    ChartFrame frame2 = ChartFrame(gfx, 10, 300, 300, 180, BUTTON_PUSHED, black);
    ChartFrame frame3 = ChartFrame(gfx, 10, 500, 300, 180, BUTTON_PUSHED, black);

    Accordion accordion = Accordion(gfx, 350, 10, 300);

    MultiChart multiChart;

    MultiChartAccordion multiChartAccordion = MultiChartAccordion(gfx, 700, 10, 300);

    vector<RealPoint> generateRealPoints(size_t n) {
        vector<RealPoint> realPoints;
        for (size_t i = 0; i < n; i++) {
            double x = (double)i, y = randd(0, 100);
            RealPoint realPoint(x, y);
            realPoints.push_back(realPoint);
        }
        return realPoints;
    }

public:
    void init() override {

        multiChart.addChartToFrame(frame1).addScale().project(generateRealPoints(100));
        gui.child(frame1);

        multiChart.addChartToFrame(frame2).addScale().project(generateRealPoints(100));
        gui.child(frame2);

        multiChart.addChartToFrame(frame3).addScale().project(generateRealPoints(100));
        gui.child(frame3);
        
        multiChart.addChartToAccordion(accordion, "First Chart", 240).addScale().project(generateRealPoints(100));
        multiChart.addChartToAccordion(accordion, "Second Chart", 240).addScale().project(generateRealPoints(100));
        multiChart.addChartToAccordion(accordion, "Third Chart", 240).addScale().project(generateRealPoints(100));        
        accordion.openAll();
        gui.child(accordion);

        multiChartAccordion.addChart("Accordion First Chart", 240).addScale().project(generateRealPoints(100));
        multiChartAccordion.addChart("Accordion Second Chart", 240).addScale().project(generateRealPoints(100));
        multiChartAccordion.addChart("Accordion Third Chart", 240).addScale().project(generateRealPoints(100));
        gui.child(multiChartAccordion);
    }
};

int main()
{
    MainApplication app;
    app.run();

    return 0;
}
