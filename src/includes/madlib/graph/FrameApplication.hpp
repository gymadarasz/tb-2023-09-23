#pragma once

#include "Application.hpp"
#include "Frame.hpp"

namespace madlib::graph {

    class FrameApplication: public Application {
    protected:
        Frame* mainFrame = nullptr;
        // Frame mainFrame = Frame(
        //     gfx, 0, 0, 
        //     gui.getWidth(), gui.getHeight(), 
        //     false, false, NONE, Theme::defaultWindowColor
        // );
    public:

        using Application::Application;

        FrameApplication(): Application() {
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