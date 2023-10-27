#include "includes/madlib/Tools.hpp"
#include "includes/madlib/graph/graph.hpp"
#include "includes/madlib/graph/Chart.hpp"
#include "includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

class CandleStrategy {
public:
    virtual void onCandleClose(const Candle&) {
        throw ERR_UNIMP;
    }
};

class ACandleStrategy: public CandleStrategy {
public:
    void onCandleClose(const Candle& candle) override {
        cout << "[" << ms_to_datetime(candle.getStart()) << "] " << candle.getClose() << endl;
    }
};

class Accordion: public Area {
public:

    class Container;

    class Toggler: public Button {
    protected:
        
        static void toggle(void* context, unsigned int, int, int) {
            Toggler* that = (Toggler*)context;
            Accordion& accordion = that->container.getAccordion(); 
            
            if (accordion.isSingle()) accordion.closeAllExcept(that->containerIndex);

            if (that->sticky) {
                accordion.toggleAt(that->containerIndex);
                return;
            }
            
            if (accordion.getContainers().at(that->containerIndex)->isOpened())
                accordion.closeAt(that->containerIndex);
            else 
                accordion.openAt(that->containerIndex);
                
        }

        Container& container;
        size_t containerIndex;
    public:
        Toggler(Container& container, GFX& gfx, size_t containerIndex, 
            int left, int top, int width, int height, 
            const string &text, const Align textAlign = Area::defaultAreaTextAlign
        ): 
            Button(gfx, left, top, width, height, text, textAlign),
            container(container),
            containerIndex(containerIndex)
        {
            sticky = container.getAccordion().isSticky();
            onTouchHandlers.push_back(toggle);
        }

        Container& getContainer() const {
            return container;
        }

        size_t getContainerIndex() const {
            return containerIndex;
        }
    };

    class Container {
    protected:
        const int togglerHeight = 30; // TODO
        const int innerPadding = 1; // TODO

        Toggler* toggler = NULL;
        Frame* frame = NULL;

        Accordion& accordion;
        int frameHeight = 100; // TODO
        bool opened = false;
    public:
        Container(
            Accordion& accordion,
            const string& title, const Align textAlign, 
            int frameHeight = 100 // TODO
        ): accordion(accordion), frameHeight(frameHeight) 
        {
            GFX& gfx = accordion.getGFX();
            const int width = accordion.getWidth();
            const int innerPadding2 = innerPadding * 2;
            const int widthInnerPadding2 = width - innerPadding2;
            vector<Container*>& containers = accordion.getContainers();
            const int togglerTop = accordion.height + innerPadding;

            toggler = new Toggler(*this, gfx, containers.size(),
                innerPadding, togglerTop, widthInnerPadding2, togglerHeight - innerPadding2, 
                title, textAlign);

            frame = new Frame(gfx, 
                innerPadding, togglerTop + togglerHeight - innerPadding2,
                widthInnerPadding2, 0);
        
            accordion.child(*toggler);
            accordion.child(*frame);
            containers.push_back(this);
            accordion.height = togglerHeight * (int)containers.size();
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
            if (!toggler->isSticky()) toggler->push();
            opened = true;
        }

        void close() {
            if (!opened) return;
            toggler->release();
            opened = false;
        }

        Accordion& getAccordion() const {
            return accordion;
        }

        Toggler& getToggler() const {
            return *toggler;
        }

        Frame& getFrame() const {
            return *frame;
        }

        int getFrameHeight() const {
            return frameHeight;
        }
    };

protected:
    vector<Container*> containers;
    bool sticky = false; // TODO
    bool single = false; // TODO
public:
    Accordion(GFX& gfx, int left, int top, int width, bool sticky = false, // TODO
        const Align textAlign = Area::defaultAreaTextAlign,
        const Border border = Area::defaultAreaBorder,
        const Color backgroundColor = Area::defaultAreaBackgroundColor,
        const int frameMargin = Area::defaultFrameMargin,
        const int textPadding = Area::defaultTextMargin
    ): 
        Area(gfx, left, top, width, 0, "", 
            textAlign, border, backgroundColor, frameMargin, textPadding), sticky(sticky) {}

    bool isSticky() const {
        return sticky;
    }

    void setSticky(bool sticky) {
        this->sticky = sticky;
        for (const Container* container: containers) {
            Toggler& toggler = container->getToggler();
            toggler.setSticky(sticky);
            if (sticky && container->isOpened()) toggler.push();
        }
    }

    bool isSingle() const {
        return single;
    }

    void setSingle(bool single) {
        this->single = single;

        // if single collapse the rest 
        // keep open only the first one 
        // that is already opened
        if (single) {
            bool closing = false;
            size_t containersSize = containers.size();
            for (size_t i = 0; i < containersSize; i++) {
                if (isOpenedAt(i)) {
                    if (closing) closeAt(i);
                    else closing = true;
                }
            }
        }
        getParentOrSelf()->draw();
    }

    void closeAllExcept(size_t exceptIndex) {
        size_t containersSize = containers.size();
        for (size_t i = 0; i < containersSize; i++) {
            if (i != exceptIndex) closeAt(i);
        }
    }

    void addContainer(const string& title, int frameHeight) {
        new Container(*this, title, textAlign, frameHeight);
    }

    vector<Container*>& getContainers() {
        return containers;
    }

    void openAt(size_t containerIndex) {
        size_t containersSize = containers.size();
        Container* container = containers.at(containerIndex);
        if (container->isOpened()) return;
        container->open();
        const int frameHeight = container->getFrameHeight();
        container->getFrame().setHeight(frameHeight);
        for (size_t i = containerIndex + 1; i < containersSize; i++) {
            container = containers.at(i);
            Toggler& toggler = container->getToggler();
            toggler.setTop(toggler.getTop(false) + frameHeight);
            Frame& frame = container->getFrame();
            frame.setTop(frame.getTop(false) + frameHeight);
        }
        height += frameHeight;
        getParentOrSelf()->draw();
    }

    void closeAt(size_t containerIndex) {
        size_t containersSize = containers.size();
        Container* container = containers.at(containerIndex);
        if (!container->isOpened()) return;
        container->close();
        const int frameHeight = container->getFrameHeight();
        container->getFrame().setHeight(0);
        for (size_t i = containerIndex + 1; i < containersSize; i++) {
            container = containers.at(i);
            Toggler& toggler = container->getToggler();
            toggler.setTop(toggler.getTop(false) - frameHeight);
            Frame& frame = container->getFrame();
            frame.setTop(frame.getTop(false) - frameHeight);
        }
        height -= frameHeight;
        getParentOrSelf()->draw();
    }

    void toggleAt(size_t containerIndex) {
        isOpenedAt(containerIndex) ? closeAt(containerIndex) : openAt(containerIndex);
    }

    bool isOpenedAt(size_t containerIndex) {
        return containers.at(containerIndex)->isOpened();
    }

    bool isClosedAt(size_t containerIndex) {
        return !isOpenedAt(containerIndex);
    }
};

class MainApplication: public Application {
protected:

    static void stickyBtnClick(void* context, unsigned int, int, int) {
        Button* stickyBtn = (Button*) context;
        MainApplication* app = (MainApplication*)stickyBtn->getGFX().getContext();
        app->accordion.setSticky(stickyBtn->isPushed());
    }

    static void singleBtnClick(void* context, unsigned int, int, int) {
        Button* stickyBtn = (Button*) context;
        MainApplication* app = (MainApplication*)stickyBtn->getGFX().getContext();
        app->accordion.setSingle(stickyBtn->isPushed());
    }

    Accordion accordion = Accordion(gfx, 10, 50, 400, false, LEFT, BUTTON_PUSHED);
    Button stickyBtn = Button(gfx, 420, 50, 100, 40, "Sticky");
    Button singleBtn = Button(gfx, 420, 100, 100, 40, "Single");

public:
    void init() override {
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


int main()
{
    MainApplication app;
    app.run();

    return 0;
}
