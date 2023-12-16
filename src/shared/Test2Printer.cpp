#include <iostream>

#include "../includes/madlib/Printer.hpp"

using namespace std;

namespace madlib::printer {

    class Test2Printer: public Printer {
    public:
        using Printer::Printer;
        virtual void print(const string& output) override {
            cout << "Test2Printer prints: " << output;
        }
    };

    extern "C" Test2Printer* createTest2Printer(const string& prefix = nullptr) {
        return new Test2Printer(prefix);
    }
}