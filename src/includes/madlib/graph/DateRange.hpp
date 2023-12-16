#pragma once

namespace madlib::graph {

    class DateRange {
    protected:
        const string prompt = "Select a date"; // TODO

        Label* fromLabel = nullptr;
        Input* fromInput = nullptr;
        Label* toLabel = nullptr;
        Input* toInput = nullptr;

        ms_t fromValue;
        ms_t toValue;

        static void onFromDateTouchHandler(void* context, unsigned int, int, int) {
            Area* that = (Area*)context;
            DateRange* dateRange = (DateRange*)that->getEventContext();
            string selection = zenity_date(
                dateRange->fromLabel->getText(),
                dateRange->prompt
            );
            that->getGFX().triggerFakeEvent({ GFX::RELEASE });
            if (selection.empty()) dateRange->fromInput->setText(ms_to_date(dateRange->fromValue));
            else dateRange->fromInput->setText(selection);
            dateRange->fromInput->draw();
        }

        static void onToDateTouchHandler(void* context, unsigned int, int, int) {
            Area* that = (Area*)context;
            DateRange* dateRange = (DateRange*)that->getEventContext();
            string selection = zenity_date(
                dateRange->toLabel->getText(),
                dateRange->prompt
            );
            that->getGFX().triggerFakeEvent({ GFX::RELEASE });
            if (selection.empty()) dateRange->toInput->setText(ms_to_date(dateRange->toValue));
            else dateRange->toInput->setText(selection);
            dateRange->toInput->draw();
        }

    public:
        DateRange(
            Area& parent,
            const int left, 
            const int top,
            const string& title = "Period",
            const ms_t fromValue = now() - day,
            const ms_t toValue = now(),
            const int inputWidth = 90,
            const int labelWidth = 90, // TODO
            const int height = 20 // TODO
        ):
            fromValue(fromValue),
            toValue(toValue)
        {
            GFX& gfx = parent.getGFX();
            int x = left, y = top;
            fromLabel = new Label(gfx, x, y, labelWidth, height, title);
            x += labelWidth;
            fromInput = new Input(gfx, x, y, inputWidth, height, ms_to_date(fromValue));
            x += inputWidth;
            toLabel = new Label(gfx, x, y, 20, height, "-", CENTER);
            x += 20;
            toInput = new Input(gfx, x, y, inputWidth, height, ms_to_date(toValue));
            parent.child(*fromLabel);
            parent.child(*fromInput);
            parent.child(*toLabel);
            parent.child(*toInput);
            fromInput->setEventContext(this);
            toInput->setEventContext(this);
            fromInput->addTouchHandler(onFromDateTouchHandler);
            toInput->addTouchHandler(onToDateTouchHandler);
        }

        virtual ~DateRange() {
            delete fromLabel;
            delete fromInput;
            delete toLabel;
            delete toInput;
        }

        Input* getFromInput() {
            return fromInput;
        }

        Input* getToInput() {
            return toInput;
        }
    };

}