#pragma once

#include <cmath>
#include <X11/X.h>

#include "Color.hpp"
#include "defs.hpp"

namespace madlib::graph {

    struct Theme {
        static const unsigned long defaultGFXEventLoopMs = 300;
        static const Color defaultWindowColor = gray;
        static const char* defaultWindowTitle;
        static const char* defaultWindowFont;

        static const Align defaultAreaTextAlign = CENTER;
        static const Border defaultAreaBorder = NONE;
        static const Color defaultAreaBackgroundColor = gray;
        static const int defaultAreaMargin = 10;
        static const int defaultAreaTextMargin = 10;
        static const Color defaultAreaBorderColor = gray;
        static const Color defaultAreaTextColor = black;

        static const Color defaultInputBackgroundColor = white;

        static const Border defaultFrameBorder = PUSHED;
        static const Color defaultFrameBackgroundColor = gray;

        static const Align defaultButtonTextAlign = CENTER;
        static const Border defaultButtonBorder = RELEASED;

        static const Border defaultAccordionBorder = NONE;
        static const Color defaultAccordionBackgroundColor = darkGray;
        static const Align defaultAccordionTogglerTextAlign = CENTER;
        static const int defaultAccordionContainterTogglerHeight = 30;

        static const Align defaultLabelTextAlign = LEFT;
        static const Border defaultLabelBorder = NONE;

        static const int defaultBarThickness = 20;

        static const int defaultTimeRangeAreMarginTop = 0;
        static const int defaultTimeRangeAreMarginLeft = 150;
        static const int defaultTimeRangeAreMarginRight = 150;
        static const int defaultTimeRangeAreMarginBottom = 20;

        static const Border defaultChartBorder = PUSHED;
        static const Color defaultChartBorderColor = gray;
        static const Color defaultChartBackgroundColor = black;
        static const int defaultChartMargin = 0;
        static const Color defaultChartCandleColorUp = green;
        static const Color defaultChartCandleColorDown = red;
        static const Color defaultChartLabelColor = white;
        static const bool defaultChartLabelHasBackground = true;
        static const Color defaultChartLabelBackgroundColor = black;
        static const Color defaultChartLabelBorderColor =  gray;
        static const int defaultChartLabelPadding = 3;
        static const Color defaultChartDotScaleContext;
        static const Color defaultChartLineScaleContext;
        static const Color defaultChartBoxScaleContext;
        static const Color defaultChartFilledScaleContext;

        static const Color defaultChartSeriesColor = gray;

        static const int touchButton = Button1;
        static const int zoomInScrollButton = Button4;
        static const int zoomOutScrollButton = Button5;
        static constexpr double zoomInRatio = 1.1;
        static constexpr double zoomOutRatio = .8;
        static constexpr double zoomRatioMax = INFINITY;
        static constexpr double zoomRatioMin = 1;
        
        // ==== trading ====
        static const Color defaultTradeHistoryChartPriceColor;
        static const Color defaultTradeHistoryChartVolumeColor;
        static const Color defaultTradeLabelBuyColor;
        static const Color defaultTradeLabelSellColor;
        static const Color defaultTradeLabelErrorColor;
    };
    const char* Theme::defaultWindowTitle = "graph";
    const char* Theme::defaultWindowFont = "7x14";
    const Color Theme::defaultChartDotScaleContext = darkGray;
    const Color Theme::defaultChartLineScaleContext = darkGray;
    const Color Theme::defaultChartBoxScaleContext = darkGray;
    const Color Theme::defaultChartFilledScaleContext = darkGray;

    // ==== trading ====
    const Color Theme::defaultTradeHistoryChartPriceColor = orange;
    const Color Theme::defaultTradeHistoryChartVolumeColor = darkGray;
    const Color Theme::defaultTradeLabelBuyColor = red;
    const Color Theme::defaultTradeLabelSellColor = green;
    const Color Theme::defaultTradeLabelErrorColor = gray;
}