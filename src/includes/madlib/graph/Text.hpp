#pragma once

#include <string>

#include "Label.hpp"
#include "Input.hpp"

using namespace std;

namespace madlib::graph {

    class Text {
    protected:

        // const string title = "Select";
        const string title = "Input"; // TODO
        
        Label* label = nullptr;
        Input* input = nullptr;

        static void onTextInputTouchHandler(void* context, unsigned int, int, int) {
            Input* that = (Input*)context;
            Text* text = (Text*)that->getEventContext("Text");
            string input = zenity_entry(
                text->title,
                text->label->getText()
            );
            that->getGFX()->triggerFakeEvent({ GFX::RELEASE });
            if (input.empty()) return;
            that->setText(input);
            that->draw();
        }

    public:
        Text(
            Area* parent,
            int left, int top, 
            const string& title = "Input", 
            const string& value = "",
            const int inputWidth = 200,
            const int labelWidth = 90, // TODO
            const int height = 20 // TODO
        ) {
            GFX* gfx = parent->getGFX();
            label = new Label(gfx, left, top, labelWidth, height, title);
            input = new Input(gfx, left + labelWidth, top, inputWidth, height, value);
            parent->child(label);
            parent->child(input);
            input->setEventContext("Text", this);
            input->addTouchHandler(onTextInputTouchHandler);
        }

        virtual ~Text() {
            delete label;
            delete input;
        }

        Input* getInput() {
            return input;
        }
    };

}
