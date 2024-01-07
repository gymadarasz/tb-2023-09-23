#pragma once

#include <cstdio>
#include <string>
#include "zenity.hpp"

#include "../../../libs/clib/clib/time.hpp"

using namespace std;
using namespace clib;

namespace madlib {

    class Progress {
    protected:

        bool closed = false;

        FILE* pipe = nullptr;

    public:
        
        explicit Progress(
            const string& title = "Loading...",
            bool noCancel = true,
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

        bool update(
            const string& status, 
            ms_t* next = nullptr, ms_t step = MS_PER_SEC
        ) {
            if (next) {
                ms_t n = now();
                if (*next < n) *next = n + step;
                else return closed;
            }
            return closed = zenity_progress_update(pipe, status);
        }

        bool update(
            double at, double from, double to, bool autoclose = true, 
            ms_t* next = nullptr, ms_t step = MS_PER_SEC
        ) {
            if (next) {
                ms_t n = now();
                if (*next < n) *next = n + step;
                else return closed;
            }
            double ratio =  (at - from) / (to - from);
            double percent = ratio * 100;
            if (percent <= 1) percent = 1;
            if (percent >= 100) percent = 100;
            if (!autoclose && percent >= 100) percent = 99;
            return update((int)percent);
        }

        int close() {
            closed = true;
            return zenity_progress_close(pipe);
        }
    };
}