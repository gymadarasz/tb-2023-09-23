#include <iostream>

#include "includes/madlib/VectorTest.hpp"
#include "includes/madlib/SystemTest.hpp"
#include "includes/madlib/FilesTest.hpp"
#include "includes/madlib/LogTest.hpp"

// Manual tests
#include "includes/madlib/graph/graph_manual_test1.hpp"
#include "includes/madlib/graph/chart_manual_test1.hpp"
#include "includes/madlib/graph/chart_manual_test2.hpp"

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
    Tester::run(VectorTest::testVector_save_and_load);
    // Tester::run(VectorTest::testVector_load_and_load_with_reference);
    Tester::run(SystemTest::testSystem_exec);
    Tester::run(FilesTest::testFiles_findByExtension);
    Tester::run(FilesTest::testFiles_findByExtensions);
    Tester::run(FilesTest::testFiles_replaceExtension);
    Tester::run(FilesTest::testFiles_normalizePath);
    Tester::run(FilesTest::testFiles_extractPath);
    Tester::run(FilesTest::testFiles_exists);
    Tester::run(FilesTest::testFiles_createPath);
    Tester::run(FilesTest::testFiles_getLastModificationTime);
    Tester::run(FilesTest::testFiles_file_get_contents);
    Tester::run(FilesTest::testFiles_file_put_contents);
    Tester::run(LogTest::testLog_writeln);

    cout << endl;

    cout << "Manual tests..." << endl;
    graph_manual_test1();
    chart_manual_test1();
    chart_manual_test2();

    cout << "\033[92mAll tests are passed\033[39m" << endl;


    return 0;
}
