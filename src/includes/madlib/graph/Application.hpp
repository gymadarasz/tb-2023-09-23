#pragma once

#include "../Log.hpp"
#include "../ERROR.hpp"
#include "GUI.hpp"
#include "GFX.hpp"

namespace madlib::graph {

    
    class Application {
    protected:

        GFX gfx = GFX(this);
        GUI gui = GUI(gfx, 1600, 900, "Application");

    public:
    
        Application() {}

        virtual ~Application() {}

        Application* run() {
            try {
                init();
                gui.draw();
                gui.loop();
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