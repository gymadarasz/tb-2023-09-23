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
    
    double randd_norm_dist(double mean, double stddev) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }

    vector<double> randd_norm_dist(double mean, double stddev, size_t count) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<double> dist(mean, stddev);
        vector<double> rands;
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

    vector<double>& randd_norm_dist(double mean, double stddev, size_t count, vector<double>& rands) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<double> dist(mean, stddev);
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

    double min(vector<double> values) {
        if (values.empty()) throw runtime_error("Empty set has no minimum");
        double minVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] < minVal) minVal = values[i];
        return minVal;
    }

    double max(vector<double> values) {
        if (values.empty()) throw runtime_error("Empty set has no maximun");
        double maxVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] > maxVal) maxVal = values[i];
        return maxVal;
    }

}
