#pragma once

#include "Area.hpp"

namespace madlib::graph {

    class Button: public Area {
    protected:
        
        static void touchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)(context);
            
            if (that->sticky) {
                that->pushed ? that->release() : that->push();                
                return;
            }
            
            if (!that->pushed) that->push();
        }
        
        static void releaseHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)(context);

            if (that->sticky) return;
                
            if (that->pushed) that->release();
        }

        bool pushed = false;
        bool sticky = false;

    public:

        Button(
            GFX& gfx, 
            int left, int top, 
            int width, int height,
            const string &text = "", 
            const Align textAlign = Theme::defaultButtonTextAlign,
            const Border buttonBorder = Theme::defaultButtonBorder,
            void* eventContext = nullptr
        ): Area(
            gfx, left, top, width, height, false, false,
            text, textAlign, buttonBorder,
            Theme::defaultAreaBackgroundColor,
            Theme::defaultAreaMargin,
            Theme::defaultAreaTextMargin,
            Theme::defaultAreaBorderColor,
            Theme::defaultAreaTextColor,
            eventContext
        ) {
            addTouchHandler(touchHandler);
            addReleaseHandler(releaseHandler);
        }

        bool isPushed() const {
            return pushed;
        }

        void setPushed(bool pushed) {
            if (pushed) push();
            else release();
        }

        bool isSticky() const {
            return sticky;
        }

        void setSticky(bool sticky) {
            this->sticky = sticky;
        }

        void push() {
            setBorder(PUSHED);
            drawBorder();
            pushed = true;
        }

        void release() {
            setBorder(RELEASED);
            drawBorder();
            pushed = false;
        }
    };

}