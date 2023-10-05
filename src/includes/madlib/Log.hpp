#pragma once

#include <string>
#include "Files.hpp"

using namespace std;

namespace madlib {

    class Log {
    protected:
        string filename;

    public:
        Log(string f = "app.log") : filename(f) {}

        // Variadic template for writeln method
        template <typename... Args>
        void writeln(Args... args) {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            Files::file_put_contents(filename, message + "\n", true);
        }

    private:
        // TODO: Helper function to concatenate variadic arguments to separate file

        template <typename T>
        string concat(const T &arg) {
            return to_string(arg);
        }

        // Overload to handle const char* arguments
        string concat(const char* arg) {
            return string(arg);
        }

        // Overload to handle char* arguments
        string concat(char* arg) {
            return string(arg);
        }

        template <typename... Args>
        string concat(const char* arg, Args... args) {
            return string(arg) + concat(args...);
        }

        template <typename T, typename... Args>
        string concat(const T &arg, Args... args) {
            return to_string(arg) + concat(args...);
        }
    } logger;

    #define LOG(...) logger.writeln(__VA_ARGS__)

}