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
            MyObject(int val) : value(val) {}
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
};
