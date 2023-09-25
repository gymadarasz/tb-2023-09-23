./cleanup.sh | ./build-run.sh TESTS_COVERAGE && \
lcov -c -d . -o coverage.info && \
genhtml coverage.info -o coverage_report && \
google-chrome ./coverage_report/index.html