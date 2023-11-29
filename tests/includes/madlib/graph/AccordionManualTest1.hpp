#pragma once

#include "../../ManualTestApplication.hpp"


class AccordionManualTest1: public ManualTestApplication {
protected:

    static void stickyBtnClick(void* context, unsigned int, int, int) {
        Button* btn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
        app->accordion.setSticky(btn->isPushed());
    }

    static void singleBtnClick(void* context, unsigned int, int, int) {
        Button* btn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
        app->accordion.setSingle(btn->isPushed());
    }

    static void oneBtnClick(void* context, unsigned int, int, int) {
        Button* btn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)btn->getGFX().getContext();
        app->accordion.setOne(btn->isPushed());
    }

    Accordion accordion = Accordion(gfx, zoom, 10, 50, 400, false, LEFT);
    Button stickyBtn = Button(gfx, zoom, 420, 50, 100, 40, "Sticky");
    Button singleBtn = Button(gfx, zoom, 420, 100, 100, 40, "Single");
    Button oneBtn = Button(gfx, zoom, 420, 150, 100, 40, "One");

public:
    void init() override {
        ManualTestApplication::init();
        gui.setTitle("AccordionManualTest1");
        
        accordion.createContainer(zoom, "First", 100);
        accordion.createContainer(zoom, "Second", 100);
        accordion.createContainer(zoom, "Third", 100);
        mainFrame.child(accordion);

        stickyBtn.setSticky(true);
        stickyBtn.addTouchHandler(stickyBtnClick);
        mainFrame.child(stickyBtn);

        singleBtn.setSticky(true);
        singleBtn.addTouchHandler(singleBtnClick);
        mainFrame.child(singleBtn);

        oneBtn.setSticky(true);
        oneBtn.setPushed(true);
        oneBtn.addTouchHandler(oneBtnClick);
        mainFrame.child(oneBtn);
    }
};
