#include <iostream>

#include "includes/madlib/MadlibTest.hpp"
#include "includes/madlib/ToolsTest.hpp"
#include "includes/madlib/VectorTest.hpp"
#include "includes/madlib/FilesTest.hpp"
#include "includes/madlib/LogTest.hpp"

// Manual tests
#include "includes/madlib/graph/graph_manual_test1.hpp"
#include "includes/madlib/graph/chart_manual_test1.hpp"
#include "includes/madlib/graph/chart_manual_test2.hpp"
#include "includes/madlib/graph/chart_manual_test3_candles.hpp"
#include "includes/madlib/graph/chart_manual_test4_history.hpp"
#include "includes/madlib/graph/chart_manual_test5_zoom.hpp"
#include "includes/madlib/graph/ChartManualTest6.hpp"
#include "includes/madlib/graph/ChartManualTest7Zoom.hpp"
#include "includes/madlib/graph/AccordionManualTest1.hpp"
#include "includes/madlib/graph/MultiChartAccordionManualTest1.hpp"
#include "includes/madlib/trading/MonteCarloChartsTest1.hpp"

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

    cout << "Testing in progress: ";

    Tester::run(MadlibTest::test_str_to_lower);
    Tester::run(MadlibTest::test_parse_bool);
    Tester::run(MadlibTest::test_FactoryWithParams_Int);
    Tester::run(MadlibTest::test_FactoryWithParams_CustomClass);
    Tester::run(MadlibTest::test_FactoryWithoutParams);
    Tester::run(ToolsTest::test_trim);
    Tester::run(ToolsTest::test_normalize_datetime);
    Tester::run(ToolsTest::test_datetime_conversion);
    Tester::run(ToolsTest::testSystem_exec);
    Tester::run(ToolsTest::test_split);
    Tester::run(ToolsTest::test_is_numeric);
    Tester::run(ToolsTest::test_parse);
    Tester::run(ToolsTest::test_str_replace);
    Tester::run(ToolsTest::test_str_replace_map);
    Tester::run(ToolsTest::test_map_has);
    Tester::run(ToolsTest::test_map_keys);
    Tester::run(ToolsTest::test_map_key_exists);
    Tester::run(VectorTest::testVector_concat);
    Tester::run(VectorTest::testVector_save_and_load);
    // Tester::run(VectorTest::testVector_load_and_load_with_reference);
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

    cout << " [OK]" << endl;

    cout << "Manual tests: ";
    
    delete (new ChartManualTest7Zoom)->run();
    delete (new MonteCarloChartsTest1)->run();
    delete (new MultiChartAccordionManualTest1)->run();
    delete (new AccordionManualTest1)->run();
    delete (new ChartManualTest6)->run();

    chart_manual_test5_zoom();
    chart_manual_test4_history();
    chart_manual_test3_candles();
    chart_manual_test2();
    chart_manual_test1();
    graph_manual_test1();

    cout << " [OK]" << endl 
        << "\033[92mAll tests are passed\033[39m" << endl;


    return 0;
}
