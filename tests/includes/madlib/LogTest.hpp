#pragma once

#include <cassert>
#include "../../../src/includes/madlib/Log.hpp"

using namespace madlib;

class LogTest {
public:
    static void testLog_writeln() {
        unlink("test.log");
        
        // Create a test Log object
        Log testLog("test.log");

        // Clear the contents of the test log file (if it exists)
        Files::file_put_contents("test.log", "");

        // Test writing a single string
        testLog.writeln("Test message 1");
        string content = Files::file_get_contents("test.log");
        assert(content == "Test message 1\n");

        // Test writing multiple strings
        testLog.writeln("Test message 2", "Test message 3");
        content = Files::file_get_contents("test.log");
        assert(content == "Test message 1\nTest message 2Test message 3\n");

        // Test writing numbers
        testLog.writeln("Number:", 42, "; Pi:", 3.14159);
        content = Files::file_get_contents("test.log");
        assert(content == "Test message 1\nTest message 2Test message 3\nNumber:42; Pi:3.141590\n");

        // Test writing a char* (const char*) argument
        const char* charPtr = "This is a const char* message";
        testLog.writeln("CharPtr:", charPtr);
        content = Files::file_get_contents("test.log");
        assert(content == "Test message 1\nTest message 2Test message 3\nNumber:42; Pi:3.141590\nCharPtr:This is a const char* message\n");

        unlink("test.log");
    }
};