#pragma once

namespace madlib::graph {

    class Input: public Area {
    public:
        Input(
            GFX* gfx, 
            int left, int top, 
            int width, int height,
            const string& text = "",
            // TODO: add style (can we use style structure for each element so pass only one argument per element?)
            void* eventContext = nullptr
        ): 
            Area(
                gfx, left, top, width, height, 
                true, true, text, LEFT, PUSHED, 
                Theme::defaultInputBackgroundColor, 
                0, Theme::defaultAreaTextMargin, 
                Theme::defaultAreaBorderColor, 
                Theme::defaultAreaTextColor, 
                eventContext
            )
        {}

        virtual ~Input() {}
    };

}