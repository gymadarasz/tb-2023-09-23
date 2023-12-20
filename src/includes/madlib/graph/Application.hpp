#pragma once

#include "../Log.hpp"
#include "../ERROR.hpp"
#include "GUI.hpp"
#include "GFX.hpp"

namespace madlib::graph {

    
    class Application {
    protected:

        GFX* gfx = nullptr;
        GUI* gui = nullptr;

    public:

        Application() {
            gfx = new GFX(this);
            gui = new GUI(gfx, 1600, 900, "Application");
        }

        virtual ~Application() {
            delete gfx;
            delete gui;
        }

        Application* run() {
            try {
                init();
                gui->draw();
                gui->loop();
            } catch (exception& e) {
                string errmsg = "Application error: " + string(e.what());
                LOG(errmsg);
                throw ERROR(errmsg);
            }
            return this;
        }

        virtual void init() {
            throw ERR_UNIMP;
        }
    };

}