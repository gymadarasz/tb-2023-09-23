#include <iostream>
#include "../includes/madlib/madlib.hpp"
#include "../includes/madlib/interfaces/Printer.h"

using namespace std;

class Test2Printer final: public Printer {
public:
    using Printer::Printer;
    void print(const string& output) override final {
        cout << "Test2Printer prints: " << output;
    }
};

EXPORT_CLASS(Test2Printer)