#pragma once

#include "../../ManualTestApplication.hpp"


class AccordionManualTest1: public ManualTestApplication {
protected:

    static void stickyBtnClick(void* context, unsigned int, int, int) {
        Button* stickyBtn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)stickyBtn->getGFX().getContext();
        app->accordion.setSticky(stickyBtn->isPushed());
    }

    static void singleBtnClick(void* context, unsigned int, int, int) {
        Button* stickyBtn = (Button*) context;
        AccordionManualTest1* app = (AccordionManualTest1*)stickyBtn->getGFX().getContext();
        app->accordion.setSingle(stickyBtn->isPushed());
    }

    Accordion accordion = Accordion(gfx, 10, 50, 400, false, LEFT, BUTTON_PUSHED);
    Button stickyBtn = Button(gfx, 420, 50, 100, 40, "Sticky");
    Button singleBtn = Button(gfx, 420, 100, 100, 40, "Single");

public:
    void init() override {
        ManualTestApplication::init();
        
        accordion.addContainer("First", 100);
        accordion.addContainer("Second", 100);
        accordion.addContainer("Third", 100);
        gui.child(accordion);

        stickyBtn.setSticky(true);
        stickyBtn.onTouchHandlers.push_back(stickyBtnClick);
        gui.child(stickyBtn);

        singleBtn.setSticky(true);
        singleBtn.onTouchHandlers.push_back(singleBtnClick);
        gui.child(singleBtn);
    }
};
