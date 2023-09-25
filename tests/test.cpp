#include <iostream>

#include "includes/VectorTest.hpp"

using namespace std;



class Tester {
public:
    typedef void (*TestFunction)();

    static void run(TestFunction test) {
        cout << ".";
        test();
    }
};

int main() {

    cout << "Testing in progress.." << endl;

    Tester::run(VectorTest::testVector_concat);

    cout << endl;

    cout << "\033[92mAll tests are passed\033[39m" << endl;

    return 0;
}
