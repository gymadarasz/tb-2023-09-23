#pragma once

#include <random>

using namespace std;

namespace madlib {

    mt19937* rand_gen = nullptr;

    inline void rand_close() {
        delete rand_gen;
        rand_gen = nullptr;
    }

    inline void rand_init() {
        if (rand_gen) rand_close();
        random_device rd; 
        rand_gen = new mt19937(rd());
    }

    inline void rand_init_seed(unsigned int seed) {
        if (rand_gen) rand_close();
        rand_gen = new mt19937(seed);
    }

    inline double randd(double min, double max) {
        if (!rand_gen) rand_init();
        uniform_real_distribution<double> dis(min, max);
        return dis(*rand_gen);
    }

    inline long randl(long min, long max) {
        if (!rand_gen) rand_init();
        uniform_int_distribution<long> dis(min, max);
        return dis(*rand_gen);
    }

    inline int randi(int min, int max) {
        if (!rand_gen) rand_init();
        uniform_int_distribution<int> dis(min, max);
        return dis(*rand_gen);
    }

    template<typename T>
    vector<T>& rand_norm_dist(T mean, T stddev, size_t count, vector<T>& rands) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<T> dist(mean, stddev);
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

    template<typename T>
    vector<T> rand_norm_dist(T mean, T stddev, size_t count) {
        vector<T> rands;
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<T> dist(mean, stddev);
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

}