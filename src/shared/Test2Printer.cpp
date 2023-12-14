#include <iostream>
#include "../includes/madlib/madlib.hpp"

using namespace std;

namespace madlib::printer {

    class Test2Printer: public Printer {
    public:
        using Printer::Printer;
        virtual void print(const string& output) override {
            cout << "Test2Printer prints: " << output;
        }
    };

    EXPORT_CLASS(Test2Printer)

}