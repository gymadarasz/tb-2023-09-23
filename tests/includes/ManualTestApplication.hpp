#pragma once

#include "../../src/includes/madlib/graph/graph.hpp"

using namespace madlib::graph;

class ManualTestApplication: public FrameApplication {
public:

    static void closeHandler(void* context, unsigned int, int, int) {
        Button* closeBtn = (Button*)context;
        closeBtn->getRoot()->getGFX().close = true;
    }

protected:

    Button closeBtn = Button(gfx, zoom, 10, 10, 100, 30, "OK");

public:

    virtual void init() override {
        FrameApplication::init();
        
        closeBtn.setBackgroundColor(green);
        closeBtn.setTextColor(white);
        closeBtn.addTouchHandler(closeHandler);
        mainFrame.child(closeBtn);
    }
};
