#pragma once

#include <string>
#include "Shared.hpp"

using namespace std;

class Printer: public Shared {
public:
    using Shared::Shared;
    virtual void print(const string& output) = 0;
    void println(const string& output) {
        print(output + "\n");
    }
    virtual ~Printer() {}
};