#pragma once

namespace madlib::graph {
    
    class LabelShape: public PointShape {
    protected:

        const string _text;
        const Color _color;
        const Color _backgroundColor;
        const int _padding;
        const bool _hasBackground;

    public:

        LabelShape(
            ms_t time,
            double value,
            const string& text,
            const Color color = Theme::defaultChartLabelColor,
            const Color backgroundColor = Theme::defaultChartLabelBackgroundColor,
            const int padding = Theme::defaultChartLabelPadding,
            const bool hasBackground = Theme::defaultChartLabelHasBackground
        ): 
            PointShape(
                time,
                value
            ),
            _text(text),
            _color(color),
            _backgroundColor(backgroundColor),
            _padding(padding),
            _hasBackground(hasBackground)
        {}

        const string text() const {
            return _text;
        }

        Color color() const {
            return _color;
        }

        Color backgroundColor() const {
            return _backgroundColor;
        }

        int padding() const {
            return _padding;
        }

        bool hasBackground() const {
            return _hasBackground;
        }
    };
}