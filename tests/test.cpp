#include <iostream>

#include "includes/ManualTestApplication.hpp"

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
#include "includes/madlib/graph/ChartLabelManualTest1.hpp"
#include "includes/madlib/graph/TestCandleHistoryChartReload.hpp"

#include "includes/madlib/trading/TradingTest.hpp"

using namespace std;

class Tester {
public:

    typedef int (*TestCommandFunction)();
    typedef void (*TestFunction)();

    static void run(ManualTestApplication* test, const string& logmsg) {
        cout << ".";
        LOG("Manual testing...: " + logmsg);
        test->run(false);
        delete test;
    }

    static void run(TestCommandFunction test, const string& logmsg) {
        cout << ".";
        LOG("Testing command...: " + logmsg);
        int result = test();
        if (result) LOG(COLOR_ERROR "Command failed: " COLOR_DEFAULT + result);
        assert(0 == result);
    }

    static void run(TestFunction test, const string& logmsg) {
        cout << ".";
        LOG("Testing...: " + logmsg);
        test();
    }
};

#define TEST(test) Tester::run(test, QUOTEME(test));

void unit_tests() {
    TEST(MadlibTest::test_shared_lib_Factory);
    TEST(MadlibTest::test_str_start_with_positive);
    TEST(MadlibTest::test_str_start_with_negative);
    TEST(MadlibTest::test_args_parse_without_shortcuts);
    TEST(MadlibTest::test_args_parse_with_shortcuts);
    TEST(MadlibTest::test_args_parse_empty_key);
    TEST(MadlibTest::test_args_parse_duplicate_arguments);
    TEST(MadlibTest::test_args_parse_missing_values);
    TEST(MadlibTest::test_args_parse_unrecognized_arguments);
    TEST(MadlibTest::test_args_parse_mix_short_and_long);
    TEST(MadlibTest::test_args_parse_no_arguments);
    TEST(MadlibTest::test_args_parse_complex_values);
    TEST(MadlibTest::test_args_parse_no_value_for_last_argument);
    TEST(MadlibTest::test_args_parse_long_arguments_with_shortcuts);
    TEST(MadlibTest::test_args_parse_short_arguments_without_values);
    TEST(MadlibTest::test_args_parse_short_arguments_with_values_starting_with_dash);
    TEST(MadlibTest::test_lib_reg_match);
    TEST(MadlibTest::test_lib_reg_match_alphabets);
    TEST(MadlibTest::test_str_to_lower);
    TEST(MadlibTest::test_parse_bool);
    TEST(MadlibTest::test_VectorFactoryWithParams_Int);
    TEST(MadlibTest::test_VectorFactoryWithParams_CustomClass);
    TEST(MadlibTest::test_VectorFactoryWithoutParams);
    TEST(ToolsTest::test_trim);
    TEST(ToolsTest::test_ltrim);
    TEST(ToolsTest::test_rtrim);
    TEST(ToolsTest::test_normalize_datetime);
    TEST(ToolsTest::test_datetime_conversion);
    TEST(ToolsTest::testSystem_exec);
    TEST(ToolsTest::test_split);
    TEST(ToolsTest::test_is_numeric);
    TEST(ToolsTest::test_parse);
    TEST(ToolsTest::test_str_replace);
    TEST(ToolsTest::test_str_replace_map);
    TEST(ToolsTest::test_map_has);
    TEST(ToolsTest::test_map_keys);
    TEST(ToolsTest::test_map_key_exists);
    TEST(VectorTest::test_vector_create_destroy);
    TEST(VectorTest::testVector_concat);
    TEST(VectorTest::testVector_save_and_load);
    TEST(VectorTest::testVector_load_and_load_with_reference);
    TEST(FilesTest::testFiles_findByExtension);
    TEST(FilesTest::testFiles_findByExtensions);
    TEST(FilesTest::testFiles_replaceExtension);
    TEST(FilesTest::testFiles_normalizePath);
    TEST(FilesTest::testFiles_extractPath);
    TEST(FilesTest::testFiles_extractFilename);
    TEST(FilesTest::testFiles_exists);
    TEST(FilesTest::testFiles_createPath);
    TEST(FilesTest::testFiles_getLastModificationTime);
    TEST(FilesTest::testFiles_file_get_contents);
    TEST(FilesTest::testFiles_file_put_contents);
    TEST(LogTest::testLog_writeln);
}

void unit_tests_trading() {
    TEST(TradingTest::testBalance_DefaultInitialization);
    TEST(TradingTest::testBalance_CustomInitialization);
    TEST(TradingTest::testBalance_SetAndGetAmount);
    TEST(TradingTest::testBalance_IncrementPositiveAmount);
    TEST(TradingTest::testBalance_IncrementNegativeAmount);
    TEST(TradingTest::testBalance_DecrementPositiveAmount);
    TEST(TradingTest::testBalance_DecrementNegativeAmount);
    TEST(TradingTest::testBalance_DecrementToNegative);
    TEST(TradingTest::testBalance_CheckIfNegativeThroughIncrement);
    TEST(TradingTest::testBalance_IncrementDecrementZeroAmount);
    TEST(TradingTest::testBalance_IncrementDecrementWithErrorHandling);
    TEST(TradingTest::testBalance_CheckIfNegativeViaIncrement);
    TEST(TradingTest::testBalance_CheckIfNegativeViaDecrement);
    TEST(TradingTest::testCandle_DefaultInitialization);
    TEST(TradingTest::testCandle_CustomInitialization);
    TEST(TradingTest::testCandle_SettersAndGetters);
    TEST(TradingTest::testCandle_InvalidInitialization);
    TEST(TradingTest::testCandle_ExtremeValues);
    TEST(TradingTest::testCandle_ConsistencyInGetterSetter);
    TEST(TradingTest::testCandle_VolumeAndPriceRelationship);
    TEST(TradingTest::testHistory_DefaultInitialization);
    TEST(TradingTest::testHistory_SetAndGetSymbol);
    TEST(TradingTest::testHistory_SetAndGetStartTime);
    TEST(TradingTest::testHistory_SetAndGetEndTime);
    TEST(TradingTest::testHistory_SetAndGetPeriod);
    TEST(TradingTest::testHistory_Load);
    TEST(TradingTest::testHistory_Reload);
}

void manual_tests() {
    TEST(new TestCandleHistoryChartReload);
    TEST(new ChartLabelManualTest1);
    TEST(new ChartManualTest7Zoom);
    TEST(new MonteCarloChartsTest1);
    TEST(new MultiChartAccordionManualTest1);
    TEST(new AccordionManualTest1);
    TEST(new ChartManualTest6);
}

void cmd_tests() {
    TEST(chart_manual_test5_zoom);
    TEST(chart_manual_test4_history);
    TEST(chart_manual_test3_candles);
    TEST(chart_manual_test2);
    TEST(chart_manual_test1);
    TEST(graph_manual_test1); // TODO: BUG: drag-scroll doesnt work if I click below the big willi button somewhere
}

int main() {

    cout << "Testing in progress: ";

    cout << "Unit tests: ";
    unit_tests();
    unit_tests_trading();
    cout << " [OK]" << endl;

    cout << "Manual tests: ";
    manual_tests();
    cout << " [OK]" << endl;

    cout << "Tests commands: ";
    cmd_tests();
    cout << " [OK]" << endl;

    string msg = "\033[92mAll tests are passed\033[39m";
    cout << msg << endl;
    LOG(msg);


    return 0;
}
