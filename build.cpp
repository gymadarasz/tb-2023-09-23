#include <iostream>
#include <string>

#include "src/includes/madlib/madlib.hpp"

using namespace std;
using namespace madlib;

class BuildArguments {
public:

    enum Mode {
        RELEASE = 1,
        DEBUG,
        DEBUG_TESTS,
        TESTS,
        TESTS_COVERAGE,
    };

    vector<string> sourcePaths = { "./src/includes" };
    vector<string> sharedPaths = { "./src/shared" };
    const string buildPath = "build/";
    string mainPath = "./src";
    string main = "main";
    // -Wshadow
    string flags = 
        "-Wall -Werror -pedantic -Wextra -Wunused -Wuninitialized "
        "-Wformat -Wconversion -Wcast-align -Wnull-dereference "
        "-Wlogical-op -Wfloat-conversion -Wdouble-promotion "
        "-Wsign-conversion -Wsign-promo -Wcast-qual "
        "-Wdisabled-optimization -Werror=return-type -Werror=main "
        "-Wsuggest-final-methods -std=c++17";
    const string flagsLibs = "-lX11 -ldl";
    const string optimizationFlag = " -O3";
    // const string flagsShared = "-shared -fPIC";
    const string cppExtension = ".cpp";
    bool executeMain = true;

    BuildArguments(const Mode mode = Mode::RELEASE) {   
        cout << "Selected mode: ";
        switch (mode)
        {
            case Mode::DEBUG:
                cout << "DEBUG";
                flags += " -g -O0";
                executeMain = false;
                break;

            case Mode::DEBUG_TESTS:
                cout << "DEBUG_TESTS";
                sourcePaths.push_back("./tests/includes");
                sharedPaths.push_back("./tests/shared");
                flags += " -g -O0";
                executeMain = false;
                mainPath = "./tests";
                main = "test";
                break;

            case Mode::TESTS:
                cout << "TESTS";
                sourcePaths.push_back("./tests/includes");
                sharedPaths.push_back("./tests/shared");
                flags += optimizationFlag;
                mainPath = "./tests";
                main = "test";
                break;

            case Mode::TESTS_COVERAGE:
                cout << "TESTS_COVERAGE";
                sourcePaths.push_back("./tests/includes");
                sharedPaths.push_back("./tests/shared");
                flags += " -fprofile-arcs -ftest-coverage -O0";
                mainPath = "./tests";
                main = "test";
                break;
            
            default:
                cout << "RELEASE";
                flags += optimizationFlag;
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

class Builder {
protected:

    const vector<string> sourcePaths;
    const vector<string> sharedPaths;
    const string buildPath;
    const string mainPath;
    const string main;
    const string flags;
    const string flagsLibs;
    // const string flagsShared;
    const string cppExtension;
    const bool executeMain;
    const string mainArgs;

    static vector<string> collectFiles(
        const vector<string> paths,
        const vector<string> cppExtensions
    ) {
        vector<string> files;
        for (const string& path : paths) {
            vector<string> founds = file_find_by_extensions(path, cppExtensions);
            files.insert(files.end(), founds.begin(), founds.end());
        }
        return files;
    }

    static vector<string> buildFiles(
        const vector<string> files,
        const string buildPath,
        const string flags,
        const string main,
        const string flagsLibs
    ) {
        vector<string> oFiles;
        for (const string& file : files) {
            string path = path_normalize(buildPath + path_extract(file));
            if (!file_exists(path)) file_create_path(path);

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
        return oFiles;
    }

public:
    Builder(BuildArguments& args, const string mainArgs):
        sourcePaths(args.sourcePaths),
        sharedPaths(args.sharedPaths),
        buildPath(args.buildPath),
        mainPath(args.mainPath),
        main(args.main),
        flags(args.flags),
        flagsLibs(args.flagsLibs),
        // flagsShared(args.flagsShared),
        cppExtension(args.cppExtension),
        executeMain(args.executeMain),
        mainArgs(mainArgs)
    {
        // build source files
        vector<string> files = collectFiles(sourcePaths, { cppExtension });
        files.push_back(mainPath + "/" + main + cppExtension);

        vector<string> oFiles = buildFiles(files, buildPath, flags, main, flagsLibs);
        
        // build shared files
        vector<string> sharedFiles = collectFiles(sharedPaths, { cppExtension });

        vector<string> osFiles;
        for (const string& file: sharedFiles) {
            string path = path_normalize(buildPath + path_extract(file));
            if (!file_exists(path)) file_create_path(path);
            string osFile = buildPath + path_normalize(file_replace_extension(file, "so"));
            if (
                !file_exists(osFile) ||
                file_get_mtime(osFile) < file_get_mtime(file)
            ) {
                ::exec("g++ " + flags + " -shared -fPIC " + file + " -o " + osFile);
            }
            osFiles.push_back(osFile);
        }


        if (executeMain) ::exec("./" + buildPath + main + " " + mainArgs);
    }
};

int main(int argc, const char *argv[]) {

    cout << "Build in progress.." << endl;

    BuildArguments::Mode mode = BuildArguments::Mode::RELEASE;  // Default mode

    if (argc > 1) {
        string modeArg = argv[1];

        if (modeArg == "RELEASE") {
            mode = BuildArguments::Mode::RELEASE;
        } else if (modeArg == "DEBUG") {
            mode = BuildArguments::Mode::DEBUG;
        } else if (modeArg == "DEBUG_TESTS") {
            mode = BuildArguments::Mode::DEBUG_TESTS;
        } else if (modeArg == "TESTS") {
            mode = BuildArguments::Mode::TESTS;
        } else if (modeArg == "TESTS_COVERAGE") {
            mode = BuildArguments::Mode::TESTS_COVERAGE;
        } else {
            cerr << "Invalid mode argument. Usage: " << argv[0] << " [RELEASE|DEBUG|DEBUG_TESTS|TESTS|TESTS_COVERAGE]" << endl;
            return 1;  // Exit with an error code
        }
    }

    vector<string> mainArgsVector;
    for (int i = 2; i < argc; i++) mainArgsVector.push_back(argv[i]);
    const string mainArgs = vector_concat(mainArgsVector, " ");

    BuildArguments args(mode);
    Builder builder(args, mainArgs);

    return 0;
}