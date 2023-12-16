#pragma once

namespace madlib::graph {
    
    class CandleShape: public Shape {
    protected:

        double _open;
        double _close;

    public:

        CandleShape(
            ms_t begin,
            ms_t end,
            double open,
            double low,
            double high,
            double close
        ): 
            Shape(
                TimeRange(begin, end), 
                MinMax<double>(low, high)
            ),
            _open(open),
            _close(close)
        {}

        ms_t begin() const {
            return timeRange.begin;
        }

        ms_t end() const {
            return timeRange.end;
        }

        double open() const {
            return _open;
        }

        double low() const {
            return valueMinMax.min;
        }

        double high() const {
            return valueMinMax.max;
        }

        double close() const {
            return _close;
        }

    };

}