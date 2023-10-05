#pragma once

#include <thread>
#include <chrono>

using namespace std;

namespace madlib {

    inline void sleep(unsigned long ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

}
