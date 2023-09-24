#include <iostream>
#include <string>
#include <array>
#include <cstdio>

using namespace std;

class System {
public:
    static string exec(const string& command, bool captureOutput = true, bool captureError = true) {
        array<char, 128> buffer;
        string result;

        // Open the command for reading
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw runtime_error("Failed to execute command.");
        }

        // Read the output (if requested)
        if (captureOutput) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                result += buffer.data();
            }
        }

        // Close the pipe
        pclose(pipe);

        return result;
    }
};