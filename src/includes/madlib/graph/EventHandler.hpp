#pragma once

#include <vector>

#include "../maps.hpp"

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
        typedef void (*onCloseHandler)(void*);

    protected:

        void* eventContext = nullptr;
        map<const string, void*> eventContexts;
        
        vector<onResizeHandler> onResizeHandlers;
        vector<onKeyPressHandler> onKeyPressHandlers;
        vector<onKeyReleaseHandler> onKeyReleaseHandlers;
        vector<onTouchHandler> onTouchHandlers;
        vector<onReleaseHandler> onReleaseHandlers;
        vector<onMoveHandler> onMoveHandlers;
        vector<onLoopHandler> onLoopHandlers;
        vector<onDrawHandler> onDrawHandlers;
        vector<onCloseHandler> onCloseHandlers;

    public:

        EventHandler(void* eventContext = nullptr): eventContext(eventContext) {}

        virtual ~EventHandler() {}

        void setEventContext(void* eventContext) {
            this->eventContext = eventContext;
        }

        void setEventContext(const string& key, void* eventContext) {
            if (map_has(eventContexts, key)) throw ERROR("Event context key conflicts: " + key);
            eventContexts[key] = eventContext;
            this->eventContext = eventContext;
        }

        void* getEventContext(const string& key) const {
            if (!map_has(eventContexts, key)) throw ERROR("Event context key is not set: " + key);
            return eventContexts.at(key);
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

        vector<onCloseHandler> getCloseHandlers() {
            return onCloseHandlers;
        }

        void addCloseHandler(onCloseHandler handler) {
            onCloseHandlers.push_back(handler);
        }

    };

}