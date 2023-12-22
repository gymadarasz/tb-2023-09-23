#pragma once

#include <string>

#include "../OrderedMap.hpp"
#include "Button.hpp"
#include "Text.hpp"
#include "FrameApplication.hpp"

using namespace std;

namespace madlib::graph {

    class MixedInputValue {
    public:

        enum Type { UNSET, BOOL, LONG, DOUBLE, STRING };

        static string dump(Type type) {
            if (type == BOOL) return "bool";
            if (type == LONG) return "long";
            if (type == DOUBLE) return "double";
            if (type == STRING) return "string";
            throw ERR_INVALID;
        }

    protected:

        string name = "";
        string value = "";
        Type type = UNSET;

        const int buttonWidth = 150; // TODO
        const int inputWidth = 300;
        const int labelWidth = 190;
        const int height = 20; // TODO

        Button* button = nullptr;
        Text* text = nullptr;

        static void onBoolTouchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;
            MixedInputValue* mixed = (MixedInputValue*)that->getEventContext("MixedInputValue");
            mixed->value = that->isPushed() ? "true" : "false";
            that->draw();
        }

        static void onTouchHandler(void* context, unsigned int, int, int) {
            Input* that = (Input*)context;
            MixedInputValue* mixed = (MixedInputValue*)that->getEventContext("MixedInputValue");
            mixed->value = that->getText();
        }

    public:

        MixedInputValue() {}

        MixedInputValue(
            const string& name,
            bool boolValue
        ): 
            name(name),
            value(boolValue ? "true" : "false"),
            type(BOOL)
        {}

        MixedInputValue(
            const string& name,
            long longValue
        ): 
            name(name),
            value(to_string(longValue)),
            type(LONG)
        {}

        MixedInputValue(
            const string& name,
            double doubleValue
        ): 
            name(name),
            value(to_string(doubleValue)),
            type(DOUBLE)
        {}

        MixedInputValue(
            const string& name,
            const string& value
        ): 
            name(name),
            value(value),
            type(STRING)
        {}

        MixedInputValue(const MixedInputValue& other): 
            name(other.name),
            value(other.value),
            type(other.type)
        {}

        MixedInputValue& operator=(const MixedInputValue& other) {
            if (this != &other) {
                name = other.name;
                value = other.value;
                type = other.type;
            }
            return *this;
        }

        virtual ~MixedInputValue() {
            if (button) delete button;
            if (text) delete text;
        }

        const string& getName() const {
            return name;
        }

        const string& getValueAsString() const {
            return value;
        }

        double getValueAsDouble() const {
            return parse<double>(value);
        }

        long getValueAsLong() const {
            return parse<long>(value);
        }

        bool getValueAsBool() const {
            return parse_bool(value);
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
                    button->setEventContext("MixedInputValue", this);
                    button->addTouchHandler(onBoolTouchHandler);
                    return button;
                
                case LONG:
                case DOUBLE:
                case STRING:
                    if (text) delete text;
                    text = new Text(parent, left, top, name, value, inputWidth, labelWidth, height);
                    input = text->getInput();
                    input->setEventContext("MixedInputValue", this);
                    input->addTouchHandler(onTouchHandler);
                    return input;
                
                default:
                    throw ERR_INVALID;
            }
        }

        string dump(bool show = false) {
            string outp = str_replace("[{name}] ({type}): '{value}'", {
                {"{name}", str_esc(name)},
                {"{type}", dump(type)},
                {"{value}", str_esc(value)},
            });
            if (show) DBG(outp);
            return outp;
        }
    };


    class MixedInputList {
    protected:

        OrderedMap<string, MixedInputValue> mixeds;

        void validate(const string& key, bool failIfAlredyExists) const {
            if (key.empty()) throw ERROR("Key can not be empty");
            if (failIfAlredyExists && mixeds.has(key))
                throw ERROR("Key already exists: " + key);
        }
        
    public:

        MixedInputList() {} //: mixeds({}) {}

        // MixedInputList(const MixedInputList& other): mixeds(other.mixeds) {}

        // MixedInputList& operator=(const MixedInputList& other) {
        //     if (this != &other) {
        //         mixeds = other.mixeds;
        //     }
        //     return *this;
        // }

        virtual ~MixedInputList() {}

        const OrderedMap<string, MixedInputValue>& getMixeds() const {
            return mixeds;
        }

        void add(const string& key, const string& name, bool value) {
            validate(key, true);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void add(const string& key, const string& name, long value) {
            validate(key, true);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void add(const string& key, const string& name, double value) {
            validate(key, true);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void add(const string& key, const string& name, const string& value) {
            validate(key, true);
            mixeds.insert(key, MixedInputValue(name, value));
        }

        void set(const string& key, const string& name, bool value) {
            validate(key, false);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void set(const string& key, const string& name, long value) {
            validate(key, false);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void set(const string& key, const string& name, double value) {
            validate(key, false);
            mixeds.insert(key, MixedInputValue(name, value));
        }
        
        void set(const string& key, const string& name, const string& value) {
            validate(key, false);
            mixeds.insert(key, MixedInputValue(name, value));
        }

        bool getAsBool(const string& key) {
            validate(key, false);
            return mixeds.at(key).getValueAsBool();
        }

        long getAsLong(const string& key) {
            validate(key, false);
            return mixeds.at(key).getValueAsLong();
        }

        double getAsDouble(const string& key) {
            validate(key, false);
            return mixeds.at(key).getValueAsDouble();
        }

        const string& getAsString(const string& key) {
            validate(key, false);
            return mixeds.at(key).getValueAsString();
        }

        int createInputs(Area* area, int left, int top, int lineHeight) {
            int row = 0;
            for (const auto& key: mixeds.getOrder())
                mixeds.at(key).createInput(area, left, top + row++ * lineHeight);
            return row;
        }

        const string dump(bool show = false) {
            vector<string> outs;
            for (const auto& key: mixeds.getOrder()) 
                outs.push_back(mixeds.at(key).dump());
            string outp = str_concat(outs, "\n");
            if (show) DBG(outp);
            return outp;
        }

    };


    class MixedInputListHolder {
    protected:

        MixedInputList list;

    public:

        MixedInputListHolder() {}

        virtual ~MixedInputListHolder() {}

        MixedInputList& getList() {
            return list;
        }

        bool getBool(const string& key) {
            return list.getAsBool(key);
        }

        long getLong(const string& key) {
            return list.getAsLong(key);
        }

        double getDouble(const string& key) {
            return list.getAsDouble(key);
        }

        const string& getString(const string& key) {
            return list.getAsString(key);
        }

        bool update(MixedInputList& inputs) {
            if (!validate(inputs)) return false;
            list = inputs;
            return true;
        }

        virtual bool validate(MixedInputList& inputs) {
            LOG("Warning: A MixedInputList validation is not implemented");
            DBG("Inputs are:\n" + inputs.dump());
            return true;
        }
    };

    class MixedInputListForm: public FrameApplication { // TODO: !@# seems breaks the Montecarlo history settings but otherwise works
    protected:
        const int paddingTop = 10;
        const int paddingLeft = 10;
        const int lineHeight = 30; // TODO

        MixedInputListHolder& holder;
        MixedInputList inputs;

        Button* okButton = nullptr;
        Button* cancelButton = nullptr;

        static void onOkButtonTouchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;
            MixedInputListForm* form = (MixedInputListForm*)that->getEventContext("MixedInputListForm");
            
            // save before close;
            if (form->holder.update(form->inputs)) that->getGFX()->close();
        }

        static void onCancelButtonTouchHandler(void* context, unsigned int, int, int) {
            Button* that = (Button*)context;
            that->getGFX()->close();
        }    

    public:
        explicit MixedInputListForm(
            MixedInputListHolder& holder,
            int width = Theme::defaultSettingsFormWidth,
            int height = Theme::defaultSettingsFormHeight,
            const char* title = Theme::defaultSettingsFormTitle
        ): 
            FrameApplication(width, height, title),
            holder(holder),
            inputs(holder.getList())
        {}

        virtual ~MixedInputListForm() {
            delete okButton;
            delete cancelButton;
        }

        virtual void init() override {
            FrameApplication::init();
            
            int row = inputs.createInputs(mainFrame, paddingLeft, paddingTop, lineHeight);
            
            // int row = 0;
            // for (MixedInputValue& setting: mixeds)
            //     setting.createInput(mainFrame, paddingLeft, paddingTop + row++ * lineHeight);
            
            row++;
            
            okButton = new Button(gfx, 200, row * lineHeight, 90, 20, "Ok");
            okButton->setBackgroundColor(green);
            okButton->setTextColor(white);
            okButton->setEventContext("MixedInputListForm", this);
            okButton->addTouchHandler(onOkButtonTouchHandler);
            mainFrame->child(okButton);

            cancelButton = new Button(gfx, 300, row * lineHeight, 90, 20, "Cancel");
            cancelButton->addTouchHandler(onCancelButtonTouchHandler);
            mainFrame->child(cancelButton);
        }
    };
}
