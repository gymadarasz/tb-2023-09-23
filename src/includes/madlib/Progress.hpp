#pragma once

#include <cstdio>
#include <string>
#include "zenity.hpp"

using namespace std;

namespace madlib {

    class Progress {
    protected:

        bool closed = false;

        FILE* pipe = nullptr;

    public:
        
        explicit Progress(
            const string& title = "Loading...",
            bool noCancel = false,
            bool autoClose = true,
            bool timeRemaining = true
        ):  
            pipe(
                zenity_progress(
                    title, 
                    noCancel, 
                    autoClose, 
                    timeRemaining
                )
            ) {}
        
        virtual ~Progress() {
            if (!closed) close();
        }

        bool update(int percent) {
            return closed = zenity_progress_update(pipe, percent);
        }

        bool update(const string& status) {
            return closed = zenity_progress_update(pipe, status);
        }

        bool update(double at, double from, double to) {
            double ratio =  (at - from) / (to - from);
            double percent = ratio * 100;
            return update((int)percent);
        }

        int close() {
            closed = true;
            return zenity_progress_close(pipe);
        }
    };
}