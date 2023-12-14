#pragma once

#include <cassert>

#include "../../../src/includes/madlib/madlib.hpp"

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

    static void testSystem_exec() {
        // Test capturing output
        string output = exec("echo 'Hello, World!'", true);
        assert(output == "Hello, World!\n");

        // Test capturing error
        string error = exec("ls /nonexistentfolder", true);
        assert(error == "ls: cannot access '/nonexistentfolder': No such file or directory\n");  // Error message should not be empty

        // Test not capturing output and error
        string neither = exec("echo 'Hello, World!'", false);
        assert(neither.empty());
        assert(exec_last_exit_code == 0);

        // Test running a valid command that should not throw
        try {
            exec("ls");
        } catch (const exception& e) {
            // This should not throw an exception
            assert(false);
        }
    }

    static void test_split() {
        // Test case 1: Split by a comma
        string data1 = "apple,banana,cherry";
        string separator1 = ",";
        vector<string> result1 = split(separator1, data1);
        assert(result1.size() == 3);
        assert(result1[0] == "apple");
        assert(result1[1] == "banana");
        assert(result1[2] == "cherry");

        // Test case 2: Split by a space
        string data2 = "one two three";
        string separator2 = " ";
        vector<string> result2 = split(separator2, data2);
        assert(result2.size() == 3);
        assert(result2[0] == "one");
        assert(result2[1] == "two");
        assert(result2[2] == "three");

        // Test case 3: Split by a semicolon
        string data3 = "Alice;Bob;Charlie";
        string separator3 = ";";
        vector<string> result3 = split(separator3, data3);
        assert(result3.size() == 3);
        assert(result3[0] == "Alice");
        assert(result3[1] == "Bob");
        assert(result3[2] == "Charlie");

        // Test case 4: Split by multiple characters
        string data4 = "apple...banana...cherry";
        string separator4 = "...";
        vector<string> result4 = split(separator4, data4);
        assert(result4.size() == 3);
        assert(result4[0] == "apple");
        assert(result4[1] == "banana");
        assert(result4[2] == "cherry");

        // Negative case 1: Empty data
        string data5 = "";
        string separator5 = ",";
        vector<string> result5 = split(separator5, data5);
        assert(result5.empty()); // Should result in an empty vector

        // Negative case 2: Separator not found
        string data6 = "abcdefg";
        string separator6 = ",";
        vector<string> result6 = split(separator6, data6);
        assert(result6.size() == 1); // Should result in a single element vector with the entire string
    }

    static void test_is_numeric() {
        // Test cases for is_numeric function
        assert(is_numeric("12345") == true);
        assert(is_numeric("-123.45") == true);
        assert(is_numeric("abc") == false);
        assert(is_numeric("   123   ") == true);
        assert(is_numeric("123abc") == false);
    }

    static void test_parse() {
        // Test cases for valid values
        assert(parse<double>("3.14159") == 3.14159);
        assert(parse<double>("-123.45") == -123.45);
        assert(parse<double>("0.0") == 0.0);
        assert(parse<double>(".0") == 0.0);
        assert(parse<long>("123") == 123);
        assert(parse<long>("-123") == -123);
        assert(parse<long>("0") == 0);
        
        // Test cases for invalid values
        try {
            parse<double>("abc"); // Invalid input
        } catch (exception &e) {
            assert("Invalid value: abc");
        }

        try {
            parse<double>("");   // Empty string
        } catch (exception &e) {
            assert("Invalid value");
        }

        try {
            parse<long>("abc"); // Invalid input
        } catch (exception &e) {
            assert("Invalid value: abc");
        }

        try {
            parse<long>("");   // Empty string
        } catch (exception &e) {
            assert("Invalid value");
        }
    }
    
    static void test_str_replace() {
        // Test case 1: Replace "This" with "That"
        string input1 = "This is a test. This is only a test.";
        string expected1 = "That is a test. That is only a test.";
        string result1 = str_replace(input1, "This", "That");
        assert(result1 == expected1);

        // Test case 2: Replace "Hello" with "Hi"
        string input2 = "Hello, World!";
        string expected2 = "Hi, World!";
        string result2 = str_replace(input2, "Hello", "Hi");
        assert(result2 == expected2);

        // Test case 3: Key not found (no replacements should occur)
        string input3 = "No replacements here.";
        string expected3 = "No replacements here.";
        string result3 = str_replace(input3, "Hello", "Hi");
        assert(result3 == expected3);

        // Test case 4: Empty input string (result should also be empty)
        string input4 = "";
        string expected4 = "";
        string result4 = str_replace(input4, "This", "That");
        assert(result4 == expected4);
    }

    static void test_str_replace_map() {
        // Test case 1: Replace multiple substrings
        string input1 = "Hello, World! This is a test.";
        map<string, string> replacements1 = {
            {"Hello", "Hi"},
            {"World", "Universe"},
            {"test", "example"}
        };
        string expected1 = "Hi, Universe! This is a example.";
        string result1 = str_replace(input1, replacements1);
        assert(result1 == expected1);

        // Test case 2: Empty input string (result should also be empty)
        string input2 = "";
        map<string, string> replacements2 = {
            {"Hello", "Hi"}
        };
        string expected2 = "";
        string result2 = str_replace(input2, replacements2);
        assert(result2 == expected2);
    }

    static void test_map_has() {
        map<int, string> intMap;
        intMap[1] = "One";
        intMap[2] = "Two";

        // Test map_has with an integer key
        assert(map_has(intMap, 1));
        assert(map_has(intMap, 2));
        assert(!map_has(intMap, 3));

        map<string, int> strMap;
        strMap["apple"] = 5;
        strMap["banana"] = 7;

        // Test map_has with a string key
        assert(map_has(strMap, "apple"));
        assert(map_has(strMap, "banana"));
        assert(!map_has(strMap, "cherry"));

        // Test with an empty integer map
        map<int, string> intMapEmpty;
        assert(!map_has(intMapEmpty, 1));
        assert(!map_has(intMapEmpty, 2));

        // Test with an empty string map
        map<string, int> strMapEmpty;
        assert(!map_has(strMapEmpty, "apple"));
        assert(!map_has(strMapEmpty, "banana"));
    }

    static void test_map_keys() {
        map<int, string> intMap;
        intMap[1] = "One";
        intMap[2] = "Two";

        vector<int> intKeys = map_keys(intMap);
        assert(intKeys.size() == 2);
        assert(intKeys[0] == 1);
        assert(intKeys[1] == 2);

        map<string, int> strMap;
        strMap["apple"] = 5;
        strMap["banana"] = 7;

        vector<string> strKeys = map_keys(strMap);
        assert(strKeys.size() == 2);
        assert(strKeys[0] == "apple");
        assert(strKeys[1] == "banana");

        // Test with an empty map
        map<int, string> emptyMap;
        vector<int> emptyKeys = map_keys(emptyMap);
        assert(emptyKeys.empty());
    }

    static void test_map_key_exists() {
        map<int, string> intMap;
        intMap[1] = "One";
        intMap[2] = "Two";

        assert(map_key_exists(intMap, 1));
        assert(map_key_exists(intMap, 2));
        assert(!map_key_exists(intMap, 3));

        map<string, int> strMap;
        strMap["apple"] = 5;
        strMap["banana"] = 7;

        assert(map_key_exists(strMap, "apple"));
        assert(map_key_exists(strMap, "banana"));
        assert(!map_key_exists(strMap, "cherry"));

        // Test with an empty map
        map<int, string> emptyMap;
        assert(!map_key_exists(emptyMap, 1));
        assert(!map_key_exists(emptyMap, 2));
    }
};
