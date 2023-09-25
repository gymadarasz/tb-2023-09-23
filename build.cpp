#include <iostream>
#include <string>
#include "src/includes/Files.hpp"
#include "src/includes/System.hpp"
#include "src/includes/Vector.hpp"

using namespace std;

class Arguments {
public:

    enum Mode {
        RELEASE = 1,
        DEBUG
    };

    const string sourcePath = "./src/includes";
    const string buildPath = "build/";
    const string mainPath = "./src";
    const string main = "main";
    string flags = "-Wall -Werror -pedantic -Wextra -Wunused -Wuninitialized -Wshadow -Wformat -Wconversion -Wcast-align -Wnull-dereference -Wlogical-op -Wfloat-conversion -Wdouble-promotion -Wsign-conversion -Wsign-promo -Wcast-qual -Wdisabled-optimization -Werror=return-type -Werror=main -Wsuggest-final-methods -std=c++17"; // for debug, add: "-g";
    const string flagsLibs = "-lX11";
    const string cppExtension = ".cpp";
    bool executeMain = true;

    Arguments(const Mode mode = Mode::RELEASE) {   
        cout << "Selected mode: ";
        switch (mode)
        {
            case Mode::DEBUG:
                cout << "DEBUG";
                flags += " -g";
                executeMain = false;
                break;
            
            default:
                cout << "RELEASE";
                break;
        }
        cout << endl;
    }
};

void exec(string cmd) {
    cout << "\033[36mExecute command:\033[39m $ " << cmd << endl;
    string output = System::exec(cmd);
    if (!output.empty()) {
        cout << output << endl;
    }
}

int main(int argc, char *argv[]) {

    cout << "Build in progress.." << endl;

    Arguments::Mode mode = Arguments::Mode::RELEASE;  // Default mode

    if (argc > 1) {
        string modeArg = argv[1];

        if (modeArg == "RELEASE") {
            mode = Arguments::Mode::RELEASE;
        } else if (modeArg == "DEBUG") {
            mode = Arguments::Mode::DEBUG;
        } else {
            cerr << "Invalid mode argument. Usage: " << argv[0] << " [RELEASE|DEBUG]" << endl;
            return 1;  // Exit with an error code
        }
    }

    Arguments args(mode);

    const string sourcePath = args.sourcePath;
    const string buildPath = args.buildPath;
    const string mainPath = args.mainPath;
    const string main = args.main;
    const string flags = args.flags;
    const string flagsLibs = args.flagsLibs;
    const string cppExtension = args.cppExtension;
    const bool executeMain = args.executeMain;

    vector<string> files = Files::findByExtensions(sourcePath, { cppExtension });
    files.push_back(mainPath + "/" + main + cppExtension);

    vector<string> oFiles;
    for (const string& file : files) {
        string path = Files::normalizePath(buildPath + Files::extractPath(file));
        // cout << "PATH:" << path << endl;
        if (!Files::exists(path)) {
            // cout << "CREATE:" << path << endl;
            Files::createPath(path);
        }
        string oFile = buildPath + Files::normalizePath(Files::replaceExtension(file, "o"));
        if (
            !Files::exists(oFile) ||
            Files::getLastModificationTime(oFile) < Files::getLastModificationTime(file)
        ) {
            exec("g++ " + flags + " -c " + file + " -o " + oFile);
        }
        oFiles.push_back(oFile);
    }

    exec("g++ " + flags + " -o " + buildPath + main + " " + Vector::concat(oFiles) + " " + flagsLibs);
    if (executeMain) exec("./" + buildPath + main);

    return 0;
}