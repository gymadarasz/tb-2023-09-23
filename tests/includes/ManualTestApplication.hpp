#pragma once

#include "../../src/includes/madlib/graph/graph.hpp"

using namespace madlib::graph;

class ManualTestApplication: public FrameApplication {
protected:

    Button closeBtn = Button(gfx, 10, 10, 100, 30, "OK");

public:

    virtual void init() override {
        FrameApplication::init();
        
        closeBtn.setBackgroundColor(green);
        closeBtn.setTextColor(white);
        closeBtn.onTouchHandlers.push_back(close);
        mainFrame.child(closeBtn);
    }
};
