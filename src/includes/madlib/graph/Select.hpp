#pragma once

#include "../zenity.hpp"
#include "Label.hpp"
#include "Input.hpp"

namespace madlib::graph {

    class Select {
    protected:

        // const string title = "Select";
        const string prompt = "Select"; // TODO
        
        Label* label = nullptr;
        Input* input = nullptr;

        vector<string> values;
        string value;

        static void onSelectTouchHandler(void* context, unsigned int, int, int) {
            Area* that = (Area*)context;
            Select* select = (Select*)that->getEventContext();
            string selection = zenity_combo(
                select->label->getText(),
                select->prompt,
                select->label->getText(),
                select->values
            );
            if (selection.empty()) select->input->setText(select->value);
            else select->input->setText(selection);
            select->input->draw();
        }

    public:
        Select(
            Area& parent,
            int left, int top, 
            const string& title = "Select", 
            const vector<string>& values = {},
            const string& value = "",
            const int inputWidth = 200,
            const int labelWidth = 90, // TODO
            const int height = 20 // TODO
        ):
            values(values), 
            value(value)
        {
            GFX& gfx = parent.getGFX();
            label = new Label(gfx, left, top, labelWidth, height, title);
            input = new Input(gfx, left + labelWidth, top, inputWidth, height, value);
            parent.child(*label);
            parent.child(*input);
            input->setEventContext(this);
            input->addTouchHandler(onSelectTouchHandler);
        }

        virtual ~Select() {
            delete label;
            delete input;
        }

        Input* getInput() {
            return input;
        }

        void setValue(const string& value) {
            this->value = value;
        }

        void setValues(const vector<string>& values) {
            this->values = values;
        }
    };

}