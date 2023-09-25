#!/bin/bash

g++ -g -c build.cpp -o build/build.o && \
g++ -g -o build/build build/build.o
# gdb ./build/build
