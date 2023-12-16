#pragma once

#include "Area.hpp"
#include "Margin.hpp"

namespace madlib::graph {
    
    class TimeRangeArea: public Area {
    protected:
        Margin margin;

        TimeRange* timeRangeFull = nullptr;
        TimeRange* timeRange = nullptr;

        // convert x to ms_t
        ms_t retractX(int x) const {
            double diff = (double)timeRange->end - (double)timeRange->begin;
            double ms = (x * diff) / width;
            return (ms_t)ms;
        }

    public:
        
        TimeRangeArea(
            GFX &gfx, 
            int left, int top, int width, int height,
            ms_t timeRangeBegin, ms_t timeRangeEnd,
            const Border border = Theme::defaultChartBorder,
            const Color backgroundColor = Theme::defaultChartBackgroundColor,
            const Color borderColor = Theme::defaultChartBorderColor,
            void *eventContext = nullptr
        ): 
            Area(
                gfx, left, top, width, height, 
                true, true, "", 
                Theme::defaultAreaTextAlign, 
                border, backgroundColor, 
                Theme::defaultAreaMargin,
                0, borderColor, 
                Theme::defaultAreaTextColor, 
                eventContext
            ),
            timeRangeFull(new TimeRange(timeRangeBegin, timeRangeEnd)),
            timeRange(new TimeRange(timeRangeBegin, timeRangeEnd))
        {}

        virtual ~TimeRangeArea() {
            delete timeRangeFull;
            delete timeRange;
        }

        TimeRange& getTimeRange() const {
            return *timeRange;
        }

        TimeRange& getTimeRangeFull() const {
            return *timeRangeFull;
        }

        Margin* getMargin() {
            return &margin;
        }

        virtual void point(int x, int y) override { 
            Area::point(x + margin.left, y + margin.top);
        }

        virtual void rect(int x1, int y1, int x2, int y2) override {
            Area::rect(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        virtual void fRect(int x1, int y1, int x2, int y2) override {
            Area::fRect(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        virtual void line(int x1, int y1, int x2, int y2) override {
            Area::line(x1 + margin.left, y1 + margin.top, x2 + margin.left, y2 + margin.top);
        }

        virtual void hLine(int x1, int y1, int x2) override {
            Area::hLine(x1 + margin.left, y1 + margin.top, x2 + margin.left);
        }

        virtual void vLine(int x1, int y1, int y2) override {
            Area::vLine(x1 + margin.left, y1 + margin.top, y2 + margin.top);
        }

        virtual void write(int x, int y, const string& text) override {
            Area::write(x + margin.left, y + margin.top, text);
        }
    };

}