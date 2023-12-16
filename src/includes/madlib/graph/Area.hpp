#pragma once

#include "GFX.hpp"
#include "Painter.hpp"

namespace madlib::graph {

    class Area: public Painter {
    protected:

        bool drag = false;
        int dragStartedX = 0, dragStartedY = 0, dragScrollStartedX = 0, dragScrollStartedY = 0;

        static void touchHandler(void* context, unsigned int, int x, int y) {
            Area* that = (Area*)(context);
            if (that->scrollFixedX && that->scrollFixedY) return;

            // drag & scroll only if no child in the event focus
            for (Area* area: that->areas)
                if (area->contains(x, y)) return;

            that->drag = true;
            that->dragStartedX = x;
            that->dragStartedY = y;
            that->dragScrollStartedX = that->scrollX;
            that->dragScrollStartedY = that->scrollY;
        }
        
        static void releaseHandler(void* context, unsigned int, int, int) {
            Area* that = (Area*)(context);
            that->drag = false;
        }
        
        static void moveHandler(void* context, int x, int y) {
            Area* that = (Area*)(context);
            if (that->scrollFixedX && that->scrollFixedY) return;

            if (that->drag) {
                that->setScrollXY(
                    that->dragScrollStartedX + (that->dragStartedX - x), 
                    that->dragScrollStartedY + (that->dragStartedY - y)
                );
                that->draw();
            }
        }


        GFX& gfx;

        int left, top;
        string text;
        const Align textAlign;
        Border border;
        Color backgroundColor;
        int areaMargin;
        int textMargin;
        Color borderColor;
        Color textColor;

        vector<Area*> areas;
        Area* parent = nullptr;

        void reduceViewport(Viewport& viewport) const {
            if (!parent) return;
            int parentTop = parent->getTop() + 1;
            int parentLeft = parent->getLeft() + 1;
            int parentRight = parent->getRight(parentLeft) - 2;
            int parentBottom = parent->getBottom(parentTop) - 2;
            if (viewport.intersect(parentLeft, parentTop, parentRight, parentBottom))
                parent->reduceViewport(viewport);
        }

        void prepareSetViewport() const {
            Viewport viewport;
            getViewport(viewport);
            reduceViewport(viewport);
            gfx.setViewport(viewport);
        }

        void prepare(int &x, int &y) const {
            int leftAndScroll = getLeft() - scrollX; // TODO: calculate zoom too
            int topAndScroll = getTop() - scrollY;
            x += leftAndScroll;
            y += topAndScroll;
            prepareSetViewport();
        }

        void prepare(int &x1, int &y1, int &x2, int &y2) const {
            int leftAndScroll = getLeft() - scrollX; // TODO: calculate zoom too
            int topAndScroll = getTop() - scrollY;
            x1 += leftAndScroll;
            y1 += topAndScroll;
            x2 += leftAndScroll;
            y2 += topAndScroll;
            prepareSetViewport();
        }

    public:

        Area(
            GFX& gfx, 
            int left, int top, 
            int width, int height, 
            bool scrollFixed = true,
            bool zoomFixed = true,
            const string &text = "", 
            const Align textAlign = Theme::defaultAreaTextAlign,
            const Border border = Theme::defaultAreaBorder,
            const Color backgroundColor = Theme::defaultAreaBackgroundColor,
            const int areaMargin = Theme::defaultAreaMargin,
            const int textMargin = Theme::defaultAreaTextMargin,
            const Color borderColor = Theme::defaultAreaBorderColor,
            const Color textColor = Theme::defaultAreaTextColor,
            void* eventContext = nullptr
        ): 
            Painter(
                width, height, 
                scrollFixed, zoomFixed,
                eventContext
            ),
            gfx(gfx),
            left(left), 
            top(top),
            text(text), 
            textAlign(textAlign), 
            border(border), 
            backgroundColor(backgroundColor), 
            areaMargin(areaMargin),
            textMargin(textMargin),
            borderColor(borderColor),
            textColor(textColor)
        {
            setScrollXY12MinMax(0, 0, width, height);
        }

        virtual ~Area() {
            // managing this->areas pointers is in a caller scope, no need vector_destroy<Area>(areas) 
        }

        const Viewport& getViewport(Viewport& viewport) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            viewport.set(l, t, r, b);
            return viewport;
        }

        virtual void brush(Color color) const override {
            gfx.setColor(color);
        }

        virtual void point(int x, int y) override {
            setScrollXYMinMax(x, y);
            prepare(x, y);
            gfx.drawPoint(x, y);
        }

        virtual void rect(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawRectangle(x1, y1, x2, y2);
        }

        virtual void fRect(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.fillRectangle(x1, y1, x2, y2);
        }

        virtual void line(int x1, int y1, int x2, int y2) override {
            setScrollXY12MinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawLine(x1, y1, x2, y2);
        }

        virtual void hLine(int x1, int y1, int x2) override {
            int y2 = y1;
            setScrollXY12MinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawHorizontalLine(x1, y1, x2);
        }

        virtual void vLine(int x1, int y1, int y2) override {
            int x2 = x1;
            setScrollXY12MinMax(x1, y1, x2, y2);
            prepare(x1, y1, x2, y2);
            gfx.drawVerticalLine(x1, y1, y2);
        }

        virtual void font(const char* font) const override {
            gfx.setFont(font);
        }

        virtual void write(int x, int y, const string &text) override {
            setScrollXYMinMax(x, y);
            prepare(x, y);
            gfx.writeText(x, y, text);
        }

        virtual TextSize getTextSize(const string& text) const override {
            TextSize textSize;
            gfx.getTextSize(text, textSize.width, textSize.height);
            return textSize;
        }

        GFX& getGFX() const {
            return gfx;
        }

        void setParent(Area* parent) {
            this->parent = parent;
        }

        Area* getParent() const {
            return parent;
        }

        Area* getParentOrSelf() {
            return parent ? parent : this;
        }

        Area* getRoot() {
            return parent ? parent->getRoot() : this;
        }

        Area& child(Area& area) {
            setScrollXYMinMax(
                area.left + area.width + areaMargin,
                area.top + area.height + areaMargin
            );
            area.setParent(this);
            areas.push_back(&area);
            return area;
        }

        int getTop(bool withParent = true) const {
            if (!withParent) return top;
            return top + (parent ? parent->getTop() - parent->scrollY : 0);
        }

        void setTop(int top) {
            this->top = top;
        }

        int getLeft(bool withParent = true) const {
            if (!withParent) return left;
            return left + (parent ? parent->getLeft() - parent->scrollX : 0);
        }

        void setLeft(int left) {
            this->left = left;
        }

        int getRight(bool withParent = true) const {
            return getLeft(withParent) + width;
        }

        int getRight(int left) const {
            return left + width;
        }

        int getRight(int left, int width) const {
            return left + width;
        }

        int getBottom(bool withParent = true) const {
            return getTop(withParent) + height;
        }

        int getBottom(int top) const {
            return top + height;
        }

        int getBottom(int top, int height) const {
            return top + height;
        }

        Border getBorder() const {
            return border;
        }

        void setBorder(Border border) {
            this->border = border;
        }

        string getText() const {
            return text;
        }

        void setText(const string& text) {
            this->text = text;
        }

        Align getTextAlign() const {
            return textAlign;
        }

        Color getBackgroundColor() const {
            return backgroundColor;
        }

        void setBackgroundColor(Color backgroundColor) {
            this->backgroundColor = backgroundColor;
        }

        Color getBorderColor() const {
            return borderColor;
        }

        void setBorderColor(Color borderColor) {
            this->borderColor = borderColor;
        }

        Color getTextColor() const {
            return textColor;
        }

        void setTextColor(Color textColor) {
            this->textColor = textColor;
        }

        bool contains(int x, int y) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            return
                x >= l && x <= r &&
                y >= t && y <= b;
        }

        bool contains(int x1, int y1, int x2, int y2) const {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);
            return !(l >= x2 || x1 >= r || t >= y2 || y1 >= b);
        }

        bool contains(const Area* area) const {
            int l = area->getLeft();
            int t = area->getTop();
            int r = area->getRight(l);
            int b = area->getBottom(t);
            return contains(l, t, r, b);
        }

        void propagateTouch(unsigned int button, int x, int y) {
            if (!contains(x, y)) return;
            for (const onTouchHandler& onTouch: onTouchHandlers)
                onTouch(this, button, x - this->getLeft(true), y - this->getTop(true));
            for (Area* area: areas)
                if (contains(area)) area->propagateTouch(button, x, y);
        }

        void propagateRelease(unsigned int button, int x, int y) {
            for (const onReleaseHandler& onRelease: onReleaseHandlers)
                onRelease(this, button, x - this->getLeft(true), y - this->getTop(true));
            for (Area* area: areas)
                if (contains(area)) area->propagateRelease(button, x, y);
        }

        void propagateMove(int x, int y) {
            for (const onMoveHandler& onMove: onMoveHandlers)
                onMove(this, x - this->getLeft(true), y - this->getTop(true));
            for (Area* area: areas)
                if (contains(area)) area->propagateMove(x, y);
        }

        void drawBorder(int left, int top, int right, int bottom) const {
            Border b = getBorder();
            Color bColor;
            Color bColorLight;
            Color bColorDark;
            switch (b) {

                case NONE:
                    break;

                case SIMPLE:
                    gfx.setColor(getBorderColor());
                    gfx.drawRectangle(left, top, right, bottom);
                    break;

                case RELEASED:
                    // button borders get the background color
                    bColor = getBackgroundColor();
                    bColorLight = ColorMixer::light(bColor);
                    bColorDark = ColorMixer::dark(bColor);
                    gfx.setColor(bColorLight);
                    gfx.drawHorizontalLine(left, top, right);
                    gfx.drawVerticalLine(left, top, bottom);
                    gfx.setColor(bColorDark);
                    gfx.drawHorizontalLine(left, bottom, right);
                    gfx.drawVerticalLine(right, top, bottom);
                    break;

                case PUSHED:
                    // button borders get the background color
                    bColor = getBackgroundColor();
                    bColorLight = ColorMixer::light(bColor);
                    bColorDark = ColorMixer::dark(bColor);
                    gfx.setColor(bColorLight);
                    gfx.drawHorizontalLine(left, bottom, right);
                    gfx.drawVerticalLine(right, top, bottom);
                    gfx.setColor(bColorDark);
                    gfx.drawHorizontalLine(left, top, right);
                    gfx.drawVerticalLine(left, top, bottom);
                    break;

                default:
                    throw ERROR("Invalid border");
                    break;
            }
        }

        void drawBorder() {
            int t = getTop();
            int l = getLeft();
            int r = getRight(l);
            int b = getBottom(t);

            Viewport viewport(l, t, r, b);
            reduceViewport(viewport);
            gfx.setViewport(viewport);

            drawBorder(l, t, r, b);
        }

        virtual void draw() override {
            int t = getTop();
            int l = getLeft();
            int w = width;
            int h = height;
            int r = getRight(l, w);
            int b = getBottom(t, h);

            Viewport viewport(l, t, r, b);
            reduceViewport(viewport);
            gfx.setViewport(viewport);
            
            gfx.setColor(getBackgroundColor());
            gfx.fillRectangle(l, t, r, b);

            drawBorder(l, t, r, b);

            if(!text.empty()) {
                int txtWidth, txtHeight;
                gfx.getTextSize(text, txtWidth, txtHeight);
                int txtLeft;
                Align txtAlign = getTextAlign();
                switch (txtAlign) {

                    case CENTER:
                        txtLeft = l + ((w - txtWidth) / 2);
                        break;

                    case LEFT:
                        txtLeft = l + textMargin;
                        break;

                    case RIGHT:
                        txtLeft = l + w - textMargin - txtWidth;
                        break;
                    
                    default:
                        throw ERROR("Invalid text align");
                        break;
                }
                int txtTop = t + ((h - txtHeight) / 2); // + 16; // ??16
                gfx.setColor(textColor);
                gfx.writeText(txtLeft, txtTop, text);
            }

            for (Area* area: areas)
                if (contains(area)) area->draw();

            for (const onDrawHandler& onDraw: onDrawHandlers) {
                onDraw(this);
            }

        }
    };

}