#pragma once

#include "../../../../libs/clib/clib/str.hpp"
#include "../Log.hpp"
#include "GUI.hpp"
#include "GFX.hpp"

using namespace clib;

namespace madlib::graph {

    
    class Application {
    protected:

        GFX* gfx = nullptr;
        GUI* gui = nullptr;

    public:

        Application(
            int width = Theme::defaultWindowWidth,
            int height = Theme::defaultWindowHeight,
            const char* title = Theme::defaultWindowTitle
        ) {
            gfx = new GFX(this);
            gui = new GUI(gfx, width, height, title);
        }

        virtual ~Application() {
            delete gfx;
            delete gui;
        }

        Application* run(bool closeDisplay = true) {
            try {
                init();
                gui->draw();
                gui->loop(closeDisplay);
            } catch (exception& e) {
                string errmsg = "Application error: " + string(e.what());
                LOGE(errmsg);
                throw ERROR(errmsg);
            }
            return this;
        }

        virtual void init() {
            throw ERR_UNIMP;
        }
    };

}