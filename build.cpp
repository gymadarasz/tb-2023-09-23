#include <vector>
#include <string>
#include <iostream>

#include "src/includes/madlib/macros.hpp"
#include "src/includes/madlib/sys.hpp"
#include "src/includes/madlib/str.hpp"
#include "src/includes/madlib/files.hpp"
#include "src/includes/madlib/vectors.hpp"
#include "src/includes/madlib/Log.hpp"
#include "src/includes/madlib/ERROR.hpp"
#include "src/includes/madlib/time.hpp"

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
    // NOTE: add -Wsuggest-final-methods only if no overrides in dynamic libraries
    string flags = 
        "-Wall -Werror -pedantic -Wextra -Wunused -Wuninitialized "
        "-Wformat -Wconversion -Wcast-align -Wnull-dereference "
        "-Wlogical-op -Wfloat-conversion -Wdouble-promotion "
        "-Wsign-conversion -Wsign-promo -Wcast-qual "
        "-Wdisabled-optimization -Werror=return-type -Werror=main "
        "-std=c++17";
    const string flagsLibs = "-lX11 -ldl";

    // NOTE: keep it the highest (-O3)
    const string optimizationFlag = " -O3";

    // const string flagsShared = "-shared -fPIC";
    const string hExtension = ".h";
    const string cppExtension = ".cpp";
    const string soExtension = ".so";
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

bool exec_cmd(string cmd) {
    cout << COLOR_INFO "Execute command:" COLOR_DEFAULT " $ " << cmd << endl;
    string output = madlib::exec(cmd);    
    if (!output.empty()) {
        string replaced = output; // TODO: regx_replace(output, "<regular-expression-finds-filenames>", COLOR_FILENAME + "<match>" + COLOR_DEFAULT);
        replaced = str_replace(replaced, "error:", COLOR_ERROR "error:" COLOR_DEFAULT);
        cout << replaced << endl;
        if (replaced != output) {
            LOG("Command output contains error(s).");
            return false;
        }
    }
    if (exec_last_exit_code != 0) {
        LOG("Command exit code is non-zero: " + exec_last_exit_code);
        return false;
    }
    return true;
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
    const string hExtension;
    const string cppExtension;
    const string soExtension;
    const bool executeMain;
    const string mainArgs;

    static vector<string> collectFiles(
        const vector<string>& paths,
        const vector<string>& cppExtensions
    ) {
        vector<string> files;
        for (const string& path : paths) {
            vector<string> founds = file_find_by_extensions(path, cppExtensions);
            files.insert(files.end(), founds.begin(), founds.end());
        }
        return files;
    }

    static void collectDependencies(
        const string& filename, vector<string>& deps,
        const string& hExtension, const string& cppExtension
    ) {
        
        vector<string> matches;
        string contents = file_get_contents(filename);
        if (!regx_match_all("\\n\\s*\\#include\\s*\"(.*)\"", contents, &matches)) return;
        for (size_t i = 1; i < matches.size(); i += 2) {
            string filepath = path_normalize(__DIR__ + "/" + path_extract(filename) + "/" + matches[i]);
            if (vector_contains(deps, filepath)) continue;
            deps.push_back(filepath);
            if (str_ends_with(hExtension, filepath)) {
                string cppFile = file_replace_extension(filepath, cppExtension);
                if (file_exists(cppFile)) {
                    if (vector_contains(deps, filepath)) continue;
                    deps.push_back(cppFile);
                }
            }
            collectDependencies(filepath, deps, hExtension, cppExtension);
        }
        vector_unique(deps);
    }

    static ms_t getLatestModification(
        const string& filename,
        const string& hExtension,
        const string& cppExtension,
        vector<string>& dependencies
    ) {
        ms_t lastModAt = file_get_mtime(filename);
        collectDependencies(filename, dependencies, hExtension, cppExtension);
        for (const string& dependency: dependencies) {
            ms_t depLastModAt = file_get_mtime(dependency);
            if (depLastModAt > lastModAt) lastModAt = depLastModAt;
        }
        return lastModAt;
    }

    static vector<string> buildFiles(
        const vector<string>& files,
        const string& buildPath,
        const string& flags,
        const string& main,
        const string& flagsLibs,
        const string& hExtension,
        const string& cppExtension
    ) {
        int errors = 0;
        vector<string> oFiles;
        for (const string& file : files) {
            string path = path_normalize(buildPath + path_extract(file));
            if (!file_exists(path)) file_create_path(path);

            string oFile = buildPath + path_normalize(file_replace_extension(file, "o"));
            vector<string> dependencies;
            if (
                !file_exists(oFile) ||
                file_get_mtime(oFile) < getLatestModification(file, hExtension, cppExtension, dependencies)  // file_get_mtime(file)
            ) {
                errors += !exec_cmd("g++ " + flags + " -c " + file + " -o " + oFile);
            }
            oFiles.push_back(oFile);
        }
        errors += !exec_cmd("g++ " + flags + " -o " + buildPath + main + " " + vector_concat(oFiles) + " " + flagsLibs);
        if (errors)
            throw ERROR("Compilation fails in " + to_string(errors) + " file(s).");
        return oFiles;
    }

    static vector<string> rebuildHdependenciesAsShared(
        const vector<string>& deps, 
        const string& buildPath, 
        const string& flags,
        const string& hExtension, 
        const string& cppExtension, 
        const string& soExtension
    ) {
        int errors = 0;
        vector<string> soDeps;
        for (const string& dep: deps) {
            string hDep = file_replace_extension(dep, hExtension);
            if (!file_exists(hDep)) continue;
            string cppDep = file_replace_extension(dep, cppExtension);
            if (!file_exists(cppDep)) continue;
            string soDep = buildPath + file_replace_extension(dep, soExtension);
            if (
                !file_exists(soDep) || 
                file_get_mtime(soDep) < file_get_mtime(hDep) ||
                file_get_mtime(soDep) < file_get_mtime(cppDep)
            ) errors += !exec_cmd("g++ " + flags + " -c -fPIC -o " + soDep + " " + cppDep);
            soDeps.push_back(soDep);
        }
        if (errors)
            throw ERROR("Compilation fails in " + to_string(errors) + " file(s).");
        return soDeps;
    }

public:
    Builder(BuildArguments* args, const string& mainArgs):
        sourcePaths(args->sourcePaths),
        sharedPaths(args->sharedPaths),
        buildPath(args->buildPath),
        mainPath(args->mainPath),
        main(args->main),
        flags(args->flags),
        flagsLibs(args->flagsLibs),
        // flagsShared(args.flagsShared),
        hExtension(args->hExtension),
        cppExtension(args->cppExtension),
        soExtension(args->soExtension),
        executeMain(args->executeMain),
        mainArgs(mainArgs)
    {
        int errors = 0;
        ms_t startAt = now();
        cout << "Build start at: " COLOR_DATETIME << ms_to_datetime(startAt) << COLOR_DEFAULT << endl;
        // build source files
        vector<string> files = collectFiles(sourcePaths, { cppExtension });
        files.push_back(mainPath + "/" + main + cppExtension);

        // vector<string> oFiles = 
        buildFiles(files, buildPath, flags, main, flagsLibs, hExtension, cppExtension);
        
        // build shared files
        vector<string> sharedFiles = collectFiles(sharedPaths, { cppExtension });

        vector<string> osFiles;
        for (const string& file: sharedFiles) {
            string path = path_normalize(buildPath + path_extract(file));
            if (!file_exists(path)) file_create_path(path);
            string osFile = buildPath + path_normalize(file_replace_extension(file, soExtension));
            vector<string> dependencies;
            ms_t latestMod = getLatestModification(file, hExtension, cppExtension, dependencies);
            if (
                !file_exists(osFile) ||
                file_get_mtime(osFile) < latestMod // file_get_mtime(file)
            ) {
                
                vector<string> soDeps = rebuildHdependenciesAsShared(dependencies, buildPath, flags, hExtension, cppExtension, soExtension);
                errors += !exec_cmd("g++ " + flags + " -shared -fPIC " + vector_concat(soDeps) + " " + file + " -o " + osFile);
            }
            osFiles.push_back(osFile);
        }
        ms_t finishAt = now();
        cout << "Build finish at: " COLOR_DATETIME << ms_to_datetime(finishAt) << COLOR_DEFAULT << endl;
        cout << "Build in " COLOR_HIGHLIGHT << finishAt - startAt << " ms" COLOR_DEFAULT << endl;

        if (errors) 
            throw ERROR("Build failed in " + to_string(errors) + " file(s).");

        if (executeMain) exec_cmd("./" + buildPath + main + " " + mainArgs);
    }
};

int main(int argc, const char *argv[]) {
    try {

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
        Builder builder(&args, mainArgs);
        static_cast<void>(builder); // hax to fix a cppcheck style warning
    } catch (exception &e) {
        const string errmsg = "Exception in build process: " + string(e.what());
        cerr << errmsg << endl;
        LOG(errmsg);
        return -1;
    }
    
    return 0;
}