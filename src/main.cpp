#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

GFX* chart_manual_test4_gfxPtr;
Chart* chart_manual_test4_chartPtr;
Button* chart_manual_test4_closeOkBtnPtr;

void chart_manual_test4_draw(void* /*context*/) {
    chart_manual_test4_chartPtr->draw();
    chart_manual_test4_closeOkBtnPtr->draw();
}

void chart_manual_test4_close(void*, unsigned int, int, int) {
    chart_manual_test4_gfxPtr->close = true;
}

int main()
{
    GFX gfx;
    chart_manual_test4_gfxPtr = &gfx;
    GUI gui(gfx, 1600, 600);
    Frame frame(gfx, 10, 10, 1580, 580, BUTTON_PUSHED, black);
    gui.child(frame);
    Chart chart(frame);
    chart_manual_test4_chartPtr = &chart;
    frame.onDrawHandlers.push_back(chart_manual_test4_draw);


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test4_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(chart_manual_test4_close);
    gui.child(closeOkBtn);


    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    const double volumeMean = 100;  // Initial volume
    const double volumeStdDeviation = 10;
    const double priceMean = 100;  // Initial price
    const double priceStdDeviation = 10;
    const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01"); // Current time as the start time
    const ms_t endTime = datetime_to_ms("2021-01-07"); // 300 seconds in the future
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 6;

    // Create a MonteCarloHistory object with the specified parameters
    MonteCarloHistory history(
        symbol, startTime, endTime, period,
        volumeMean, volumeStdDeviation,
        priceMean, priceStdDeviation,
        timeLambda, seed
    );
    history.init();

    TradeHistoryChartPlugin candlesPlugin;
    candlesPlugin.project(chart, &history);

    gui.loop();
    
    return 0;
}
