#pragma once

#include "../../ManualTestApplication.hpp"


class AccordionManualTest1: public ManualTestApplication {
protected:

    // static void stickyBtnClick(void* /*context*/, unsigned int, int, int) {
    //     // Button* btn = (Button*) context;
    //     // AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
    //     // app->accordion.setSticky(btn->isPushed());
    //     // DBG("STICKY: ", app->accordion.isSticky());
    // }

    static void singleBtnClick(void* context, unsigned int, int, int) {
        Button* btn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
        app->accordion.setSingle(btn->isPushed(), true);
        DBG("SINGLE: ", app->accordion.isSingle());
    }

    // static void oneBtnClick(void* /*context*/, unsigned int, int, int) {
    //     // Button* btn = (Button*) context;
    //     // AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
    //     // app->accordion.setOne(btn->isPushed());
    // }

    Accordion accordion = Accordion(gfx, 10, 50, 400, false, SIMPLE);
    // Button stickyBtn = Button(gfx, 420, 50, 100, 40, "Sticky");
    Button singleBtn = Button(gfx, 420, 50, 100, 40, "Single");
    // Button oneBtn = Button(gfx, 420, 150, 100, 40, "One");

public:
    void init() override {
        ManualTestApplication::init();
        gui.setTitle("AccordionManualTest1");
        
        accordion.createContainer("First", 100);
        accordion.createContainer("Second", 100);
        accordion.createContainer("Third", 100);
        mainFrame.child(accordion);

        // stickyBtn.setSticky(true);
        // stickyBtn.addTouchHandler(stickyBtnClick);
        // mainFrame.child(stickyBtn);

        singleBtn.setSticky(true);
        singleBtn.addTouchHandler(singleBtnClick);
        mainFrame.child(singleBtn);

        // oneBtn.setSticky(true);
        // oneBtn.setPushed(true);
        // oneBtn.addTouchHandler(oneBtnClick);
        // mainFrame.child(oneBtn);
    }
};
