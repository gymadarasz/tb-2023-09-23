#pragma once

#include "Button.hpp"
#include "Frame.hpp"

namespace madlib::graph {

    class Accordion: public Area {
    public:

        class Container;

        class Toggler: public Button {
        protected:
            
            static void toggleHandler(void* context, unsigned int, int, int) {
                Toggler* that = (Toggler*)context;
                Accordion* accordion = that->container->getAccordion(); 
                
                if (accordion->isSingle()) {
                    accordion->closeAllExcept(that->containerIndex, true);
                    return;
                }

                // if (that->sticky) {
                //     accordion.toggleAt(that->containerIndex);
                //     return;
                // }
                
                Container* container = accordion->getContainers().at(that->containerIndex);
                if (container->isOpened()) {
                    // if (accordion.isOne()) 
                        accordion->closeAt(that->containerIndex, true);
                } else {
                    accordion->openAt(that->containerIndex, true);
                }
            }

            Container* container = nullptr;
            size_t containerIndex;
        public:
            Toggler(
                Container* container, GFX* gfx,
                size_t containerIndex, 
                int left, int top, int width, int height, 
                const string &text, 
                const Align textAlign = Theme::defaultAccordionTogglerTextAlign
            ): 
                Button(gfx, left, top, width, height, text, textAlign),
                container(container),
                containerIndex(containerIndex)
            {
                // sticky = container.getAccordion().isSticky();
                addTouchHandler(toggleHandler); // TODO: unify handler names
            }

            virtual ~Toggler() {}

            Container* getContainer() const {
                return container;
            }

            size_t getContainerIndex() const {
                return containerIndex;
            }
        };

        class Container {
        protected:
            const int innerBorderSize = 1;

            Toggler* toggler = nullptr;
            Frame* frame = nullptr;

            Accordion* accordion = nullptr;
            int frameHeight;
            int togglerHeight;
            bool opened = false;
        public:
            Container(
                Accordion* accordion,
                const string& title, const Align textAlign, 
                int frameHeight,
                int togglerHeight = Theme::defaultAccordionContainterTogglerHeight
            ): 
                accordion(accordion), 
                frameHeight(frameHeight), 
                togglerHeight(togglerHeight)
            {
                GFX* gfx = accordion->getGFX();
                const int width = accordion->width;
                size_t accordionContainerAt = accordion->getContainers().size();
                const int togglerTop = accordion->height + innerBorderSize;

                toggler = new Toggler(this, gfx, accordionContainerAt,
                    innerBorderSize, togglerTop, 
                    width - innerBorderSize*2, 
                    togglerHeight - innerBorderSize*2, 
                    title, textAlign);

                frame = new Frame(gfx, 
                    innerBorderSize, togglerTop + togglerHeight - innerBorderSize*2,
                    width, 0, false, false, NONE, accordion->getBackgroundColor());
            
                accordion->child(toggler);
                accordion->child(frame);
                accordion->height = togglerHeight * ((int)accordionContainerAt + 1);
            }

            virtual ~Container() {
                if (toggler) delete toggler;
                toggler = nullptr;
                if (frame) delete frame;
                frame = nullptr;
            }

            bool isOpened() const {
                return opened;
            }

            void setOpened(bool opened) {
                if (opened) close();
                else open();
            }

            void open() { 
                if (opened) return;
                // toggler->push();
                // if (!toggler->isSticky()) toggler->release();
                opened = true;
            }

            void close() {
                if (!opened) return;
                // toggler->release();
                opened = false;
            }

            Accordion* getAccordion() const {
                return accordion;
            }

            Toggler* getToggler() const {
                return toggler;
            }

            Frame* getFrame() const {
                return frame;
            }

            int getFrameHeight() const {
                return frameHeight;
            }
        };

    protected:
        vector<Container*> containers;
        bool single;

    public:
        Accordion(
            GFX* gfx, int left, int top, int width,
            bool single = false,
            const Border border = Theme::defaultAccordionBorder,
            const Color backgroundColor = Theme::defaultAccordionBackgroundColor,
            void* eventContext = nullptr
        ): 
            Area(
                gfx, left, top, width, 
                0, false, false, "", CENTER,
                border, backgroundColor,
                Theme::defaultAreaMargin,
                Theme::defaultAreaTextMargin,
                Theme::defaultAreaBorderColor,
                Theme::defaultAreaTextColor,
                eventContext
            ),
            single(single)
        {}

        virtual ~Accordion() {
            vector_destroy<Container>(containers);
        }

        bool isSingle() const {
            return single;
        }

        void setSingle(bool single, bool redraw) {
            this->single = single;

            // if single collapse the rest 
            // keep open only the first one 
            // that is already opened
            if (single) {
                bool closing = false;
                size_t containersSize = containers.size();
                for (size_t i = 0; i < containersSize; i++) {
                    if (isOpenedAt(i)) {
                        if (closing) closeAt(i, false);
                        else closing = true;
                    }
                }
            }
            if (redraw) getParentOrSelf()->draw();
        }

        void closeAllExcept(size_t exceptIndex, bool redraw) {
            size_t containersSize = containers.size();
            for (size_t i = 0; i < containersSize; i++) {
                if (i != exceptIndex) closeAt(i, false);
                else openAt(i, false);
            }
            if (redraw) getParentOrSelf()->draw();
        }

        void openAll(bool redraw) {
            const size_t containersSize = containers.size();
            for (size_t i = 0; i < containersSize; i++) {
                openAt(i, false);
            }
            if (redraw) getParentOrSelf()->draw();
        }

        Container* createContainer(const string& title, int frameHeight) {
            // return vector_create(containers, *this, title, textAlign, frameHeight);
            Container* container = new Container(this, title, textAlign, frameHeight);
            containers.push_back(container);
            return container;
        }

        vector<Container*>& getContainers() {
            return containers;
        }

        void openAt(size_t containerIndex, bool redraw) {
            size_t containersSize = containers.size();
            Container* container = containers.at(containerIndex);
            if (container->isOpened()) return;
            container->open();
            const int frameHeight = container->getFrameHeight();
            container->getFrame()->setHeight(frameHeight);
            for (size_t i = containerIndex + 1; i < containersSize; i++) {
                container = containers.at(i);
                Toggler* toggler = container->getToggler();
                toggler->setTop(toggler->getTop(false) + frameHeight);
                Frame* frame = container->getFrame();
                frame->setTop(frame->getTop(false) + frameHeight);
            }
            height += frameHeight;
            if (redraw) getParentOrSelf()->draw();
        }

        void closeAt(size_t containerIndex, bool redraw) {
            size_t containersSize = containers.size();
            Container* container = containers.at(containerIndex);
            if (!container->isOpened()) return;
            container->close();
            const int frameHeight = container->getFrameHeight();
            container->getFrame()->setHeight(0);
            for (size_t i = containerIndex + 1; i < containersSize; i++) {
                container = containers.at(i);
                Toggler* toggler = container->getToggler();
                toggler->setTop(toggler->getTop(false) - frameHeight);
                Frame* frame = container->getFrame();
                frame->setTop(frame->getTop(false) - frameHeight);
            }
            height -= frameHeight;
            if (redraw) getParentOrSelf()->draw();
        }

        // void toggleAt(size_t containerIndex) {
        //     isOpenedAt(containerIndex) 
        //         ? closeAt(containerIndex, true) 
        //         : openAt(containerIndex, true);
        // }

        bool isOpenedAt(size_t containerIndex) {
            return containers.at(containerIndex)->isOpened();
        }

        bool isClosedAt(size_t containerIndex) {
            return !isOpenedAt(containerIndex);
        }
    };

}