#pragma once

#include <chrono>
#include <thread>
#include <iomanip>
#include "macros.hpp"

using namespace std;
using namespace chrono;

namespace madlib {

    #define __DATE_TIME__ COLOR_DATETIME "[" + madlib::ms_to_datetime() + "] " COLOR_DEFAULT

    typedef long ms_t;

    const ms_t second = 1000;
    const ms_t minute = 60 * second;
    const ms_t hour = 60 * minute;
    const ms_t day = 24 * hour;
    const ms_t week = 7 * day;
    
    ms_t now() {
        // Get the current time_point
        system_clock::time_point currentTime = system_clock::now();

        // Convert it to milliseconds since the epoch
        milliseconds ms = duration_cast<milliseconds>(currentTime.time_since_epoch());

        // Extract the value as a long
        return static_cast<ms_t>(ms.count()); 
    }

    inline void sleep_ms(unsigned long ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
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

    ms_t date_to_ms(const string& date) {
        return datetime_to_ms(date);
    }

    string ms_to_datetime(ms_t ms = now(), const char* fmt = "%Y-%m-%d %H:%M:%S", bool millis = true, bool local = false) {
        long sec = ms / 1000;
        long mil = ms % 1000;

        struct tm converted_time;
        if (local) localtime_r(&sec, &converted_time);
        else gmtime_r(&sec, &converted_time);

        ostringstream oss;
        oss << put_time(&converted_time, fmt);

        if (millis) oss << "." << setfill('0') << setw(3) << mil;

        return oss.str();
    }

    string ms_to_date(ms_t ms = now(), const char* fmt = "%Y-%m-%d", bool local = false) {
        return ms_to_datetime(ms, fmt, false, local);
    }
}