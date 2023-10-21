#pragma once

#include <thread>
#include <chrono>
#include <random>
#include <stdexcept>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace chrono;

#define ERR_UNIMP runtime_error("Unimplemented");

namespace madlib {

    typedef long ms_t;

    const ms_t MS_PER_SEC = 1000;
    const ms_t MS_PER_MIN = 60 * MS_PER_SEC;
    const ms_t MS_PER_HOUR = 60 * MS_PER_MIN;
    const ms_t MS_PER_DAY = 24 * MS_PER_HOUR;
    const ms_t MS_PER_WEEK = 7 * MS_PER_DAY;

    string trim(const string& str) {
        // Find the first non-whitespace character from the beginning
        size_t start = 0;
        while (start < str.length() && isspace(str[start])) {
            start++;
        }

        // Find the first non-whitespace character from the end
        size_t end = str.length();
        while (end > start && isspace(str[end - 1])) {
            end--;
        }

        // Extract the trimmed substring
        return str.substr(start, end - start);
    }
        
    string normalize_datetime(const string& datetime) {
        string tpl = "0000-01-01 00:00:00.000";
        const string trimed = trim(datetime);

        const size_t trimedLength = trimed.length();
        for (size_t i = 0; i < trimedLength; i++) {
            tpl[i] = trimed[i];
        }

        return tpl;
    }

    ms_t datetime_to_ms(const string& datetime) {
        if (datetime.empty()) return 0;

        struct tm time_info = {};
        int milliseconds = 0;
        
        size_t size = datetime.size();
        time_info.tm_year = (size > 3 ? stoi(datetime.substr(0, 4)) : 1970) - 1900;
        time_info.tm_mon = (size > 6 ? stoi(datetime.substr(5, 2)) : 1) - 1;
        time_info.tm_mday = size > 9 ? stoi(datetime.substr(8, 2)) : 1;
        time_info.tm_hour = size > 12 ? stoi(datetime.substr(11, 2)) : 0;
        time_info.tm_min = size > 15 ? stoi(datetime.substr(14, 2)) : 0;
        time_info.tm_sec = size > 18 ? stoi(datetime.substr(17, 2)) : 0;
        milliseconds = size > 22 ? stoi(datetime.substr(20, 3)) : 0;

        // Convert the struct tm to milliseconds
        time_t seconds = mktime(&time_info);
        return seconds * 1000 + milliseconds;
    }

    string ms_to_datetime(long ms = 0, const char* fmt = "%Y-%m-%d %H:%M:%S", bool millis = true, bool local = false) {
        long sec = ms / 1000;
        long mil = ms % 1000;

        struct tm converted_time;
        if (local) {
            localtime_r(&sec, &converted_time);
        }
        else {
            gmtime_r(&sec, &converted_time);
        }

        ostringstream oss;
        oss << put_time(&converted_time, fmt);

        if (millis) {
            oss << "." << setfill('0') << setw(3) << mil;
        }

        return oss.str();
    }

    ms_t now() {
        // Get the current time_point
        system_clock::time_point currentTime = system_clock::now();

        // Convert it to milliseconds since the epoch
        milliseconds ms = duration_cast<milliseconds>(currentTime.time_since_epoch());

        // Extract the value as a long
        return static_cast<ms_t>(ms.count()); 
    }

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

    template<typename T>
    T min(vector<T> values) {
        if (values.empty()) throw runtime_error("Empty set has no minimum");
        T minVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] < minVal) minVal = values[i];
        return minVal;
    }

    template<typename T>
    T max(vector<T> values) {
        if (values.empty()) throw runtime_error("Empty set has no maximun");
        T maxVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] > maxVal) maxVal = values[i];
        return maxVal;
    }

}
