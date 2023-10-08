#pragma once

#include <thread>
#include <chrono>
#include <random>

using namespace std;

namespace madlib {

inline void sleep(unsigned long ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

inline double randd(double min, double max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

inline long randl(long min, long max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long> dis(min, max);
    return dis(gen);
}

inline int randi(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

}
