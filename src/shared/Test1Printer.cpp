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
    
    extern "C" Test1Printer* createTest1Printer(const string& prefix = nullptr) {
        return new Test1Printer(prefix);
    }

}
