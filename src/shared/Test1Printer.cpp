#include <iostream>
#include "../includes/madlib/madlib.hpp"

using namespace std;

namespace madlib::printer {

    class Test1Printer final: public Printer {
    public:
        using Printer::Printer;
        void print(const string& output) override final {
            cout << "Test1Printer prints: " << output;
        }
    };

    EXPORT_CLASS(Test1Printer)

}
