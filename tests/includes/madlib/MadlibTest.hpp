#pragma once

#include <cassert>
#include "../../../src/includes/madlib/madlib.hpp"

using namespace madlib;

class MadlibTest {
public:
    static void test_FactoryWithParams_Int() {
        Factory<int> intFactory;
        int& intInstance = intFactory.create(42);

        assert(intInstance == 42);

        intFactory.destroy(intInstance);

        // After destroying, attempting to access the instance should fail.
        assert(intInstance != 42);
    }

    static void test_FactoryWithParams_CustomClass() {
        // Additional test for objects of a custom class
        class MyObject {
        public:
            int value;
            MyObject(int val) : value(val) {}
        };

        Factory<MyObject> objFactory;
        MyObject& objInstance = objFactory.create(100);

        assert(objInstance.value == 100);

        objFactory.destroy(objInstance);

        // After destroying, attempting to access the instance should fail.
        assert(objInstance.value != 100);
    }

    static void test_FactoryWithoutParams() {
        // Test for classes without constructor parameters
        class NoParamClass {
        public:
            int value;
            NoParamClass() : value(999) {}  // Default value for testing
        };

        Factory<NoParamClass> noParamFactory;
        NoParamClass& noParamInstance = noParamFactory.create();

        assert(noParamInstance.value == 999);

        noParamFactory.destroy(noParamInstance);

        // After destroying, attempting to access the instance should fail.
        assert(noParamInstance.value != 999);
    }
};
