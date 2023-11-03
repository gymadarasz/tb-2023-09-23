#include <iostream>
#include <string>

#include "src/includes/madlib/madlib.hpp"

using namespace std;
using namespace madlib;

class Arguments {
public:

    enum Mode {
        RELEASE = 1,
        DEBUG,
        DEBUG_TESTS,
        TESTS,
        TESTS_COVERAGE,
    };

    vector<string> sourcePaths = { "./src/includes" };
    const string buildPath = "build/";
    string mainPath = "./src";
    string main = "main";
    // -Wshadow
    string flags = "-Wall -Werror -pedantic -Wextra -Wunused -Wuninitialized -Wformat -Wconversion -Wcast-align -Wnull-dereference -Wlogical-op -Wfloat-conversion -Wdouble-promotion -Wsign-conversion -Wsign-promo -Wcast-qual -Wdisabled-optimization -Werror=return-type -Werror=main -Wsuggest-final-methods -std=c++17"; // for debug, add: "-g";
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

            case Mode::DEBUG_TESTS:
                cout << "DEBUG_TESTS";
                sourcePaths.push_back("./tests/includes");
                flags += " -g";
                executeMain = false;
                mainPath = "./tests";
                main = "test";
                break;

            case Mode::TESTS:
                cout << "TESTS";
                sourcePaths.push_back("./tests/includes");
                mainPath = "./tests";
                main = "test";
                break;

            case Mode::TESTS_COVERAGE:
                cout << "TESTS_COVERAGE";
                sourcePaths.push_back("./tests/includes");
                flags += " -fprofile-arcs -ftest-coverage";
                mainPath = "./tests";
                main = "test";
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
    string output = madlib::exec(cmd);
    if (!output.empty()) {
        cout << output << endl;
    }
}

int main(int argc, const char *argv[]) {

    cout << "Build in progress.." << endl;

    Arguments::Mode mode = Arguments::Mode::RELEASE;  // Default mode

    if (argc > 1) {
        string modeArg = argv[1];

        if (modeArg == "RELEASE") {
            mode = Arguments::Mode::RELEASE;
        } else if (modeArg == "DEBUG") {
            mode = Arguments::Mode::DEBUG;
        } else if (modeArg == "DEBUG_TESTS") {
            mode = Arguments::Mode::DEBUG_TESTS;
        } else if (modeArg == "TESTS") {
            mode = Arguments::Mode::TESTS;
        } else if (modeArg == "TESTS_COVERAGE") {
            mode = Arguments::Mode::TESTS_COVERAGE;
        } else {
            cerr << "Invalid mode argument. Usage: " << argv[0] << " [RELEASE|DEBUG|DEBUG_TESTS|TESTS|TESTS_COVERAGE]" << endl;
            return 1;  // Exit with an error code
        }
    }

    vector<string> mainArgsVector;
    for (int i = 2; i < argc; i++) mainArgsVector.push_back(argv[i]);
    const string mainArgs = vector_concat(mainArgsVector, " ");

    Arguments args(mode);

    const vector<string> sourcePaths = args.sourcePaths;
    const string buildPath = args.buildPath;
    const string mainPath = args.mainPath;
    const string main = args.main;
    const string flags = args.flags;
    const string flagsLibs = args.flagsLibs;
    const string cppExtension = args.cppExtension;
    const bool executeMain = args.executeMain;

    vector<string> files;
    for (const string& sourcePath : sourcePaths) {
        vector<string> founds = file_find_by_extensions(sourcePath, { cppExtension });
        files.insert(files.end(), founds.begin(), founds.end());
    }
    files.push_back(mainPath + "/" + main + cppExtension);

    vector<string> oFiles;
    for (const string& file : files) {
        string path = path_normalize(buildPath + path_extract(file));
        // cout << "PATH:" << path << endl;
        if (!file_exists(path)) {
            // cout << "CREATE:" << path << endl;
            file_create_path(path);
        }
        string oFile = buildPath + path_normalize(file_replace_extension(file, "o"));
        if (
            !file_exists(oFile) ||
            file_get_mtime(oFile) < file_get_mtime(file)
        ) {
            ::exec("g++ " + flags + " -c " + file + " -o " + oFile);
        }
        oFiles.push_back(oFile);
    }

    ::exec("g++ " + flags + " -o " + buildPath + main + " " + vector_concat(oFiles) + " " + flagsLibs);
    if (executeMain) ::exec("./" + buildPath + main + " " + mainArgs);

    return 0;
}