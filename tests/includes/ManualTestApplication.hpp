#pragma once

#include "../../src/includes/madlib/graph/FrameApplication.hpp"
#include "../../src/includes/madlib/graph/Button.hpp"

using namespace madlib::graph;

class ManualTestApplication: public FrameApplication {
public:

    static void closeHandler(void* context, unsigned int, int, int) {
        Button* closeBtn = (Button*)context;
        closeBtn->getRoot()->getGFX()->close();
    }

protected:

    Button* closeBtn = nullptr; // Button(gfx, 10, 10, 100, 30, "OK");

public:

    using FrameApplication::FrameApplication;

    virtual ~ManualTestApplication() {
        delete closeBtn;
    }

    virtual void init() override {
        FrameApplication::init();

        closeBtn = new Button(gfx, 10, 10, 100, 30, "OK");
        
        closeBtn->setBackgroundColor(green);
        closeBtn->setTextColor(white);
        closeBtn->addTouchHandler(closeHandler);
        mainFrame->child(closeBtn);
    }
};
