#!/bin/bash
tail -f app.log &
cppcheck . \
--enable=all \
--suppress=useStlAlgorithm \
--suppress=cstyleCast \
--suppress=missingIncludeSystem \
--suppress=unusedFunction \
--suppress=noCopyConstructor \
--suppress=noOperatorEq \
--error-exitcode=1 && \
g++ -c build.cpp -o build/build.o && \
g++ -o build/build build/build.o && \
./build/build "$@"
