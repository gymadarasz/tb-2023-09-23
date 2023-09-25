#include <vector>
#include <string>
#include <cassert>

#include "../../src/includes/Vector.hpp"

using namespace std;

class VectorTest {
public:

    static void testVector_concat() {

        // Test case 1
        vector<string> words1 = {"Hello", "world"};
        string result1 = Vector::concat(words1, " ");
        assert(result1 == "Hello world");

        // Test case 2
        vector<string> words2 = {"One", "Two", "Three"};
        string result2 = Vector::concat(words2, "-");
        assert(result2 == "One-Two-Three");

        // Test case 3: Empty vector
        vector<string> emptyVector;
        string result3 = Vector::concat(emptyVector, " ");
        assert(result3.empty());
    }

};
