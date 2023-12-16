#pragma once

#include <vector>

namespace madlib::graph {

    class EventHandler {
    public:

        typedef void (*onResizeHandler)(void*, int, int);
        typedef void (*onKeyPressHandler)(void*, unsigned long);
        typedef void (*onKeyReleaseHandler)(void*, unsigned long);
        typedef void (*onTouchHandler)(void*, unsigned int, int, int);
        typedef void (*onReleaseHandler)(void*, unsigned int, int, int);
        typedef void (*onMoveHandler)(void*, int, int);
        typedef void (*onLoopHandler)(void*);
        typedef void (*onDrawHandler)(void*);

    protected:

        void* eventContext = nullptr;
        
        vector<onResizeHandler> onResizeHandlers;
        vector<onKeyPressHandler> onKeyPressHandlers;
        vector<onKeyReleaseHandler> onKeyReleaseHandlers;
        vector<onTouchHandler> onTouchHandlers;
        vector<onReleaseHandler> onReleaseHandlers;
        vector<onMoveHandler> onMoveHandlers;
        vector<onLoopHandler> onLoopHandlers;
        vector<onDrawHandler> onDrawHandlers;

    public:

        EventHandler(void* eventContext = nullptr): eventContext(eventContext) {}

        virtual ~EventHandler() {}

        void setEventContext(void* eventContext) {
            this->eventContext = eventContext;
        }

        void* getEventContext() const {
            return eventContext;
        }

        vector<onResizeHandler> getResizeHandlers() {
            return onResizeHandlers;
        }

        void addResizeHandler(onResizeHandler handler) {
            onResizeHandlers.push_back(handler);
        }

        vector<onKeyPressHandler> getKeyPressHandlers() {
            return onKeyPressHandlers;
        }

        void addKeyPressHandler(onKeyPressHandler handler) {
            onKeyPressHandlers.push_back(handler);
        }

        vector<onKeyReleaseHandler> getKeyReleaseHandlers() {
            return onKeyReleaseHandlers;
        }

        void addKeyReleaseHandler(onKeyReleaseHandler handler) {
            onKeyReleaseHandlers.push_back(handler);
        }

        vector<onTouchHandler> getTouchHandlers() {
            return onTouchHandlers;
        }

        void addTouchHandler(onTouchHandler handler) {
            onTouchHandlers.push_back(handler);
        }

        vector<onReleaseHandler> getReleaseHandlers() {
            return onReleaseHandlers;
        }

        void addReleaseHandler(onReleaseHandler handler) {
            onReleaseHandlers.push_back(handler);
        }

        vector<onMoveHandler> getMoveHandlers() {
            return onMoveHandlers;
        }

        void addMoveHandler(onMoveHandler handler) {
            onMoveHandlers.push_back(handler);
        }

        vector<onLoopHandler> getLoopHandlers() {
            return onLoopHandlers;
        }

        void addLoopHandler(onLoopHandler handler) {
            onLoopHandlers.push_back(handler);
        }

        vector<onDrawHandler> getDrawHandlers() {
            return onDrawHandlers;
        }

        void addDrawHandler(onDrawHandler handler) {
            onDrawHandlers.push_back(handler);
        }

    };

}