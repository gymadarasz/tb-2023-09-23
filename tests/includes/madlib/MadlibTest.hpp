#pragma once

#include <cassert>
#include "../../../src/includes/madlib/madlib.hpp"

using namespace madlib;

class MadlibTest {
public:
    static void test_str_to_lower() {
        // Test case 1: Empty string
        assert(str_to_lower("") == "");

        // Test case 2: All uppercase characters
        assert(str_to_lower("HELLO") == "hello");

        // Test case 3: All lowercase characters
        assert(str_to_lower("world") == "world");

        // Test case 4: Mixed case characters
        assert(str_to_lower("HeLLo WoRLd") == "hello world");

        // Test case 5: String with non-alphabetic characters
        assert(str_to_lower("123!@#") == "123!@#");
    }

    static void test_parse_bool() {
        // Test case 1: Empty string
        assert(parse_bool("") == false);

        // Test case 2: String containing "0"
        assert(parse_bool("0") == false);

        // Test case 3: String containing "false" (case-insensitive)
        assert(parse_bool("FaLsE") == false);

        // Test case 4: String containing "no" (case-insensitive)
        assert(parse_bool("No") == false);

        // Test case 5: String containing "true"
        assert(parse_bool("true") == true);

        // Test case 6: String containing "yes"
        assert(parse_bool("Yes") == true);

        // Test case 7: String containing "1"
        assert(parse_bool("1") == true);
    }

    static void test_FactoryWithParams_Int() {
        Factory<int> intFactory;
        int* intInstance = intFactory.create(42);

        assert(*intInstance == 42);

        intFactory.destroy(intInstance);

        // After destroying, attempting to access the instance should fail.
        assert(*intInstance != 42);
    }

    static void test_FactoryWithParams_CustomClass() {
        // Additional test for objects of a custom class
        class MyObject {
        public:
            int value;
            explicit MyObject(int val) : value(val) {}
        };

        Factory<MyObject> objFactory;
        MyObject* objInstance = objFactory.create(100);

        assert(objInstance->value == 100);

        objFactory.destroy(objInstance);

        // After destroying, attempting to access the instance should fail.
        assert(objInstance->value != 100);
    }

    static void test_FactoryWithoutParams() {
        // Test for classes without constructor parameters
        class NoParamClass {
        public:
            int value;
            NoParamClass() : value(999) {}  // Default value for testing
        };

        Factory<NoParamClass> noParamFactory;
        NoParamClass* noParamInstance = noParamFactory.create();

        assert(noParamInstance->value == 999);

        noParamFactory.destroy(noParamInstance);

        // After destroying, attempting to access the instance should fail.
        assert(noParamInstance->value != 999);
    }
    
    static void test_str_start_with_positive() {
        assert(str_starts_with("Hello", "Hello, World!"));
        assert(str_starts_with("abc", "abcdef"));
        assert(str_starts_with("", "Anything")); // An empty needle should match any haystack
        assert(str_starts_with("", "")); // Empty needle and haystack should match
        assert(str_starts_with("a", "a")); // Single character needle and haystack match
    }

    static void test_str_start_with_negative() {
        assert(!str_starts_with("World", "Hello, World!"));
        assert(!str_starts_with("abc", "ABCDEF")); // Case-sensitive check
        assert(!str_starts_with("Something", "")); // Needle is longer than haystack
        assert(!str_starts_with("abc", "ab")); // Needle is longer than haystack
    }

    static void test_lib_reg_match() {
        string str = "This is a test string with some numbers 123 and some special characters !@#$%^&*";
        vector<string> matches;
        
        // Test for matching pattern that exists in the string
        assert(reg_match("[0-9]+", str, &matches) == 1);
        assert(matches.size() == 1);
        assert(matches[0] == string("123"));
        
        // Test for matching pattern that does not exist in the string
        assert(reg_match("[a-z]+", str, &matches) == 1);
        assert(matches.size() == 1);
        
        // Test for matching pattern that captures multiple groups
        assert(reg_match("([a-zA-Z]+)\\s([a-z]+)", str, &matches) == 1);
        assert(matches.size() == 3);
        assert(matches[0] == string("This is"));
        assert(matches[1] == string("This"));
        assert(matches[2] == string("is"));
    }

    static void test_lib_reg_match_alphabets() {
        string str = "Thisisateststringwithsomenumbersandallsorts of special characters!@#$%^&*()_+";
        vector<string> matches;

        // Test for matching pattern that does not exist in the string
        assert(reg_match("[0-9]+", str, &matches) == 0);
        assert(matches.size() == 0);

        // Test for matching pattern that exists in the string
        assert(reg_match("test", str, &matches) == 1);
        assert(matches.size() == 1);
        assert(matches[0] == string("test"));

        // Test for matching pattern that appears multiple times in the string
        assert(reg_match("[a-z]+", str, &matches) == 1);
        assert(matches.size() == 1);
    }

    static void test_args_parse_without_shortcuts() {
        const char* argv[] = { "program", "-a", "valueA", "-b", "valueB", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"] == "valueA");
        assert(result["b"] == "valueB");
    }

    static void test_args_parse_with_shortcuts() {
        const char* argv[] = { "program", "-x", "valueX", "--long", "valueLong", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        const map<const char, string> shorts = { { 'x', "short" } };
        map<const string, string> result = args_parse(argc, argv, &shorts);

        assert(result["short"] == "valueX");
        assert(result["long"] == "valueLong");
    }

    static void test_args_parse_empty_key() {
        const char* argv[] = { "program", "-", "value", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        try {
            args_parse(argc, argv);
            assert(false);  // Should not reach here, an exception is expected
        } catch (const exception& e) {
            assert(str_ends_with("Empty argument key", string(e.what())));
        }
    }

    static void test_args_parse_duplicate_arguments() {
        const char* argv[] = { "program", "-a", "valueA", "-a", "valueB", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"] == "valueB");
    }

    static void test_args_parse_missing_values() {
        const char* argv[] = { "program", "-a", "-b", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"].empty());
        assert(result["b"].empty());
    }

    static void test_args_parse_unrecognized_arguments() {
        const char* argv[] = { "program", "-a", "valueA", "--unknown", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"] == "valueA");
        assert(result.find("unknown") != result.end());
        assert(result["unknown"] == ""); // Ensure that "--unknown" has an empty string value
    }


    static void test_args_parse_mix_short_and_long() {
        const char* argv[] = { "program", "-a", "valueA", "--long", "valueLong", "-b", "valueB", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"] == "valueA");
        assert(result["long"] == "valueLong");
        assert(result["b"] == "valueB");
    }

    static void test_args_parse_no_arguments() {
        const char* argv[] = { "program", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result.empty());
    }

    static void test_args_parse_complex_values() {
        const char* argv[] = { "program", "-a", "value with spaces", "--long", "value\"with\"quotes", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"] == "value with spaces");
        assert(result["long"] == "value\"with\"quotes");
    }

    static void test_args_parse_no_value_for_last_argument() {
        const char* argv[] = { "program", "-a", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"].empty());
    }

    static void test_args_parse_long_arguments_with_shortcuts() {
        const char* argv[] = { "program", "--longA", "valueA", "-b", "valueB", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        const map<const char, string> shorts = { { 'b', "shortB" } };
        map<const string, string> result = args_parse(argc, argv, &shorts);

        assert(result["longA"] == "valueA");
        assert(result["shortB"] == "valueB");
    }

    static void test_args_parse_short_arguments_without_values() {
        const char* argv[] = { "program", "-a", "-b", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        map<const string, string> result = args_parse(argc, argv);

        assert(result["a"].empty());
        assert(result["b"].empty());
    }

    static void test_args_parse_short_arguments_with_values_starting_with_dash() {
        const char* argv[] = { "program", "-a", "-valueA", "-b", "--valueB", nullptr };
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        try {
            map<const string, string> result = args_parse(argc, argv);
            // If no exception is thrown, the test has failed
            assert(false);
        } catch (const exception& e) {
            // Exception expected for invalid key "-valueA"
            assert(str_ends_with("Invalid argument key: -valueA", e.what()));
        }
    }

    static void test_shared_lib() {
        // Redirect standard output to a stringstream
        stringstream buffer;
        streambuf* original_cout = cout.rdbuf(buffer.rdbuf());

        SharedFactory sharedFactory = SharedFactory();

        Printer* printer1 = (Printer*)sharedFactory.create("build/src/shared", "Test1Printer");
        printer1->println("Printer1 is printing");
        assert(buffer.str() == "Test1Printer prints: Printer1 is printing\n");

        buffer.str("");  // Clear the buffer for reuse
        
        Printer* printer2 = (Printer*)sharedFactory.create("build/src/shared", "Test2Printer");
        printer2->println("Printer2 is printing");
        assert(buffer.str() == "Test2Printer prints: Printer2 is printing\n");

        // Restore the original cout
        cout.rdbuf(original_cout);
    }
};
