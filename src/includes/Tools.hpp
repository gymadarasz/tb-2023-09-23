#pragma once

#include <thread>
#include <chrono>

using namespace std;

class Tools {
public:
    static void replace(int &a, int &b) {
        int t = a;
        a = b;
        b = t;
    }

    static void sleep(unsigned long ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
};