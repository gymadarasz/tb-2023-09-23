#include <iostream>
#include "../includes/madlib/madlib.hpp"

using namespace std;

namespace madlib::printer {

    class Test1Printer: public Printer {
    public:
        using Printer::Printer;
        virtual void print(const string& output) override {
            cout << "Test1Printer prints: " << output;
        }
    };

    EXPORT_CLASS(Test1Printer)

}
