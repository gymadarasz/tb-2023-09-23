#include <iostream>

#include "includes/VectorTest.hpp"
#include "includes/SystemTest.hpp"
#include "includes/FilesTest.hpp"
#include "includes/LogTest.hpp"

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

    cout << "\033[92mAll tests are passed\033[39m" << endl;

    return 0;
}
