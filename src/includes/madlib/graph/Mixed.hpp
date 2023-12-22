#pragma once

#include <string>

#include "Button.hpp"
#include "Text.hpp"

using namespace std;

namespace madlib::graph {

    class Mixed {
    public:

        enum Type { BOOL, LONG, DOUBLE, STRING };

    protected:

        string name = "";
        string value = "";
        Type type = STRING;

        const int buttonWidth = 150; // TODO
        const int inputWidth = 300;
        const int labelWidth = 190;
        const int height = 20; // TODO

        Button* button = nullptr;
        Text* text = nullptr;

        static void onBoolTouchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;
            Mixed* mixed = (Mixed*)that->getEventContext("Mixed");
            mixed->value = that->isPushed() ? "true" : "false";
            that->draw();
        }

        static void onTouchHandler(void* context, unsigned int, int, int) {
            Input* that = (Input*)context;
            Mixed* mixed = (Mixed*)that->getEventContext("Mixed");
            mixed->value = that->getText();
        }

    public:

        Mixed(
            const string& name,
            bool boolValue
        ): 
            name(name),
            value(boolValue ? "true" : "false"),
            type(BOOL)
        {}

        Mixed(
            const string& name,
            long longValue
        ): 
            name(name),
            value(to_string(longValue)),
            type(LONG)
        {}

        Mixed(
            const string& name,
            double doubleValue
        ): 
            name(name),
            value(to_string(doubleValue)),
            type(DOUBLE)
        {}

        Mixed(
            const string& name,
            const string& value
        ): 
            name(name),
            value(value),
            type(STRING)
        {}

        virtual ~Mixed() {
            if (button) delete button;
            if (text) delete text;
        }

        Mixed& operator=(const Mixed& other) {
            if (this != &other) {
                name = other.name;
                value = other.value;
                type = other.type;
            }
            return *this;
        }

        const string& getName() const {
            return name;
        }

        const string& getValueAsString() const {
            return value;
        }

        const string getTypeName() const {
            switch (type)
            {
                case BOOL: return "boolean";
                case LONG: return "integer";
                case DOUBLE: return "real";
                case STRING: return "string";
            }
            throw ERROR("Invalid type");
        }

        Area* createInput(Area* parent, int left, int top) {
            Input* input = nullptr;
            switch (type)
            {
                case BOOL:
                    if (button) delete button;
                    button = new Button(parent->getGFX(), left + labelWidth, top, buttonWidth, height, name);
                    parent->child(button);
                    button->setSticky(true);
                    button->setPushed(parse_bool(value));
                    button->setEventContext("Mixed", this);
                    button->addTouchHandler(onBoolTouchHandler);
                    return button;
                
                case LONG:
                case DOUBLE:
                case STRING:
                    if (text) delete text;
                    text = new Text(parent, left, top, name, value, inputWidth, labelWidth, height);
                    input = text->getInput();
                    input->setEventContext("Mixed", this);
                    input->addTouchHandler(onTouchHandler);
                    return input;
                
                default:
                    throw ERR_INVALID;
            }
        }
    };

}
