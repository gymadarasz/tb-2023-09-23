#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

// class CandleStrategy {
// public:
//     virtual void onCandleClose(const Candle&) {
//         throw ERR_UNIMP;
//     }
// };

// class ACandleStrategy: public CandleStrategy {
// public:
//     void onCandleClose(const Candle& candle) override {
//         cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
//     }
// };

class ChartFrame {
public:
    static void draw(void* context) {
        Frame* that = (Frame*)context;
        Chart* chart = (Chart*)that->getEventContext();
        if (chart) chart->draw();
    }
};


class MultiChart: public ChartFrame {
protected:

    const int innerBorderSize = 2; // TODO

    vector<Chart*> charts;

public:

    ~MultiChart() {
        Vector::destroy<Chart>(charts);
    }

    Chart& addChartToArea(Area& area) {
        Chart* chart = new Chart(area);
        area.setEventContext(chart);
        area.onDrawHandlers.push_back(draw);
        charts.push_back(chart);
        return *chart;
    }

    Chart& addChartToAccordion(Accordion& accordion, const string& title = "Chart", int frameHeight = 150) {
        const Accordion::Container& container = accordion.addContainer(title, frameHeight);
        Frame& cntrFrame = container.getFrame();
        Frame* frame = new Frame(
            accordion.getGFX(), 
            innerBorderSize, innerBorderSize, 
            cntrFrame.getWidth() - innerBorderSize*2,  // accordion.getWidth() - innerBorderSize*4, 
            frameHeight - innerBorderSize*2,  // frameHeight - innerBorderSize*2,
            BUTTON_PUSHED, black
        );
        cntrFrame.child(*frame);
        return addChartToArea(*frame);
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
        testFrame.onDrawHandlers.push_back(ChartFrame::draw);
        gui.child(testFrame);
    }
};

int main()
{
    MainApplication app;
    app.run();

    return 0;
}
