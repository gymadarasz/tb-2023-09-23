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

class ChartPlugin {
public:
    virtual void show(Chart& /*chart*/, void* /*context*/) const {
        throw ERR_UNIMP;
    };
};

class CandlesChartPlugin: public ChartPlugin {
public:
    void show(Chart& chart, void* context) const override {
        TradeHistory* history = (TradeHistory*)context;
        vector<Candle> candles = history->getCandles();
        vector<RealPoint> candlesRealPoints;
        for (const Candle& candle: candles) {
            double start = static_cast<double>(candle.getStart());
            double end = static_cast<double>(candle.getEnd());
            double open = candle.getOpen();
            double close = candle.getClose();
            double low = candle.getLow();
            double high = candle.getHigh();
            double middle = start + (end - start) / 2;
            candlesRealPoints.push_back(RealPoint(start, open));
            candlesRealPoints.push_back(RealPoint(end, close));
            candlesRealPoints.push_back(RealPoint(middle, low));
            candlesRealPoints.push_back(RealPoint(middle, high));
        }

        vector<Trade> trades = history->getTrades();
        vector<RealPoint> pricesRealPoints;
        vector<RealPoint> volumesRealPoints;
        for (const Trade& trade: trades) {
            pricesRealPoints.push_back(
                RealPoint(static_cast<double>(trade.timestamp), trade.price));
            volumesRealPoints.push_back(
                RealPoint(static_cast<double>(trade.timestamp), trade.volume));
        }
        

        chart.getScaleAt(0).setShape(CANDLE);
        chart.getScaleAt(0).project(candlesRealPoints);
        chart.addScale(1, 1, LINE, orange);
        chart.getScaleAt(1).project(pricesRealPoints);
        chart.addScale(1, 1, LINE, darkGray);
        chart.getScaleAt(2).project(volumesRealPoints);
    }
};

int main()
{
    GFX gfx;
    chart_manual_test4_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600);
    Frame frame(gfx, 10, 10, 780, 580, BUTTON_PUSHED, black);
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
    const string symbol = "MONTE-CARLO";
    const double volumeMean = 50;  // Initial volume
    const double volumeStdDeviation = 5;
    const double priceMean = 100;  // Initial price
    const double priceStdDeviation = 5;
    const double timeMean = 60000;  // Mean time in milliseconds (60 seconds)
    const double timeStdDeviation = 20000;  // Time standard deviation in milliseconds (20 seconds)
    const ms_t startTime = datetime_to_ms("2023-01-01"); // Current time as the start time
    const ms_t endTime = datetime_to_ms("2023-01-02"); // 300 seconds in the future
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 3;

    // Create a MonteCarloHistory object with the specified parameters
    MonteCarloHistory history(
        symbol, startTime, endTime, period,
        volumeMean, volumeStdDeviation,
        priceMean, priceStdDeviation,
        timeMean, timeStdDeviation, seed
    );

    CandlesChartPlugin candlesPlugin;
    candlesPlugin.show(chart, &history);

    gui.loop();
    
    return 0;
}
