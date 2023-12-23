#pragma once

#include <string>

using namespace std;

namespace madlib {

    class Printer {
    protected:
        string prefix;
        
    public:
        Printer(const string& prefix = ""): prefix(prefix) {}
        
        virtual void print(const string& output) = 0;
        
        void println(const string& output) {
            print(output + "\n" + prefix);
        }

        virtual ~Printer() {}
        // virtual void init(void* = nullptr) override {}
    };

}