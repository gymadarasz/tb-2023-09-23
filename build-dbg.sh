#!/bin/bash
cppcheck . \
--enable=all \
--suppress=useStlAlgorithm \
--suppress=cstyleCast \
--suppress=missingIncludeSystem \
--suppress=unusedFunction \
--suppress=noCopyConstructor \
--suppress=noOperatorEq \
--error-exitcode=1 && \
g++ -g -c build.cpp -o build/build.o && \
g++ -g -o build/build build/build.o
# gdb ./build/build
