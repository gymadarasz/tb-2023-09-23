#!/bin/bash
unset GTK_PATH
cppcheck . \
--enable=all \
--suppress=useStlAlgorithm \
--suppress=cstyleCast \
--suppress=missingInclude \
--suppress=unusedFunction \
--suppress=noCopyConstructor \
--suppress=noOperatorEq \
--error-exitcode=1 && \
g++ -c build.cpp -o build/build.o && \
g++ -o build/build build/build.o && \
./build/build "$@"
