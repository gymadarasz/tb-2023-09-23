#pragma once

#include "Application.hpp"
#include "Frame.hpp"

namespace madlib::graph {

    class FrameApplication: public Application {
    protected:
        
        Frame* mainFrame = nullptr;

    public:

        FrameApplication(
            int width = Theme::defaultWindowWidth,
            int height = Theme::defaultWindowHeight,
            const char* title = Theme::defaultWindowTitle
        ): 
            Application(width, height, title) 
        {
            mainFrame = new Frame(
                gfx, 0, 0, 
                gui->getWidth(), gui->getHeight(), 
                false, false, NONE, Theme::defaultWindowColor
            );
        }

        virtual ~FrameApplication() {
            delete mainFrame;
        }

        virtual void init() override {
            gui->child(mainFrame);
        }
    };

}