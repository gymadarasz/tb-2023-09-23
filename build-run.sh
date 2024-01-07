#!/bin/bash
unset GTK_PATH
cppcheck . -ibuild \
--enable=all \
--suppress=useStlAlgorithm \
--suppress=cstyleCast \
--suppress=missingInclude \
--suppress=unusedFunction \
--suppress=noCopyConstructor \
--suppress=noOperatorEq \
--enable=style \
--std=c++11 \
--error-exitcode=1 && \
g++ -c build.cpp -o build/build.o && \
g++ -o build/build build/build.o && \
./build/build "$@"
