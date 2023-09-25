#!/bin/bash

g++ -c build.cpp -o build/build.o && \
g++ -o build/build build/build.o && \
./build/build "$@"
