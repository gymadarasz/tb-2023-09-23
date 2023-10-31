#pragma once

#include <string>
#include "Files.hpp"

using namespace std;

namespace madlib {

    class Log {
    protected:
        string filename;

    public:
        Log(const string& f = "app.log") : filename(f) {}

        const Log& date() const {
            write("[" + ms_to_datetime() + "] ");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        const Log& writeln(Args... args) const {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            write(message + "\n");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        const Log& write(Args... args) const {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            Files::file_put_contents(filename, message, true);
            return *this;
        }
    } logger;

    #define LOG(...) logger.date().writeln(__VA_ARGS__)

}