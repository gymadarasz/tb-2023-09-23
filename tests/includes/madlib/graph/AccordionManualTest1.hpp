#pragma once

#include "../../ManualTestApplication.hpp"


class AccordionManualTest1: public ManualTestApplication {
protected:

    static void singleBtnClick(void* context, unsigned int, int, int) {
        Button* btn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
        app->accordion.setSingle(btn->isPushed(), true);
    }

    Accordion accordion = Accordion(gfx, 10, 50, 400, false, SIMPLE);
    Button singleBtn = Button(gfx, 420, 50, 100, 40, "Single");

public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~AccordionManualTest1() {}

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("AccordionManualTest1");
        
        accordion.createContainer("First", 100);
        accordion.createContainer("Second", 100);
        accordion.createContainer("Third", 100);
        mainFrame.child(accordion);

        singleBtn.setSticky(true);
        singleBtn.addTouchHandler(singleBtnClick);
        mainFrame.child(singleBtn);

    }
};
