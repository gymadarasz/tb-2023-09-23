#include <cassert>

#pragma once

#include "../../../src/includes/madlib/System.hpp"

using namespace std;
using namespace madlib;

class SystemTest {
public:

    static void testSystem_exec() {
        // Test capturing output
        string output = System::exec("echo 'Hello, World!'", true);
        assert(output == "Hello, World!\n");

        // Test capturing error
        string error = System::exec("ls /nonexistentfolder", true);
        assert(error == "ls: cannot access '/nonexistentfolder': No such file or directory\n");  // Error message should not be empty

        // Test not capturing output and error
        string neither = System::exec("echo 'Hello, World!'", false);
        assert(neither.empty());

        // Test running a valid command that should not throw
        try {
            System::exec("ls");
        } catch (const runtime_error& e) {
            // This should not throw an exception
            assert(false);
        }
    }

};
