#include <iostream>
#include <string>
#include "src/Files.hpp"
#include "src/System.hpp"
#include "src/Vector.hpp"

using namespace std;

void exec(string cmd) {
    cout << "execute: $ " << cmd << endl;
    string output = System::exec(cmd);
    if (!output.empty()) {
        cout << output << endl;
    }
}

int main(/* int argc, char *argv[] */) {

    cout << "Build in progress.." << endl;

    const string sourcePath = "./src";
    const string buildPath = "build/";
    const string main = "main";
    const string flagsDbg = ""; // "-g";
    const string flagsLibs = "-lX11";

    vector<string> files = Files::findByExtensions(sourcePath, { ".cpp" });

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
            exec("g++ " + flagsDbg + " -c " + file + " -o " + oFile);
        }
        oFiles.push_back(oFile);
    }

    exec("g++ " + flagsDbg + " -o " + buildPath + main + " " + Vector::concat(oFiles) + " " + flagsLibs);
    exec("./" + buildPath + main);

    return 0;
}