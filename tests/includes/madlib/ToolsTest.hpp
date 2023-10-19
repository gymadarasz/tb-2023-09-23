#pragma once

#include <cassert>

#include "../../../src/includes/madlib/Tools.hpp"

using namespace std;
using namespace madlib;

class ToolsTest {
public:

    static void test_trim() {
        const string testStr1 = "   Hello, World!   ";
        const string testStr2 = "  \t  \t Trim this string \t  \t  ";
        const string testStr3 = "No spaces to trim";

        string result1 = trim(testStr1);
        string result2 = trim(testStr2);
        string result3 = trim(testStr3);

        // Check if the results match the expected trimmed strings
        assert(result1 == "Hello, World!");
        assert(result2 == "Trim this string");
        assert(result3 == "No spaces to trim");
    }

    static void test_normalize_datetime() {
        const string testDateTime1 = "2023-01-15 08:30:45.123";
        const string testDateTime2 = "2022-05-02 12:23";
        const string testDateTime3 = "2021-06-10";

        string result1 = normalize_datetime(testDateTime1);
        string result2 = normalize_datetime(testDateTime2);
        string result3 = normalize_datetime(testDateTime3);

        // Check if the results match the expected normalized strings
        assert(result1 == "2023-01-15 08:30:45.123");
        assert(result2 == "2022-05-02 12:23:00.000");
        assert(result3 == "2021-06-10 00:00:00.000");
    }

    static void test_datetime_conversion() {
        // Test cases
        const string testDateTime0 = "2023-01-15 08:30:45.123";
        const string testDateTime1 = "2023-01-15 08:30:45";
        const string testDateTime2 = "2022-05-02 12:23";
        const string testDateTime3 = "2021-06-10";

        // Convert datetime strings to milliseconds and back to datetime
        ms_t ms0 = datetime_to_ms(testDateTime0);
        string resultDateTime0 = ms_to_datetime(ms0);
        assert(1673771445123 == ms0);
        assert(resultDateTime0 == testDateTime0);

        ms_t ms1 = datetime_to_ms(testDateTime1);
        string resultDateTime1 = ms_to_datetime(ms1);
        assert(1673771445000 == ms1);
        assert(resultDateTime1 == normalize_datetime(testDateTime1));

        ms_t ms2 = datetime_to_ms(testDateTime2);
        string resultDateTime2 = ms_to_datetime(ms2);
        assert(1651494180000 == ms2);
        assert(resultDateTime2 == normalize_datetime(testDateTime2));

        ms_t ms3 = datetime_to_ms(testDateTime3);
        string resultDateTime3 = ms_to_datetime(ms3);
        assert(1623283200000 == ms3);
        assert(resultDateTime3 == normalize_datetime(testDateTime3));
    }

};
