#pragma once

#include <cassert>

#include "../../src/includes/Tools.hpp"

class ToolsTest {
public:
    static void testTools_replace() {        
        // Test case 1: Swap two integers
        int a = 5;
        int b = 10;
        Tools::replace(a, b);
        assert(a == 10 && b == 5);

        // Test case 2: Swap two integers with the same value
        int x = 3;
        int y = 3;
        Tools::replace(x, y);
        assert(x == 3 && y == 3);
    }
};