#pragma once

#include <string>
#include <vector>

using namespace std;

namespace madlib {

    class Vector {
    public:
        static string concat(const vector<string>& strings, const string& separator = " ") {
            string result;
            
            for (size_t i = 0; i < strings.size(); ++i) {
                result += strings[i];
                
                // Add the separator unless it's the last element
                if (i < strings.size() - 1) {
                    result += separator;
                }
            }
            
            return result;
        }
    };
    
}